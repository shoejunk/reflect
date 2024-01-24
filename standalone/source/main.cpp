import stk.log;
import stk.reflect;
import stk.hash;
import std.core;
import <nlohmann/json.hpp>;

using namespace NStk::NReflect;
using namespace NStk::NLog;
using namespace NStk::NHash;
using namespace std;
using namespace nlohmann;

class CTest
{
public:
	CTest() : m_iTest(0) { debugln("Hello World!\n"); }
	CTest(int32_t i)
		: m_iTest(i)
	{
		debugln("Hello {} Worlds!", i);
	}
	void Test() { debugln("Test {}!", m_iTest); }

private:
	int32_t m_iTest;
};

int main()
{
	CReflect oReflect;

	// Register the CTest class with 0 and 1 parameters
	oReflect.Register<CTest, 0>("Test");
	oReflect.Register<CTest, 1>("Test");

	// Open the level file
	ifstream oFile("data/level.json");
	if (!oFile.is_open())
	{
		debugln("Error: Could not open level.json!");
		return 1;
	}

	// Read it into a json object
	json oJson;

	try
	{
		oFile >> oJson;
	}
	catch (json::parse_error& oError)
	{
		debugln("Error: Could not parse level.json: {}", oError.what());
		return 1;
	}

	// Construct an object from each object in the json array
	for (auto& oObject : oJson)
	{
		string sType = oObject["type"];
		oReflect.Construct(sType);
		oReflect.Construct(sType, oObject["data"]);
	}

	auto oEnd = oReflect.end<CTest, "Test"_h>();
	for (auto it = oReflect.begin<CTest, "Test"_h>(); it != oEnd; ++it)
	{
		it->Test();
	}

	return 0;
}
