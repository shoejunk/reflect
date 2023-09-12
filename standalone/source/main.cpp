// STK
#include <log/log.h>
#include <reflect/reflect.h>

// 3rd Party
#include <nlohmann/json.hpp>

// System
#include <fstream>

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

int main()
{
	CReflect oReflect;
	oReflect.Register<CTest, 1>("Test");

	// Open the level file
	std::ifstream oFile("data/level.json");
	if (!oFile.is_open())
	{
		Log("Error: Could not open level.json!\n");
		return 1;
	}

	// Read it into a json object
	nlohmann::json oJson;

	try
	{
		oFile >> oJson;
	}
	catch (nlohmann::json::parse_error& oError)
	{
		Log("Error: Could not parse level.json: %s\n", oError.what());
		return 1;
	}

	// Construct an object from each object in the json array
	for (auto& oObject : oJson)
	{
		std::string sType = oObject["type"];
		oReflect.Construct(sType, oObject["data"]);
	}
	return 0;
}
