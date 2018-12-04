#include "stdafx.h"
#include "CppUnitTest.h"
#include "../A Star Pathfinding/Main.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO: Your test code here
			Assert::AreEqual(hypot(5,5), heuristic_cost_estimate(vec2(0, 0), vec2(5, 5)));
			Assert::AreEqual(hypot(4,4), heuristic_cost_estimate(vec2(1, 1), vec2(5, 5)));
			Assert::AreEqual(hypot(3,3), heuristic_cost_estimate(vec2(2, 2), vec2(5, 5)));
			Assert::AreEqual(hypot(2,2), heuristic_cost_estimate(vec2(3, 3), vec2(5, 5)));

		}

	};
}