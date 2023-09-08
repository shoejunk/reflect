#include <reflect/reflect.h>

#include <log/log.h>

#include <iostream>

using namespace NStk::NReflect;
using namespace NStk::NLog;

class CTest
{
public:
	CTest() { Log("Hello World!\n"); }
};

int main()
{
	CReflect oReflect;
	oReflect.Register<CTest>("Test");
	oReflect.Construct("Test");
	return 0;
}
