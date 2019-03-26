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
			Assert::AreEqual(hypot(5,5), heuristic_cost_estimate(make_shared<sf::Vector2f>(0, 0), make_shared<sf::Vector2f>(5, 5)));
			Assert::AreEqual(hypot(4,4), heuristic_cost_estimate(make_shared<sf::Vector2f>(1, 1), make_shared<sf::Vector2f>(5, 5)));
			Assert::AreEqual(hypot(3,3), heuristic_cost_estimate(make_shared<sf::Vector2f>(2, 2), make_shared<sf::Vector2f>(5, 5)));
			Assert::AreEqual(hypot(2,2), heuristic_cost_estimate(make_shared<sf::Vector2f>(3, 3), make_shared<sf::Vector2f>(5, 5)));

		}
		TEST_METHOD(PathTest1)
		{
			{
				obstacles.push_back(sf::Vector2f(0, 2));
				obstacles.push_back(sf::Vector2f(1, 2));
				obstacles.push_back(sf::Vector2f(2, 2));
				obstacles.push_back(sf::Vector2f(3, 2));
				obstacles.push_back(sf::Vector2f(4, 2));
				obstacles.push_back(sf::Vector2f(5, 2));
				obstacles.push_back(sf::Vector2f(6, 2));
				obstacles.push_back(sf::Vector2f(7, 2));
				//obstacles.push_back(sf::Vector2f(8, 2));

				shared_ptr<sf::Vector2f> start = make_shared<sf::Vector2f>(0, 0);
				shared_ptr<sf::Vector2f> end = make_shared<sf::Vector2f>(mapHeight - 1, mapWidth - 1);
				sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

				auto completed_path = A_Star(window, start, end);
				Assert::IsFalse(completed_path.empty());
			}
		}
		TEST_METHOD(PathTest2)
		{
			{
				obstacles.push_back(sf::Vector2f(2, 0));
				obstacles.push_back(sf::Vector2f(2, 1));
				obstacles.push_back(sf::Vector2f(2, 2));
				obstacles.push_back(sf::Vector2f(2, 3));
				obstacles.push_back(sf::Vector2f(2, 4));
				obstacles.push_back(sf::Vector2f(2, 5));
				//obstacles.push_back(sf::Vector2f(2, 6));
				//obstacles.push_back(sf::Vector2f(2, 7));
				//obstacles.push_back(sf::Vector2f(2, 8));

				shared_ptr<sf::Vector2f> start = make_shared<sf::Vector2f>(0, 0);
				shared_ptr<sf::Vector2f> end = make_shared<sf::Vector2f>(mapHeight - 1, mapWidth - 1);
				sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

				auto completed_path = A_Star(window, start, end);
				Assert::IsFalse(completed_path.empty());
			}
		}
	};
}