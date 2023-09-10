#include <reflect/reflect.h>

#include <log/log.h>

#include <iostream>

using namespace NStk::NReflect;
using namespace NStk::NLog;

class CTest
{
public:
	CTest() { Log("Hello World!\n"); }
	CTest(int32_t i)
	{
		Log("Hello World %d!\n", i);
	}
};

template<>
void TClass<CTest>::Construct(std::vector<CDatum*> const& kaData)
{
	if (kaData.size() == 0)
	{
		m_aObjects.push_back(std::make_unique<CTest>());
	}
	else if (kaData.size() == 1)
	{
		int32_t i = kaData[0]->Get<int32_t>();
		m_aObjects.push_back(std::make_unique<CTest>(i));
	}
	else
	{
		Log("Error: Too many arguments to construct CTest!\n");
	}
}


int main()
{
	CReflect oReflect;
	oReflect.Register<CTest>("Test");
	TDatum<int32_t> oDatum;
	oDatum.Get() = 5;
	std::vector<CDatum*> aData{ &oDatum };
	oReflect.Construct("Test", aData);
	return 0;
}
