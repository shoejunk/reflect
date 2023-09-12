#pragma once

// STK
#include <log/log.h>

// 3rd Party
#include <nlohmann/json.hpp>

// System
#include <memory>			// std::unique_ptr
#include <string>			// std::string
#include <unordered_map>	// std::unordered_map
#include <utility>			// std::pair

// Namespaces
using namespace NStk::NLog;

// NReflect
namespace NStk::NReflect
{
	// Base class for the "Class" class
	class CClassBase
	{
	public:
		virtual void Construct() = 0;
		virtual void Construct(nlohmann::json const& kaData) = 0;
		virtual std::string const& GetClassName() const = 0;
	};

	// Template class for the "Class" class
	template<typename T, size_t kuConstructParams>
	class TClass : public CClassBase
	{
	public:
		TClass<T, kuConstructParams>(std::string sClassName) : m_ksClassName(sClassName) {}

		virtual void Construct() override
		{
			m_aObjects.push_back(std::make_unique<T>());
		}

		// Construct with a vector of data. By default, this just calls the default constructor.
		// In order to use the data, you must override this function in a template specialization
		// of TClass.
		virtual void Construct(nlohmann::json const& kaData) override
		{
			if (!kaData.is_array())
			{
				Log("Error: json object passed to Construct must be an array.\n");
				return;
			}

			if (kaData.size() < kuConstructParams)
			{
				Log("Error: Not enough paramaters to construct %s!\n", m_ksClassName.c_str());
				return;
			}

			if (kaData.size() > kuConstructParams)
			{
				Log("Error: Too many paramaters to construct %s!\n", m_ksClassName.c_str());
				return;
			}

			if constexpr (kuConstructParams == 0)
			{
				m_aObjects.push_back(std::make_unique<T>());
			}
			else if constexpr (kuConstructParams == 1)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0]));
			}
			else if constexpr (kuConstructParams == 2)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0], kaData[1]));
			}
			else if constexpr (kuConstructParams == 3)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0], kaData[1], kaData[2]));
			}
			else if constexpr (kuConstructParams == 4)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3]));
			}
			else if constexpr (kuConstructParams == 5)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3], kaData[4]));
			}
			else if constexpr (kuConstructParams == 6)
			{
				m_aObjects.push_back(std::make_unique<T>(kaData[0], kaData[1], kaData[2], kaData[3], kaData[4], kaData[5]));
			}
		}

		virtual std::string const& GetClassName() const override
		{
			return m_ksClassName;
		}

	private:
		std::string const m_ksClassName;
		std::vector<std::unique_ptr<T>> m_aObjects;
	};

	struct SPairHash
	{
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& p) const
		{
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);

			// Combine the hashes
			return hash1 ^ hash2;
		}
	};

	// Class that holds a map of class names to class objects
	class CReflect
	{
	public:
		template<typename T, size_t kuConstructParams>
		void Register(std::string sClassName)
		{
			m_aClasses[std::pair(sClassName, kuConstructParams)] = std::make_unique<TClass<T, kuConstructParams>>(sClassName);
		}

		void Construct(std::string const& ksClassName)
		{
			auto oIt = m_aClasses.find(std::pair(ksClassName, 0));
			if (oIt == m_aClasses.end())
			{
				Log("Error: Could not find class %s with a 0 parameter constructor!\n", ksClassName.c_str());
				return;
			}
			oIt->second->Construct();
		}

		void Construct(std::string const& ksClassName, nlohmann::json const& kaData)
		{
			if (!kaData.is_array())
			{
				Log("Error: json object passed to Construct must be an array.\n");
				return;
			}
			m_aClasses[std::pair(ksClassName, kaData.size())]->Construct(kaData);
		}

	private:
		std::unordered_map<std::pair<std::string, size_t>, std::unique_ptr<CClassBase>, SPairHash> m_aClasses;
	};
}
