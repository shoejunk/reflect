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
namespace NStk::NReflect
{
	// Base class for the "Class" class
	class CClassBase
	{
	public:
		virtual void Construct() = 0;
		virtual void Construct(json const& kaData) = 0;
		virtual string const& GetClassName() const = 0;
	};

	template<class T>
	class TClassBase : public CClassBase
	{
	public:
		TClassBase<T>(string sClassName) : m_ksClassName(sClassName) {}
		virtual string const& GetClassName() const override
		{
			return m_ksClassName;
		}

		size_t Size() const
		{
			return m_aObjects.size();
		}

		T& operator[](size_t uIndex)
		{
			return *m_aObjects[uIndex];
		}

		T const& operator[](size_t uIndex) const
		{
			return *m_aObjects[uIndex];
		}

	protected:
		vector<unique_ptr<T>> m_aObjects;

	private:
		string const m_ksClassName;
	};

	// Template class for the "Class" class
	template<class T, size_t kuConstructParams>
	class TClass : public TClassBase<T>
	{
	public:
		TClass<T, kuConstructParams>(string sClassName) : TClassBase<T>(sClassName) {}

		virtual void Construct() override
		{
			this->m_aObjects.push_back(make_unique<T>());
		}

		// Construct with a vector of data. By default, this just calls the default constructor.
		// In order to use the data, you must override this function in a template specialization
		// of TClass.
		virtual void Construct(json const& kaData) override
		{
			if (!kaData.is_array())
			{
				errorln("Error: json object passed to Construct must be an array.");
				return;
			}

			if (kaData.size() < kuConstructParams)
			{
				errorln("Error: Not enough parameters to construct {}!", this->GetClassName().c_str());
				return;
			}

			if (kaData.size() > kuConstructParams)
			{
				errorln("Error: Too many parameters to construct {}!", this->GetClassName().c_str());
				return;
			}

			if constexpr (kuConstructParams == 0)
			{
				this->m_aObjects.push_back(make_unique<T>());
			}
			else if constexpr (kuConstructParams == 1)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0]));
			}
			else if constexpr (kuConstructParams == 2)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1]));
			}
			else if constexpr (kuConstructParams == 3)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1], kaData[2]));
			}
			else if constexpr (kuConstructParams == 4)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3]));
			}
			else if constexpr (kuConstructParams == 5)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3], kaData[4]));
			}
			else if constexpr (kuConstructParams == 6)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3], kaData[4], kaData[5]));
			}
			else if constexpr (kuConstructParams == 7)
			{
				this->m_aObjects.push_back(make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3], kaData[4], kaData[5], kaData[6]));
			}
		}
	};

	struct SHashSizeHash
	{
		std::size_t operator()(const std::pair<c_hash, size_t>& p) const
		{
			return p.first.m_hash + p.second;
		}
	};

	class CReflect;

	template<class T, c_hash koHash>
	class TIter
	{
	public:
		using iterator_category = forward_iterator_tag;
		using value_type = CClassBase;
		using pointer = T*;
		using reference = T&;

		TIter<T, koHash>(CReflect& oReflect) : m_oReflect(oReflect), m_uParams(0), m_uIndex(0) {}
		TIter<T, koHash>(CReflect& oReflect, uint32_t uParams, uint32_t uIndex) : m_oReflect(oReflect), m_uParams(uParams), m_uIndex(uIndex) {}

		TIter<T, koHash>& operator++()
		{
			// 1. Increase the index
			// 2. Look for a class with the same hash and number of parameters. If it doesn't exist, stop.
			// 3. If it does exist, check if the index is valid. If it is, stop.
			// 4. If it isn't, increase the number of parameters, reset the index to 0, and go to step 2.
			++m_uIndex;

			while (m_uParams <= 7)
			{
				auto it = m_oReflect.m_aClasses.find(pair(koHash, m_uParams));
				if (it == m_oReflect.m_aClasses.end())
				{
					m_uParams = 7;
					m_uIndex = 0x1fffffff;
					return *this;
				}

				std::unique_ptr<CClassBase>& pClass = it->second;
				TClassBase<T>* pkClass = static_cast<TClassBase<T>*>(pClass.get());
				if (pkClass->Size() > m_uIndex)
				{
					return *this;
				}

				++m_uParams;
				m_uIndex = 0;
			}

			m_uIndex = 0x1fffffff;
			return *this;
		}

		TIter<T, koHash> operator++(int)
		{
			TIter<T, koHash> koTemp = *this;
			++(*this);
			return koTemp;
		}

		bool operator==(TIter<T, koHash> const& koRhs) const
		{
			return m_uParams == koRhs.m_uParams && m_uIndex == koRhs.m_uIndex;
		}

		bool operator!=(TIter<T, koHash> const& koRhs) const
		{
			return m_uParams != koRhs.m_uParams || m_uIndex != koRhs.m_uIndex;
		}

		reference operator*() const
		{
			auto it = m_oReflect.m_aClasses.find(pair(koHash, m_uParams));
			assert(it != m_oReflect.m_aClasses.end());
			std::unique_ptr<CClassBase> const& koClass = it->second;
			TClassBase<T>* pkClass = static_cast<TClassBase<T>*>(koClass.get());
			assert(pkClass);
			return (*pkClass)[m_uIndex];
		}

		pointer operator->()
		{
			auto it = m_oReflect.m_aClasses.find(pair(koHash, m_uParams));
			assert(it != m_oReflect.m_aClasses.end());
			std::unique_ptr<CClassBase>& oClass = it->second;
			TClassBase<T>* pClass = static_cast<TClassBase<T>*>(oClass.get());
			assert(pClass);
			return &(*pClass)[m_uIndex];
		}

	private:
		CReflect& m_oReflect;
		uint32_t m_uParams : 3;
		uint32_t m_uIndex : 29;
	};

	// Class that holds a map of class names to class objects
	export class CReflect
	{
	private:
		static constexpr size_t s_kuMaxConstructParams = 7;

	public:
		template<typename T, size_t kuConstructParams>
		void Register(string sClassName)
		{
			m_aClasses[pair(sClassName, kuConstructParams)] = make_unique<TClass<T, kuConstructParams>>(sClassName);
		}

		void Construct(string const& ksClassName)
		{
			auto oIt = m_aClasses.find(pair(ksClassName, 0));
			if (oIt == m_aClasses.end())
			{
				errorln("Error: Could not find class {} with a 0 parameter constructor!", ksClassName.c_str());
				return;
			}
			oIt->second->Construct();
		}

		void Construct(string const& ksClassName, json const& kaData)
		{
			if (!kaData.is_array())
			{
				errorln("Error: json object passed to Construct must be an array.");
				return;
			}
			m_aClasses[pair(CHash{ ksClassName }, kaData.size())]->Construct(kaData);
		}

		template<class T, CHash koHash>
		TIter<T, koHash> begin()
		{
			return TIter<T, koHash>(*this);
		}

		template<class T, CHash koHash>
		TIter<T, koHash> end()
		{
			return TIter<T, koHash>(*this, 7, 0x1fffffff);
		}

		template<class T, CHash koHash>
		friend class TIter;

	private:
		std::unordered_map<pair<CHash, size_t>, unique_ptr<CClassBase>, SHashSizeHash> m_aClasses;

		
	};
}
