export module stk.reflect;

import stk.hash;
import stk.log;

import <nlohmann/json.hpp>;
#pragma warning(push)
#pragma warning(disable: 5050) // _M_FP_PRECISE is defined in current command line and not in module command line
import std.core;
#pragma warning(pop)

// Namespaces
using namespace std;
using namespace stk;
using namespace nlohmann;

// NReflect
namespace stk
{
	// Base class for the class" class
	class c_class_base
	{
	public:
		virtual ~c_class_base() = default;
		virtual void construct() = 0;
		virtual void construct(json const& data) = 0;
		virtual string const& class_name() const = 0;
	};

	template<class T>
	class c_class_base_typed : public c_class_base
	{
	public:
		virtual ~c_class_base_typed()
		{
			m_objects.clear();
		}

		c_class_base_typed<T>(string class_name) : m_class_name(class_name) {}
		virtual string const& class_name() const override
		{
			return m_class_name;
		}

		size_t size() const
		{
			return m_objects.size();
		}

		T& operator[](size_t index)
		{
			return *m_objects[index];
		}

		T const& operator[](size_t index) const
		{
			return *m_objects[index];
		}

	protected:
		vector<unique_ptr<T>> m_objects;

	private:
		string const m_class_name;
	};

	// Template class for the "class" class
	template<class T, size_t construct_params>
	class c_class : public c_class_base_typed<T>
	{
	public:
		c_class<T, construct_params>(string class_name) : c_class_base_typed<T>(class_name) {}

		virtual void construct() override
		{
			this->m_objects.push_back(make_unique<T>());
		}

		// Construct with a vector of data. By default, this just calls the default constructor.
		// In order to use the data, you must override this function in a template specialization
		// of TClass.
		virtual void construct(json const& data) override
		{
			if (!data.is_array())
			{
				errorln("Error: json object passed to Construct must be an array.");
				return;
			}

			if (data.size() < construct_params)
			{
				errorln("Error: Not enough parameters to construct {}!", this->class_name().c_str());
				return;
			}

			if (data.size() > construct_params)
			{
				errorln("Error: Too many parameters to construct {}!", this->class_name().c_str());
				return;
			}

			if constexpr (construct_params == 0)
			{
				this->m_objects.push_back(make_unique<T>());
			}
			else if constexpr (construct_params == 1)
			{
				this->m_objects.push_back(make_unique<T>(data[0]));
			}
			else if constexpr (construct_params == 2)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1]));
			}
			else if constexpr (construct_params == 3)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1], data[2]));
			}
			else if constexpr (construct_params == 4)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1], data[2], data[3]));
			}
			else if constexpr (construct_params == 5)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1], data[2], data[3], data[4]));
			}
			else if constexpr (construct_params == 6)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1], data[2], data[3], data[4], data[5]));
			}
			else if constexpr (construct_params == 7)
			{
				this->m_objects.push_back(make_unique<T>(data[0], data[1], data[2], data[3], data[4], data[5], data[6]));
			}
		}
	};

	struct s_hash_size_hash
	{
		std::size_t operator()(const std::pair<c_hash, size_t>& p) const
		{
			return p.first.m_hash + p.second;
		}
	};

	class c_reflect;

	template<class T, c_hash hash>
	class c_iter
	{
	public:
		using iterator_category = forward_iterator_tag;
		using value_type = c_class_base;
		using pointer = T*;
		using reference = T&;

		c_iter<T, hash>(c_reflect& reflect) : m_reflect(reflect), m_params(0), m_index(0) {}
		c_iter<T, hash>(c_reflect& reflect, uint32_t params, uint32_t index) : m_reflect(reflect), m_params(params), m_index(index) {}

		c_iter<T, hash>& operator++()
		{
			// 1. Increase the index
			// 2. Look for a class with the same hash and number of parameters. If it doesn't exist, stop.
			// 3. If it does exist, check if the index is valid. If it is, stop.
			// 4. If it isn't, increase the number of parameters, reset the index to 0, and go to step 2.
			++m_index;

			while (m_params <= 7)
			{
				auto it = m_reflect.m_classes.find(pair(hash, m_params));
				if (it == m_reflect.m_classes.end())
				{
					m_params = 7;
					m_index = 0x1fffffff;
					return *this;
				}

				std::unique_ptr<c_class_base>& class_obj = it->second;
				c_class_base_typed<T>* class_ptr = static_cast<c_class_base_typed<T>*>(class_obj.get());
				if (class_ptr->size() > m_index)
				{
					return *this;
				}

				++m_params;
				m_index = 0;
			}

			m_index = 0x1fffffff;
			return *this;
		}

		c_iter<T, hash> operator++(int)
		{
			c_iter<T, hash> temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(c_iter<T, hash> const& rhs) const
		{
			return m_params == rhs.m_params && m_index == rhs.m_index;
		}

		bool operator!=(c_iter<T, hash> const& rhs) const
		{
			return m_params != rhs.m_params || m_index != rhs.m_index;
		}

		reference operator*() const
		{
			auto it = m_reflect.m_classes.find(pair(hash, m_params));
			assert(it != m_reflect.m_classes.end());
			std::unique_ptr<c_class_base> const& class_obj = it->second;
			c_class_base_typed<T>* class_obj = static_cast<c_class_base_typed<T>*>(class_obj.get());
			assert(class_obj);
			return (*class_obj)[m_index];
		}

		pointer operator->()
		{
			auto it = m_reflect.m_classes.find(pair(hash, m_params));
			assert(it != m_reflect.m_classes.end());
			std::unique_ptr<c_class_base>& class_obj = it->second;
			c_class_base_typed<T>* class_ptr = static_cast<c_class_base_typed<T>*>(class_obj.get());
			assert(class_ptr);
			return &(*class_ptr)[m_index];
		}

	private:
		c_reflect& m_reflect;
		uint32_t m_params : 3;
		uint32_t m_index : 29;
	};

	// Class that holds a map of class names to class objects
	export class c_reflect
	{
	private:
		static constexpr size_t s_kuMaxConstructParams = 7;

	public:
		~c_reflect()
		{
			m_classes.clear();
		}

		template<typename T, size_t construct_params>
		void register_class(string class_name)
		{
			m_classes[pair(class_name, construct_params)] = make_unique<c_class<T, construct_params>>(class_name);
		}

		void construct(string const& class_name)
		{
			auto oIt = m_classes.find(pair(class_name, 0));
			if (oIt == m_classes.end())
			{
				errorln("Error: Could not find class {} with a 0 parameter constructor!", class_name.c_str());
				return;
			}
			oIt->second->construct();
		}

		void construct(string const& class_name, json const& data)
		{
			if (!data.is_array())
			{
				errorln("Error: json object passed to Construct must be an array.");
				return;
			}
			m_classes[pair(c_hash{ class_name }, data.size())]->construct(data);
		}

		template<class T, c_hash hash>
		c_iter<T, hash> begin()
		{
			return c_iter<T, hash>(*this);
		}

		template<class T, c_hash hash>
		c_iter<T, hash> end()
		{
			return c_iter<T, hash>(*this, 7, 0x1fffffff);
		}

		template<class T, c_hash hash>
		friend class c_iter;

	private:
		std::unordered_map<pair<c_hash, size_t>, unique_ptr<c_class_base>, s_hash_size_hash> m_classes;
	};
}
