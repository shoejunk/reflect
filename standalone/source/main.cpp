import stk.log;
import stk.reflect;
import stk.hash;
#pragma warning(push)
#pragma warning(disable: 5050) // _M_FP_PRECISE is defined in current command line and not in module command line
import std.core;
#pragma warning(pop)
import <nlohmann/json.hpp>;

using namespace stk;
using namespace std;
using namespace nlohmann;

class c_test
{
public:
	c_test() : m_test(0) { debugln("Hello World!"); }
	c_test(int32_t i)
		: m_test(i)
	{
		debugln("Hello {} Worlds!", i);
	}
	void test() { debugln("Test {}!", m_test); }

private:
	int32_t m_test;
};

int main()
{
	c_reflect reflect;

	// Register the CTest class with 0 and 1 parameters
	reflect.register_class<c_test, 0>("Test");
	reflect.register_class<c_test, 1>("Test");

	// Open the level file
	ifstream file("data/level.json");
	if (!file.is_open())
	{
		debugln("Error: Could not open level.json!");
		return 1;
	}

	// Read it into a json object
	json json;

	try
	{
		file >> json;
	}
	catch (json::parse_error& error)
	{
		debugln("Error: Could not parse level.json: {}", error.what());
		return 1;
	}

	// Construct an object from each object in the json array
	for (auto& object : json)
	{
		string type = object["type"];
		reflect.construct(type);
		reflect.construct(type, object["data"]);
	}

	auto end = reflect.end<c_test, "Test"_h>();
	for (auto it = reflect.begin<c_test, "Test"_h>(); it != end; ++it)
	{
		it->test();
	}

	return 0;
}
