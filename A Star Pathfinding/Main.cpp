#include <SFML/Graphics.hpp>
#include "ParticleSystem.cpp"
#include "VectorMath.cpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <math.h>

using namespace std;

typedef pair<int, int> vec2;

#define M_PI 3.14159265358979323846   // pi
const int mapHeight = 50;
const int mapWidth = 50;
const int cellHeightWidth = 10;
const int cellHeightWidthHalf = cellHeightWidth/2;
const int screenWidth = mapWidth * cellHeightWidth;
const int screenHeight = mapHeight * cellHeightWidth;


class Enemy {
public:
	float health = 100;
	bool toDelete = false;
	const float mass = 1;
	const float max_force = 10;
	float max_speed = 0.5;
	float heading; //angle
	float boundingRadius = cellHeightWidth / 2;
	sf::Vector2f center;
	sf::Vector2f velocity = sf::Vector2f(0.1,0);
	sf::Vector2f forward = sf::Vector2f(1, 0);
	sf::Vector2f side;
	sf::Vector2f up = sf::Vector2f(0,1);
	float speed = 5;
	sf::RectangleShape baseRect;
	sf::Vector2f position;
	vector<sf::Vector2f> currentPath;
	float rotation;
	const float rotationOffset = 90;
	Enemy() {}
	~Enemy() {}
	Enemy(sf::Vector2f pos) {
		baseRect = sf::RectangleShape(sf::Vector2f(cellHeightWidth, cellHeightWidth));
		position = pos;
		baseRect.setOrigin(sf::Vector2f(baseRect.getSize().x / 2, baseRect.getSize().y / 2));
		baseRect.setPosition(pos.x, pos.y);
		baseRect.setFillColor(sf::Color::Blue);
		baseRect.setOutlineColor(sf::Color::Blue);
		baseRect.setOutlineThickness(1);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(baseRect);
	}

	void rotateToFaceTarget(sf::Vector2<float> target) {
		float dx = position.x - target.x;
		float dy = position.y - target.y;

		rotation = (atan2(dy, dx)) * 180 / M_PI;
		baseRect.setRotation(rotation + rotationOffset);
	}

	sf::Vector2f seek(sf::Vector2f from, sf::Vector2f to, float length) {
		return Math::normalize(from - to) * length;
	}
	sf::Vector2f seek(sf::Vector2f to) {
		return Math::normalize(to - position) * max_speed;
	}

	void update() {

		if (health <= 0)
		{
			toDelete = true;
		}

		sf::Vector2f desired_velocity;
		sf::Vector2f steering;
		//path progression
		if (Math::length(currentPath[0] - position) < 0.25)
		{
			currentPath.erase(currentPath.begin());
		}
		if (currentPath.empty())
			currentPath.push_back(sf::Vector2f(500, 500));

		//seek
		desired_velocity = seek(currentPath[0]);
		steering = desired_velocity - velocity;

		//combine forces
		steering = Math::truncate(steering, max_force);
		steering = steering / mass;
		velocity = Math::truncate(velocity + steering, max_speed);
		UpdatePosition(position + velocity);
	}

	void UpdatePosition(sf::Vector2f p){
		position = p;
		baseRect.setPosition(position);
	}
	void Hit(const float dmg) {
		health -= dmg;
	}
};

class Bullet {
	sf::RectangleShape baseRect;
	shared_ptr<Enemy> target;
	float rotation;
	sf::Vector2f velocity;
	const float rotationOffset = 90;
	float max_speed = 10;
	const float max_force = 20;
	float distanceTravelled = 0;
	float distanceToTravel = 0;
	int steps = 0;
	const float damage = 0.1;

public:
	bool toDelete = false;
	sf::Vector2f position;

	Bullet() {}
	~Bullet() {}
	Bullet(sf::Vector2f pos, shared_ptr<Enemy> tar, float _distanceToTravel) {
		baseRect = sf::RectangleShape(sf::Vector2f(cellHeightWidthHalf, cellHeightWidthHalf));
		position = pos;
		target = tar;
		distanceToTravel = _distanceToTravel;
		baseRect.setOrigin(sf::Vector2f(baseRect.getSize().x / 2, baseRect.getSize().y / 2));
		baseRect.setPosition(pos.x, pos.y);
		baseRect.setFillColor(sf::Color::White);
		baseRect.setOutlineColor(sf::Color::White);
		baseRect.setOutlineThickness(1);
		baseRect.rotate(rotation);
	}

	sf::Vector2f seek(sf::Vector2f to) {
		return Math::normalize(to - position) * max_speed;
	}

	void print() {
		cout << position << endl << velocity << endl;
	}

	void update() {
		sf::Vector2f desired_velocity;
		sf::Vector2f steering;
		//path progression
		if (Math::length(target->position - position) < 0.2 ||
			distanceTravelled > distanceToTravel)
		{
			toDelete = true;
			target->Hit(damage);
		}

		//seek
		desired_velocity = seek(target->position);
		steering = desired_velocity - velocity;

		//combine forces
		steering = Math::truncate(steering, max_force);
		velocity = Math::truncate(velocity + steering, max_speed);
		UpdatePosition(position + velocity);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(baseRect);
	}

	void UpdatePosition(sf::Vector2f p) {
		distanceTravelled += Math::length(p - position);
		steps++;
		position = p;
		baseRect.setPosition(position);
	}
};

/*
1 	White 	0-4
2 	Green 	5-10
3 	Blue 	11-15
4 	Purple 	16-49
5 	Yellow 	50-60
6 	Orange 	61-65
7 	Dark Orange 	66-100
8 	Pearlescent 	101+
*/
class Turret {
	sf::RectangleShape baseRect;
	sf::RectangleShape topRect;
	float rotation;
	double cooldownTime = 1000;
	double cooldownTimer = 0;
	bool isReady = true;
	const float rotationOffset = 90;
	list<Bullet> deadAmmo;
public:
	sf::Vector2f position;
	list<shared_ptr<Bullet>> ammo;
	shared_ptr<Enemy> target;
	bool activeTarget;
	Turret() {}
	~Turret() {}
	Turret(sf::Vector2f pos) {
		baseRect = sf::RectangleShape(sf::Vector2f(cellHeightWidthHalf, cellHeightWidthHalf));
		topRect = sf::RectangleShape(sf::Vector2f(cellHeightWidthHalf, cellHeightWidthHalf*3));
		position = pos;
		topRect.setOrigin(sf::Vector2f(topRect.getSize().x / 2, topRect.getSize().y / 2));
		baseRect.setOrigin(sf::Vector2f(baseRect.getSize().x / 2, baseRect.getSize().y / 2));
		baseRect.setPosition(pos.x, pos.y);
		topRect.setPosition(pos.x, pos.y);
		baseRect.setFillColor(sf::Color::Blue);
		baseRect.setOutlineColor(sf::Color::Blue);
		baseRect.setOutlineThickness(1);
		topRect.setFillColor(sf::Color::Cyan);
		topRect.rotate(rotation);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(baseRect);
		window.draw(topRect);
		for each (auto b in ammo)
		{
			b->draw(window);
		}
	}

	void rotateToFaceTarget(sf::Vector2<float> target) {
		float dx = position.x - target.x;
		float dy = position.y - target.y;

		rotation = (atan2(dy, dx)) * 180 / M_PI;
		topRect.setRotation(rotation + rotationOffset);
	}

	void update(double elapsedTime) {
		if (!isReady) {
			cooldownTimer += elapsedTime;
			if (cooldownTimer > cooldownTime)
			{
				isReady = true;
				cooldownTimer = 0;
			}
		}
		else
		{
			// SHOOT
			if (activeTarget && isReady)
			{
				ammo.push_back(make_shared<Bullet>(position, target, Math::length(target->position - position)));
				isReady = false;
			}
		}
		std::list<shared_ptr<Bullet>>::iterator it = ammo.begin();
		while (it != ammo.end())
		{
			if (it->get()->toDelete)
			{
				ammo.erase(it);
			}
			it->get()->update();
			it++;
		}
		ammo.remove_if([](shared_ptr<Bullet> elem) { if (elem->toDelete) return true; else return false; });
	}
};

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
			/*sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });

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
			}*/

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
		}
	}
	window.display();
	//system("pause");
}

void draw_gridlines(sf::RenderWindow& window)
{
	for (int y = mapHeight; y > -1; --y) {
		for (int x = 0; x < mapWidth; ++x) {
			float tlx = x * cellHeightWidth;
			float tly = y * cellHeightWidth;
			sf::Color trans(255, 255, 255, 50);

			//GRIDLINES
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(tlx, tly),trans),
				sf::Vertex(sf::Vector2f(tlx + cellHeightWidth, tly),trans),
				sf::Vertex(sf::Vector2f(tlx, tly),trans),
				sf::Vertex(sf::Vector2f(tlx, tly + cellHeightWidth),trans)
			};
			window.draw(line, 4, sf::Lines);
		}
	}
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
std::vector<Turret>turrets;
std::list<shared_ptr<Enemy>>enemies;

int main(int argc, char **argv)
{
	int map[mapHeight][mapWidth];

	vec2 start = vec2(0, 0);
	vec2 end = vec2(4, 4);

	if (argc == 5) {
		start.first  = atoi(argv[1]);
		start.second = atoi(argv[2]);
		end.first    = atoi(argv[3]);
		end.second   = atoi(argv[4]);
	}

	for (int y = 0; y < mapHeight; y+=10)
	{
		for (int x = 0; x < mapWidth; x+=10)
		{
			turrets.push_back(Turret(sf::Vector2f(x * cellHeightWidth + cellHeightWidthHalf, y * cellHeightWidth + cellHeightWidthHalf)));
		}
	}

	for (int e = 0; e < 1; e++)
	{
		enemies.push_back(make_shared<Enemy>(sf::Vector2f(100, 100)));
	}

    // create the window
    sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");
	
	// create screen buffer margin
	float buffer_width = 50;
	sf::RectangleShape screen_boundary(sf::Vector2f(mapWidth* cellHeightWidth - buffer_width, 
													mapHeight* cellHeightWidth - buffer_width));
	screen_boundary.setPosition(sf::Vector2f(buffer_width, buffer_width));

	// create screen background
	sf::Texture bgtexture;
	if (!bgtexture.loadFromFile("Resources/output-0.png"))
	{
		cout << "File not found" << endl;
	}
	bgtexture.setSmooth(true);
	sf::Sprite bgsprite;
	bgsprite.setTexture(bgtexture);

	// Create goal texture
	sf::Texture goalTexture;
	if (!goalTexture.loadFromFile("Resources/output-13.png"))
	{
		cout << "File not found" << endl;
	}
	goalTexture.setSmooth(true);
	sf::Sprite goalSprite;
	goalSprite.setTexture(goalTexture);
	goalSprite.setPosition(sf::Vector2f(screenHeight - goalTexture.getSize().x / 2,
										screenWidth - goalTexture.getSize().y / 2));

	//Find Path to Goal
	std::vector<vec2> completed_path = A_Star(window, start, end);

	//Give Path to Enemies
	auto it = enemies.begin();
	for (int e = 0; e < 1; e++)
	{
		for (vec2 v : completed_path)
		{
			it->get()->currentPath.insert(it->get()->currentPath.begin(), sf::Vector2f(v.first * cellHeightWidth, v.second * cellHeightWidth));
		}
		it++;
	}

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

			// New Path
			if (event.mouseButton.button == sf::Mouse::Button::Left && event.type == event.MouseButtonPressed)
			{
				end.first = sf::Mouse::getPosition(window).x / cellHeightWidth;
				end.second = sf::Mouse::getPosition(window).y / cellHeightWidth;
				completed_path = A_Star(window, start, end);
				it = enemies.begin();
				while(it != enemies.end())
				{
					it->get()->UpdatePosition(sf::Vector2f(0, 0));
					it->get()->currentPath.clear();

					for (vec2 v : completed_path)
					{
						it->get()->currentPath.insert(it->get()->currentPath.begin(), sf::Vector2f(v.first * cellHeightWidth, v.second * cellHeightWidth));
					}
					it++;
				}
			}
        }

		// make the particle system emitter follow the mouse
		sf::Vector2f mouse = (sf::Vector2f)sf::Mouse::getPosition(window);
        // update it
        sf::Time elapsed = clock.restart();
        particles.update(elapsed);

        // draw it
		window.draw(bgsprite);
		window.draw(goalSprite);
		//display_route2D(window, completed_path, end, (sf::Vector2i)mouse);
		draw_gridlines(window);
		particles.setEmitter(window.mapPixelToCoords((sf::Vector2i)mouse));
        window.draw(particles);

		for (size_t i = 0; i < turrets.size(); i++)
		{
			if (!enemies.empty())
			{
				turrets[i].target = enemies.front();
				turrets[i].activeTarget = true;
				turrets[i].rotateToFaceTarget(enemies.begin()->get()->position); //DEBUG
			}
			else 
			{
				turrets[i].activeTarget = false;
			}
			turrets[i].update(elapsed.asMilliseconds());
			turrets[i].draw(window);
		}

		it = enemies.begin();
		while (it != enemies.end())
		{
			it->get()->update();
			//e.UpdatePosition(Math::clamp(e.position, screen_boundary.getSize(), screen_boundary.getPosition()));
			it->get()->draw(window);
			it++;
		}

		enemies.remove_if([](shared_ptr<Enemy> elem) { if (elem->toDelete) return true; else return false; });

        window.display();
    }

    return 0;
}