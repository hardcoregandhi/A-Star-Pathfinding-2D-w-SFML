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
		TEST_METHOD(PathTest1)
		{
			{
				obstacles.push_back(vec2(0, 2));
				obstacles.push_back(vec2(1, 2));
				obstacles.push_back(vec2(2, 2));
				obstacles.push_back(vec2(3, 2));
				obstacles.push_back(vec2(4, 2));
				obstacles.push_back(vec2(5, 2));
				obstacles.push_back(vec2(6, 2));
				obstacles.push_back(vec2(7, 2));
				//obstacles.push_back(vec2(8, 2));

				vec2 start = vec2(0, 0);
				vec2 end = vec2(mapHeight - 1, mapWidth - 1);
				sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

				std::vector<vec2> completed_path = A_Star(window, start, end);
				Assert::IsFalse(completed_path.empty());
			}
		}
		TEST_METHOD(PathTest2)
		{
			{
				obstacles.push_back(vec2(2, 0));
				obstacles.push_back(vec2(2, 1));
				obstacles.push_back(vec2(2, 2));
				obstacles.push_back(vec2(2, 3));
				obstacles.push_back(vec2(2, 4));
				obstacles.push_back(vec2(2, 5));
				//obstacles.push_back(vec2(2, 6));
				//obstacles.push_back(vec2(2, 7));
				//obstacles.push_back(vec2(2, 8));

				vec2 start = vec2(0, 0);
				vec2 end = vec2(mapHeight - 1, mapWidth - 1);
				sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

				std::vector<vec2> completed_path = A_Star(window, start, end);
				Assert::IsFalse(completed_path.empty());
			}
		}
	};
}