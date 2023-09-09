#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace NStk::NReflect
{
	template<typename T>
	class TDatum;

	class CDatum
	{
	public:
		template<typename T>
		T& Get()
		{
			return static_cast<TDatum<T>*>(this)->Get();
		}

		template<typename T>
		T const& Get() const
		{
			return static_cast<TDatum<T> const*>(this)->Get();
		}
	};

	template<typename T>
	class TDatum : public CDatum
	{
	public:
		T& Get()
		{
			return m_mDatum;
		}

		T const& Get() const
		{
			return m_mDatum;
		}

	private:
		T m_mDatum;
	};

	class CClassBase
	{
	public:
		virtual void Construct() = 0;
		virtual void Construct(std::vector<CDatum*> const& kaData) = 0;
		virtual std::string const& GetClassName() const = 0;
	};

	template<typename T>
	class TClass : public CClassBase
	{
	public:
		TClass<T>(std::string sClassName) : m_ksClassName(sClassName) {}

		virtual void Construct() override
		{
			m_aObjects.push_back(std::make_unique<T>());
		}

		// Construct with a vector of data. By default, this just calls the default constructor.
		// In order to use the data, you must override this function in a template specialization
		// of TClass.
		virtual void Construct(std::vector<CDatum*> const& kaData) override
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
		template<typename T>
		void Register(std::string sClassName)
		{
			m_aClasses[sClassName] = std::make_unique<TClass<T>>(sClassName);
		}

		void Construct(std::string const& ksClassName)
		{
			m_aClasses[ksClassName]->Construct();
		}

		void Construct(std::string const& ksClassName, std::vector<CDatum*> const& kaData)
		{
			m_aClasses[ksClassName]->Construct(kaData);
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<CClassBase>> m_aClasses;
	};
}
