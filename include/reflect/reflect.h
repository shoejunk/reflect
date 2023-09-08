#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace NStk::NReflect
{
	class CClassBase
	{
	public:
		virtual void Construct() = 0;
		virtual std::string const& GetClassName() const = 0;
	};

	template<class T>
	class TClass : public CClassBase
	{
	public:
		TClass<T>(std::string sClassName) : m_ksClassName(sClassName) {}
		virtual void Construct() override
		{
			m_aObjects.push_back(std::make_unique<T>());
		}

		virtual std::string const& GetClassName() const override
		{
			return m_ksClassName;
		}

	private:
		std::string const m_ksClassName;
		std::vector<std::unique_ptr<T>> m_aObjects;
	};

	class CReflect
	{
	public:
		template<class T>
		void Register(std::string sClassName)
		{
			m_aClasses[sClassName] = std::make_unique<TClass<T>>(sClassName);
		}

		void Construct(std::string const& ksClassName)
		{
			m_aClasses[ksClassName]->Construct();
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<CClassBase>> m_aClasses;
	};
}
