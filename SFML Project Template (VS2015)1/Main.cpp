#include <SFML/Graphics.hpp>
#include "ParticleSystem.cpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <vector>
using namespace std;

typedef pair<int, int> vec2;

const int mapHeight = 50;
const int mapWidth = 50;
const int cellHeightWidth = 10;

std::vector<vec2> reconstruct_path(std::map<vec2, vec2> cameFrom,
	vec2 current) {
	std::vector<vec2> total_path;
	total_path.push_back(current);
	while (cameFrom.find(current) != cameFrom.end()) {
		current = cameFrom[current];
		total_path.push_back(current);
	}
	return total_path;
}

double heuristic_cost_estimate(vec2 from, vec2 to) {
	return hypot(from.first - to.first, from.second - to.second);
}

std::vector<vec2> getNeighbours(vec2 pos) {
	// replace when adding obstacles
	std::vector<vec2> temp;
	if (pos.first != 0)
		temp.push_back(vec2(pos.first - 1, pos.second));
	if (pos.first != mapWidth - 1)
		temp.push_back(vec2(pos.first + 1, pos.second));
	if (pos.second != 0)
		temp.push_back(vec2(pos.first, pos.second - 1));
	if (pos.second != mapHeight - 1)
		temp.push_back(vec2(pos.first, pos.second + 1));

	// topleft
	if (pos.first != 0 && pos.second != 0)
		temp.push_back(vec2(pos.first - 1, pos.second - 1));
	// bottomleft
	if (pos.first != 0 && pos.second != mapHeight)
		temp.push_back(vec2(pos.first - 1, pos.second + 1));
	// bottomright
	if (pos.first != mapWidth && pos.second != mapHeight)
		temp.push_back(vec2(pos.first + 1, pos.second + 1));
	// topright
	if (pos.first != mapWidth && pos.second != 0)
		temp.push_back(vec2(pos.first + 1, pos.second - 1));

	std::vector<vec2> toRemove;
	std::vector<vec2> obstacles;
	//ADD OBSTACLES HERE TODO:Implement true map editor
	/*for (size_t i = 1; i < 100; i++)
	{
		obstacles.push_back(vec2(i, 10));
	}

	for (size_t i = 0; i < 25; i++)
	{
		obstacles.push_back(vec2(i, 13));
	}
	for (size_t i = 1; i < 100; i++)
	{
		obstacles.push_back(vec2(i, 16));
	}*/
	for (vec2 pos : temp)
		for (vec2 ob : obstacles)
			if (pos == ob)
			{
				toRemove.push_back(pos);
			}
	for (vec2 tr : toRemove)
	{
		std::vector<vec2>::iterator findInTemp =
			std::find(temp.begin(), temp.end(), tr);
		temp.erase(findInTemp);
	}

	return temp;
}

int dist_between(vec2 current, vec2 neighbour) {
	return 1; // Replace when adding obstacles
}

int find_closest_to_goal(std::vector<vec2> set, vec2 goal) {
	int current_lowest_index = 0;
	int current_lowest_score = heuristic_cost_estimate(set[0], goal);
	int score;
	for (int i = 1; i < set.size(); ++i) {
		score = heuristic_cost_estimate(set[i], goal);
		if (score < current_lowest_score) {
			current_lowest_index = i;
			current_lowest_score = score;
		}
	}

	return current_lowest_index;
}

void display_route2D(sf::RenderWindow& window, std::vector<vec2> path, vec2 goal, sf::Vector2i mouse) {

	int mouseCellX = mouse.x / cellHeightWidth;
	int mouseCellY = mouse.y / cellHeightWidth;

	for (int y = mapHeight; y > -1; --y) {
		for (int x = 0; x < mapWidth; ++x) {
			float tlx = x * cellHeightWidth;
			float tly = y * cellHeightWidth;

			//GRID
			sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });

			std::vector<vec2>::iterator findInPath =
				std::find(path.begin(), path.end(), vec2(x, y));
			if (findInPath != path.end()) {
				rect.setFillColor(sf::Color::Red);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (vec2(x, y) == goal) {
				rect.setFillColor(sf::Color::Yellow);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (mouseCellX == x && mouseCellY == y)
			{
				rect.setFillColor(sf::Color::White);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}

			//GRIDLINES
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(tlx, tly)),
				sf::Vertex(sf::Vector2f(tlx + cellHeightWidth, tly)),
				sf::Vertex(sf::Vector2f(tlx, tly)),
				sf::Vertex(sf::Vector2f(tlx, tly + cellHeightWidth))
			};
			window.draw(line, 4, sf::Lines);

		}
	}
}

void display_route_in_progress(
	sf::RenderWindow& window, 
	std::list<vec2> closedSet, 
	std::vector<vec2> openSet,
	std::map<vec2, double> gScore,
	std::map<vec2, double> fScore,
	vec2 current, 
	vec2 goal) {

	sf::Font MyFont;
	MyFont.loadFromFile("arial.ttf");
	sf::Text text("Hello", MyFont, cellHeightWidth/4);
	text.setColor(sf::Color::Black);
	window.clear();
	for (int y = mapHeight; y > -1; --y) {
		for (int x = 0; x < mapWidth; ++x) {
			float tlx = x * cellHeightWidth;
			float tly = y * cellHeightWidth;

			//CELLS
			sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });

			std::vector<vec2>::iterator findInOpenSet =
				std::find(openSet.begin(), openSet.end(), vec2(x,y));
			std::list<vec2>::iterator findInClosedSet =
				std::find(closedSet.begin(), closedSet.end(), vec2(x,y));

			if (findInOpenSet != openSet.end()) {
				rect.setFillColor(sf::Color::Blue);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (vec2(x, y) == goal) 
			{
				rect.setFillColor(sf::Color::Green);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (vec2(x, y) == current)
			{
				rect.setFillColor(sf::Color::Yellow);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (findInClosedSet != closedSet.end()) {
				rect.setFillColor(sf::Color::Red);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}

			std::map<vec2, double>::iterator findInGScore = fScore.find(vec2(x,y));
			if (findInGScore != fScore.end()) {
				text.setString(to_string(findInGScore->second));
			}
			text.setPosition(tlx, tly);
			window.draw(text);
			
			//GRIDLINES
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(tlx, tly)),
				sf::Vertex(sf::Vector2f(tlx + cellHeightWidth, tly)),
				sf::Vertex(sf::Vector2f(tlx, tly)),
				sf::Vertex(sf::Vector2f(tlx, tly + cellHeightWidth))
			};
			window.draw(line, 4, sf::Lines);

		}
	}
	window.display();
	//system("pause");
}

vec2 find_lowest_node_by_fScore(std::vector<vec2> openSet,
	std::map<vec2, double> fScore) {
	double lowest_score = heuristic_cost_estimate(vec2(0,0), vec2(mapWidth,mapHeight));
	vec2 lowest_node;

	for (const std::pair<vec2, double> fff : fScore) {
		for (vec2 v : openSet) {
			if (v == fff.first) {
				if (fff.second < lowest_score) {
					lowest_score = fff.second;
					lowest_node = fff.first;
				}
			}
		}
	}
	return lowest_node;
}

std::vector<vec2> A_Star(sf::RenderWindow& window, vec2 start, vec2 goal) {
	// The set of nodes already evaluated
	std::list<vec2> closedSet;

	// The set of currently discovered nodes that are not evaluated yet.
	// Initially, only the start node is known.
	std::vector<vec2> openSet;
	openSet.push_back(start);

	// For each node, which node it can most efficiently be reached from.
	// If a node can be reached from many nodes, cameFrom will eventually contain
	// the most efficient previous step.
	std::map<vec2, vec2> cameFrom; // := an empty map

	// For each node, the cost of getting from the start node to that node.
	std::map<vec2, double> gScore; // := map with default value of Infinity

	// The cost of going from start to start is zero.
	gScore[start] = 0;

	// For each node, the total cost of getting from the start node to the goal
	// by passing by that node. That value is partly known, partly heuristic.
	std::map<vec2, double> fScore;
	// fScore := map with default value of Infinity

	// For the first node, that value is completely heuristic.
	fScore[start] = heuristic_cost_estimate(start, goal);

	while (!openSet.empty()) {
		// std::system("clear");
		// display_map(openSet, closedSet, goal);

		vec2 current = find_lowest_node_by_fScore(openSet, fScore);
		if (current == goal) {
			return reconstruct_path(cameFrom, current);
		}
		std::vector<vec2>::iterator findInOpenSet =
			std::find(openSet.begin(), openSet.end(), current);

		openSet.erase(findInOpenSet);
		closedSet.push_back(current);

		std::vector<vec2> neighbours = getNeighbours(current);
		for (vec2 neighbour : neighbours) {

			std::list<vec2>::iterator findInClosedSet =
				std::find(closedSet.begin(), closedSet.end(), neighbour);

			if (findInClosedSet != closedSet.end()) {
				continue; // Ignore the neighbour which is already evaluated.
			}

			// The distance from start to a neighbour
			double tentative_gScore;
			std::map<vec2, double>::iterator findInGScore = gScore.find(current);
			if (findInGScore != gScore.end()) {
				tentative_gScore = heuristic_cost_estimate(start, neighbour);
			}
			else
				tentative_gScore = heuristic_cost_estimate(current, neighbour);

			std::vector<vec2>::iterator findInOpenSet =
				std::find(openSet.begin(), openSet.end(), neighbour);
			if (findInOpenSet == openSet.end()) {
				openSet.push_back(neighbour);
			}
			else if (tentative_gScore >= gScore[neighbour]) {
				continue; // This is not a better path.
			}

			// This path is the best until now. Record it!
			cameFrom.insert(std::make_pair(neighbour, current));
			gScore.insert(std::make_pair(neighbour, tentative_gScore));
			pair<std::map<vec2, double>::iterator, bool> insert = fScore.insert(
				std::make_pair(neighbour, gScore[neighbour] + heuristic_cost_estimate(
					neighbour, goal)));
		}
		display_route_in_progress(window, closedSet, openSet, gScore, fScore, current, goal);
	}
	return std::vector<vec2>();
}

int main(int argc, char **argv)
{
	int map[mapHeight][mapWidth];

	vec2 start = vec2(2, 2);
	vec2 end = vec2(25, 28);

	if (argc == 5) {
		start.first  = atoi(argv[1]);
		start.second = atoi(argv[2]);
		end.first    = atoi(argv[3]);
		end.second   = atoi(argv[4]);
	}

    // create the window
    sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

	std::vector<vec2> completed_path = A_Star(window, start, end);

    // create the particle system
    ParticleSystem particles(1000);

    // create a clock to track the elapsed time
    sf::Clock clock;

    // run the main loop
    while (window.isOpen())
    {
		window.clear();

        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
			if (event.mouseButton.button == sf::Mouse::Button::Left && event.type == event.MouseButtonPressed)
			{
				end.first = sf::Mouse::getPosition(window).x / cellHeightWidth;
				end.second = sf::Mouse::getPosition(window).y / cellHeightWidth;
				completed_path = A_Star(window, start, end);
			}
        }

		// make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition(window);

        // update it
        sf::Time elapsed = clock.restart();
        particles.update(elapsed);

        // draw it
		//display_route(completed_path, end);

		display_route2D(window, completed_path, end, mouse);
		particles.setEmitter(window.mapPixelToCoords(mouse));

        window.draw(particles);
        window.display();
    }

    return 0;
}