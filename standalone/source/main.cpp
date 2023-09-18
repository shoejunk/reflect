import stk.log;
import stk.reflect;

// 3rd Party
import <nlohmann/json.hpp>;

// System
import std.core;

using namespace NStk::NReflect;
using namespace NStk::NLog;

class CTest
{
public:
	CTest() { Log("Hello World!\n"); }
	CTest(int32_t i)
	{
		Log("Hello %d Worlds!\n", i);
	}
};

int main()
{
	CReflect oReflect;

	// Register the CTest class with 0 and 1 parameters
	oReflect.Register<CTest, 0>("Test");
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
		oReflect.Construct(sType);
		oReflect.Construct(sType, oObject["data"]);
	}
	return 0;
}
