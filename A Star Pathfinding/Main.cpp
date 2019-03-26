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
#pragma warning(disable: 4244)
using namespace std;

const float M_PI = 3.14159265358979323846f;   // pi
const int mapHeight = 15;
const int mapWidth = 15;
const int cellHeightWidth = 50;
const int cellHeightWidthHalf = cellHeightWidth/2;
const int screenWidth = mapWidth * cellHeightWidth;
const int screenHeight = mapHeight * cellHeightWidth;

class Level {
	float	enemyHealthModifier = 0.1f;
	float	enemyRateModifier = -100;
	float	enemyNumberModifier = 5;
	float	enemySpeedModifier = 1;
public:
	float	enemyNumber = 5;
	float	enemySpeed = 1;
	float	enemyHealth = 1;
	float	enemyTimer = 0;
	float	enemyRate = 2000;			// seconds between enemies
	int		levelNumber = 1;			// incremental level number
	bool	completed = false;
	float	reward = 250;

	void IncreaseLevel()
	{
		levelNumber++;
		cout << levelNumber << endl;
		enemyHealth += enemyHealthModifier;
		enemyRate += enemyRateModifier;
		enemyNumber += enemyNumberModifier;

		cout << "enemyHealth:" << enemyHealth << endl;
		cout << "enemyRate:" << enemyRate << endl;
		cout << "enemyNumber:" << enemyNumber << endl;
	}
};

class Enemy {
public:
	sf::Texture* enemyTexture = new sf::Texture();
	sf::Sprite* enemySprite = new sf::Sprite();
	float health;
	bool toDelete = false;
	const float mass = 1;
	const float max_force = 10;
	float max_speed = 0.5f;
	float heading; //angle
	float boundingRadius = cellHeightWidth / 2;
	sf::Vector2f center;
	sf::Vector2f velocity = sf::Vector2f(0.1f,0);
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
	Enemy(sf::Vector2f pos, float _health, float _max_speed) {
		baseRect = sf::RectangleShape(sf::Vector2f(cellHeightWidth, cellHeightWidth));
		position = pos;
		baseRect.setOrigin(sf::Vector2f(baseRect.getSize().x / 2, baseRect.getSize().y / 2));
		baseRect.setPosition(pos.x, pos.y);
		baseRect.setFillColor(sf::Color::Blue);
		baseRect.setOutlineColor(sf::Color::Blue);
		baseRect.setOutlineThickness(1);
		health = _health;
		max_speed = _max_speed;

		if (!enemyTexture->loadFromFile("Resources/output-49.png")) {
			cout << "File not found" << endl;
		}
		enemyTexture->setSmooth(true);
		enemySprite->setTexture(*enemyTexture);
		enemySprite->setOrigin(sf::Vector2f(enemyTexture->getSize().x / 2,
			enemyTexture->getSize().y / 2));
		enemySprite->setPosition(position);
		enemySprite->setRotation(rotation);
		enemySprite->setScale(0.5, 0.5);
		
	}

	void draw(sf::RenderWindow& window) {
		//window.draw(baseRect);
		window.draw(*enemySprite);
	}

	void RotateToFaceTarget(sf::Vector2<float> target) {
		float dx = position.x - target.x;
		float dy = position.y - target.y;

		rotation = (atan2(dy, dx)) * 180.0f / M_PI;
		baseRect.setRotation(rotation + rotationOffset);
		enemySprite->setRotation(rotation - 90);

	}

	sf::Vector2f seek(sf::Vector2f from, sf::Vector2f to, float length) {
		return Math::normalize(from - to) * length;
	}
	sf::Vector2f seek(sf::Vector2f to) {
		return Math::normalize(to - position) * max_speed;
	}

	void update() {
		sf::Vector2f desired_velocity;
		sf::Vector2f steering;
		//path progression
		if (currentPath.empty())
			currentPath.push_back(sf::Vector2f(500, 500));
		if (Math::length(currentPath[0] - position) < 0.50)
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
		RotateToFaceTarget(position + velocity);
	}

	void UpdatePosition(sf::Vector2f p){
		position = p;
		baseRect.setPosition(position);
		enemySprite->setPosition(position);
	}
	void Hit(const float dmg) {
		health -= dmg;

		if (health <= 0)
		{
			toDelete = true;
		}

		//cout << "Enemy:" << this << "Heath: " << health << (toDelete ?  "DEAD" : "") << endl;
	}
};

class Bullet {
	sf::Texture* bulletTexture = new sf::Texture();
	sf::Sprite* bulletSprite = new sf::Sprite();
	
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
	float damage = 0.1f;

public:
	bool toDelete = false;
	sf::Vector2f position;

	Bullet() {}
	~Bullet() {}
	Bullet(sf::Vector2f pos, shared_ptr<Enemy> tar, float _distanceToTravel, float dmg) {
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

		if (!bulletTexture->loadFromFile("Resources/output-46.png")) {
			cout << "File not found" << endl;
		}
		bulletTexture->setSmooth(true);
		bulletSprite->setTexture(*bulletTexture);
		bulletSprite->setOrigin(sf::Vector2f(bulletTexture->getSize().x / 2,
			bulletTexture->getSize().y / 2));
		bulletSprite->setPosition(position);
		bulletSprite->setScale(2, 2);
		bulletSprite->setRotation(rotation);
		damage = dmg;
	}

	sf::Vector2f seek(sf::Vector2f to) {
		return Math::normalize(to - position) * max_speed;
	}

	void print() {
		cout << position << endl << velocity << endl;
	}

	void RotateToFaceTarget(sf::Vector2<float> target) {
		float dx = position.x - target.x;
		float dy = position.y - target.y;

		rotation = (atan2(dy, dx)) * 180 / M_PI;
		baseRect.setRotation(rotation + rotationOffset);
		bulletSprite->setRotation(rotation - 90);
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
		RotateToFaceTarget(position + velocity);
	}

	void draw(sf::RenderWindow& window) {
		//window.draw(baseRect);
		window.draw(*bulletSprite);
	}

	void UpdatePosition(sf::Vector2f p) {
		distanceTravelled += Math::length(p - position);
		steps++;
		position = p;
		baseRect.setPosition(position);
		bulletSprite->setPosition(position);
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

white
green
blue
yellow
red

*/
class Turret {
	sf::Texture* turretTexture[5];
	sf::Sprite* turretSprite = new sf::Sprite();

	sf::RectangleShape baseRect;
	sf::RectangleShape topRect;
	float rotation;
	double cooldownTime = 1000;
	double cooldownTimer = 0;
	bool isReady = true;
	const float rotationOffset = 90;
	float rotationSpeed = 20;
	list<Bullet> deadAmmo;
	int level = 1;
public:
	sf::Vector2f position;
	list<shared_ptr<Bullet>> ammo;
	shared_ptr<Enemy> target;
	bool activeTarget;
	float damage;
	Turret() {}
	~Turret() {}
	Turret(sf::Vector2f pos, float dmg) {
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
		
		turretTexture[0] = new sf::Texture();
		if (!turretTexture[0]->loadFromFile("Resources/output-51w.png"))
		{
			cout << "File not found" << endl;
		}
		turretTexture[0]->setSmooth(true);
		turretSprite->setTexture(*turretTexture[0]);
		turretSprite->setOrigin(sf::Vector2f(turretTexture[0]->getSize().x / 2,
											turretTexture[0]->getSize().y / 2));
		turretSprite->setPosition(position.x + cellHeightWidthHalf, position.y + cellHeightWidthHalf);
		turretSprite->setScale(0.5, 0.5);

		damage = dmg;
	}

	void draw(sf::RenderWindow& window) {
		//window.draw(baseRect);
		//window.draw(topRect);
		window.draw(*turretSprite);
		for each (auto b in ammo)
		{
			b->draw(window);
		}

	}

	void rotateToFaceTarget(sf::Vector2<float> target) {
		float dx = position.x - target.x;
		float dy = position.y - target.y;

		rotation = (atan2(dy, dx)) * 180 / M_PI;
		//topRect.setRotation(rotation + rotationOffset);
		turretSprite->setRotation(rotation - 90);
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
				ammo.push_back(make_shared<Bullet>(position, target, Math::length(target->position - position),damage));
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

	void Upgrade()
	{
		switch (level)
		{
		case 1:
			if (!turretTexture[0]->loadFromFile("Resources/output-51w.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		case 2:
			if (!turretTexture[0]->loadFromFile("Resources/output-51g.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		case 3:
			if (!turretTexture[0]->loadFromFile("Resources/output-51b.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		case 4:
			if (!turretTexture[0]->loadFromFile("Resources/output-51y.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		case 5:
			if (!turretTexture[0]->loadFromFile("Resources/output-51r.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		default:
			if (!turretTexture[0]->loadFromFile("Resources/output-51.png"))
			{
				cout << "File not found" << endl;
			}
			break;
		}
		cout << "upgraded dmg from " << damage << " to " << damage * 2 << endl;
		level++;
		damage *= 2;

	}
};

std::vector<Turret>	turrets;
std::list<shared_ptr<Enemy>> enemies;
std::list<shared_ptr<Enemy>> preppedEnemies;

std::vector<shared_ptr<sf::Vector2f>> reconstruct_path(std::map<shared_ptr<sf::Vector2f>, shared_ptr<sf::Vector2f>> cameFrom,
	shared_ptr<sf::Vector2f> current) {
	std::vector<shared_ptr<sf::Vector2f>> total_path;
	total_path.push_back(current);
	while (cameFrom.find(current) != cameFrom.end()) {
		current = cameFrom[current];
		total_path.push_back(current);
	}
	return total_path;
}

double heuristic_cost_estimate(shared_ptr<sf::Vector2f> from, shared_ptr<sf::Vector2f> to) {
	return hypot(from->x - to->x, from->y - to->y);
}
std::vector<sf::Vector2f> obstacles;

std::vector<shared_ptr<sf::Vector2f>> getNeighbours(shared_ptr<sf::Vector2f> pos) {
	// replace when adding obstacles
	std::vector<sf::Vector2f> temp;
	if (pos->x != 0)
		temp.push_back(sf::Vector2f(pos->x - 1, pos->y));
	if (pos->x != mapWidth - 1)
		temp.push_back(sf::Vector2f(pos->x + 1, pos->y));
	if (pos->y != 0)
		temp.push_back(sf::Vector2f(pos->x, pos->y - 1));
	if (pos->y != mapHeight - 1)
		temp.push_back(sf::Vector2f(pos->x, pos->y + 1));

	// topleft
	if (pos->x != 0 && pos->y != 0)
		temp.push_back(sf::Vector2f(pos->x - 1, pos->y - 1));
	// bottomleft
	if (pos->x != 0 && pos->y != mapHeight)
		temp.push_back(sf::Vector2f(pos->x - 1, pos->y + 1));
	// bottomright
	if (pos->x != mapWidth && pos->y != mapHeight)
		temp.push_back(sf::Vector2f(pos->x + 1, pos->y + 1));
	// topright
	if (pos->x != mapWidth && pos->y != 0)
		temp.push_back(sf::Vector2f(pos->x + 1, pos->y - 1));

	std::vector<sf::Vector2f> toRemove;
	//ADD OBSTACLES HERE TODO:Implement true map editor
	/*for (size_t i = 1; i < 100; i++)
	{
		obstacles.push_back(sf::Vector2f(i, 10));
	}

	for (size_t i = 0; i < 25; i++)
	{
		obstacles.push_back(sf::Vector2f(i, 13));
	}
	for (size_t i = 1; i < 100; i++)
	{
		obstacles.push_back(sf::Vector2f(i, 16));
	}*/
	for (auto pos : temp)
	{
		//sf::Vector2f pos(t.position.x, t.position.y);
		for (sf::Vector2f ob : obstacles)
		{
			if (pos == ob)
			{
				toRemove.push_back(pos);
			}
		}
	}
	for (sf::Vector2f tr : toRemove)
	{
		std::vector<sf::Vector2f>::iterator findInTemp =
			std::find(temp.begin(), temp.end(), tr);
		temp.erase(findInTemp);
	}
	vector<shared_ptr<sf::Vector2f>> toReturn;
	for (auto a : temp)
	{
		toReturn.push_back(make_shared<sf::Vector2f>(a));
	}
	return toReturn;
}

int dist_between(shared_ptr<sf::Vector2f> current, shared_ptr<sf::Vector2f> neighbour) {
	return 1; // Replace when adding obstacles
}

int find_closest_to_goal(std::vector<shared_ptr<sf::Vector2f>> set, shared_ptr<sf::Vector2f> goal) {
	int current_lowest_index = 0;
	float current_lowest_score = heuristic_cost_estimate(set[0], goal);
	float score;
	for (int i = 1; i < (int)set.size(); ++i) {
		score = heuristic_cost_estimate(set[i], goal);
		if (score < current_lowest_score) {
			current_lowest_index = i;
			current_lowest_score = score;
		}
	}

	return current_lowest_index;
}

void display_route2D(sf::RenderWindow& window, std::vector<sf::Vector2f> path, sf::Vector2f goal, sf::Vector2i mouse) {

	int mouseCellX = mouse.x / cellHeightWidth;
	int mouseCellY = mouse.y / cellHeightWidth;

	for (int y = mapHeight; y > -1; --y) {
		for (int x = 0; x < mapWidth; ++x) {
			float tlx = x * cellHeightWidth;
			float tly = y * cellHeightWidth;

			//GRID
			/*sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });

			std::vector<sf::Vector2f>::iterator findInPath =
				std::find(path.begin(), path.end(), sf::Vector2f(x, y));
			if (findInPath != path.end()) {
				rect.setFillColor(sf::Color::Red);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (sf::Vector2f(x, y) == goal) {
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
	std::list<shared_ptr<sf::Vector2f>> closedSet,
	std::vector<shared_ptr<sf::Vector2f>> openSet,
	std::map<shared_ptr<sf::Vector2f>, double> gScore,
	std::map<shared_ptr<sf::Vector2f>, double> fScore,
	shared_ptr<sf::Vector2f> current,
	shared_ptr<sf::Vector2f> goal) {

	sf::Font MyFont;
	MyFont.loadFromFile("arial.ttf");
	sf::Text textF("Hello", MyFont, cellHeightWidth / 4);
	sf::Text textG("Hello", MyFont, cellHeightWidth / 4);
	textF.setColor(sf::Color::Black);
	textG.setColor(sf::Color::Black);
	window.clear();
	for (int y = mapHeight; y > -1; --y) {
		for (int x = 0; x < mapWidth; ++x) {
			float tlx = x * cellHeightWidth;
			float tly = y * cellHeightWidth;

			//CELLS
			sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });

			std::vector<shared_ptr<sf::Vector2f>>::iterator findInOpenSet =
				std::find(openSet.begin(), openSet.end(), make_shared<sf::Vector2f>(x,y));
			std::list<shared_ptr<sf::Vector2f>>::iterator findInClosedSet =
				std::find(closedSet.begin(), closedSet.end(), make_shared<sf::Vector2f>(x,y));

			if (findInOpenSet != openSet.end()) {
				rect.setFillColor(sf::Color::Blue);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (sf::Vector2f(x, y) == *goal) 
			{
				rect.setFillColor(sf::Color::Green);
				rect.setPosition(tlx, tly);
				window.draw(rect);
			}
			else if (sf::Vector2f(x, y) == *current)
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

			std::map<shared_ptr<sf::Vector2f>, double>::iterator findInFScore = fScore.find(make_shared<sf::Vector2f>(x, y));
			std::map<shared_ptr<sf::Vector2f>, double>::iterator findInGScore = gScore.find(make_shared<sf::Vector2f>(x, y));
			if (findInFScore != fScore.end()) {
				textF.setString(to_string(findInGScore->second));
				textG.setString(to_string(findInGScore->second));
			}
			textF.setPosition(tlx, tly);
			textG.setPosition(tlx, tly + cellHeightWidth / 4);
			window.draw(textF);
			window.draw(textG);
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

shared_ptr<sf::Vector2f> find_lowest_node_by_fScore(std::vector<shared_ptr<sf::Vector2f>> openSet,
	std::map<shared_ptr<sf::Vector2f>, double> fScore) {
	auto from = make_shared<sf::Vector2f>((float)0, (float)0);
	auto to = make_shared<sf::Vector2f>((float)mapWidth, (float)mapHeight);
	double lowest_score = heuristic_cost_estimate(from, to);
	shared_ptr<sf::Vector2f> lowest_node;

	for (auto fff : fScore) {
		for (auto v : openSet) {
			if (v == fff.first) {
				if (fff.second <= lowest_score) {
					lowest_score = fff.second;
					lowest_node = fff.first;
				}
			}
		}
	}
	return lowest_node;
}


std::vector<shared_ptr<sf::Vector2f>> A_Star(sf::RenderWindow& window, shared_ptr<sf::Vector2f> start, shared_ptr<sf::Vector2f> goal) {
	// The set of nodes already evaluated
	std::list<shared_ptr<sf::Vector2f>> closedSet;

	// The set of currently discovered nodes that are not evaluated yet.
	// Initially, only the start node is known.
	std::vector<shared_ptr<sf::Vector2f>> openSet;
	openSet.push_back(start);

	// For each node, which node it can most efficiently be reached from.
	// If a node can be reached from many nodes, cameFrom will eventually contain
	// the most efficient previous step.
	std::map<shared_ptr<sf::Vector2f>, shared_ptr<sf::Vector2f>> cameFrom; // := an empty map

	// For each node, the cost of getting from the start node to that node.
	std::map<shared_ptr<sf::Vector2f>, double> gScore; // := map with default value of Infinity

	// The cost of going from start to start is zero.
	gScore[start] = 0;

	// For each node, the total cost of getting from the start node to the goal
	// by passing by that node. That value is partly known, partly heuristic.
	std::map<shared_ptr<sf::Vector2f>, double> fScore;
	// fScore := map with default value of Infinity

	// For the first node, that value is completely heuristic.
	fScore[start] = heuristic_cost_estimate(start, goal);
	int passes = 0;

	while (!openSet.empty()) {
		// std::system("clear");
		// display_map(openSet, closedSet, goal);
		//cout << "Passes: " << passes++ << endl;

		shared_ptr<sf::Vector2f> current = find_lowest_node_by_fScore(openSet, fScore);
		if (current == goal) {
			return reconstruct_path(cameFrom, current);
		}
		//cout << "Current: " << *current << endl;
		//cout << "OpenSet: " << endl;
		//for (auto i : openSet)
			//cout << "\t" << *i << endl;
		std::vector<shared_ptr<sf::Vector2f>>::iterator findInOpenSet =
			std::find(openSet.begin(), openSet.end(), current);

		openSet.erase(findInOpenSet);
		closedSet.push_back(current);
		//cout << "ClosedSet: " << endl;
		//for (auto i : closedSet)
			//cout << "\t" << i << endl;

		std::vector<shared_ptr<sf::Vector2f>> neighbours = getNeighbours(current);
		for (auto neighbour : neighbours) {

			std::list<shared_ptr<sf::Vector2f>>::iterator findInClosedSet =
				std::find(closedSet.begin(), closedSet.end(), neighbour);

			if (findInClosedSet != closedSet.end()) {
				continue; // Ignore the neighbour which is already evaluated.
			}

			// The distance from start to a neighbour
			double tentative_gScore;
			std::map<shared_ptr<sf::Vector2f>, double>::iterator findInGScore = gScore.find(current);
			if (findInGScore != gScore.end()) 
			{
				tentative_gScore = heuristic_cost_estimate(start, neighbour);
			}
			else
			{
				tentative_gScore = gScore[current] + heuristic_cost_estimate(current, neighbour);
			}

			std::vector<shared_ptr<sf::Vector2f>>::iterator findInOpenSet =
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
			pair<std::map<shared_ptr<sf::Vector2f>, double>::iterator, bool> insert = fScore.insert(
				std::make_pair(neighbour, gScore[neighbour] + heuristic_cost_estimate(
					neighbour, goal)));
		}
		display_route_in_progress(window, closedSet, openSet, gScore, fScore, current, goal);
	}
	return std::vector<shared_ptr<sf::Vector2f>>();
}

int main(int argc, char **argv)
{
	obstacles.push_back(sf::Vector2f(2, 0));
	obstacles.push_back(sf::Vector2f(2, 1));
	obstacles.push_back(sf::Vector2f(2, 2));
	obstacles.push_back(sf::Vector2f(2, 3));
	obstacles.push_back(sf::Vector2f(2, 4));
	obstacles.push_back(sf::Vector2f(2, 5));
	obstacles.push_back(sf::Vector2f(2, 6));
	obstacles.push_back(sf::Vector2f(2, 7));
	//obstacles.push_back(sf::Vector2f(2, 8));

	int map[screenHeight/cellHeightWidth][screenWidth/cellHeightWidth];

	shared_ptr<sf::Vector2f> start = make_shared<sf::Vector2f>(0, 0);
	shared_ptr<sf::Vector2f> end = make_shared<sf::Vector2f>(mapHeight-1, mapWidth-1);

	if (argc == 5) {
		start->x  = atoi(argv[1]);
		start->y = atoi(argv[2]);
		end->x    = atoi(argv[3]);
		end->y   = atoi(argv[4]);
	}

	// create the window
	sf::RenderWindow window(sf::VideoMode(mapWidth* cellHeightWidth, mapHeight* cellHeightWidth), "A* Pathfinding");

	// create screen buffer margin
	float buffer_width = 50;
	sf::RectangleShape screen_boundary(sf::Vector2f(mapWidth* cellHeightWidth - buffer_width,
		mapHeight* cellHeightWidth - buffer_width));
	screen_boundary.setPosition(sf::Vector2f(buffer_width, buffer_width));

	Level levelTracker;
	float money = 1000;
	//Debug Turrets
	//for (int y = 0; y < mapHeight; y+=10)
	//{
	//	for (int x = 0; x < mapWidth; x+=10)
	//	{
	//		turrets.push_back(Turret(sf::Vector2f(x * cellHeightWidth + cellHeightWidthHalf, y * cellHeightWidth + cellHeightWidthHalf)));
	//	}
	//}
	//turrets.push_back(Turret(sf::Vector2f(50 * cellHeightWidth, 50 * cellHeightWidth), 0.1));


	for (size_t i = 0; i < levelTracker.enemyNumber; i++)
	{
		preppedEnemies.push_back(make_shared<Enemy>(sf::Vector2f(start->x + 20, start->y + 20), levelTracker.enemyHealth, levelTracker.enemySpeed));
	}

	for (int e = 0; e < 1; e++)
	{
		enemies.push_back(preppedEnemies.back());
		preppedEnemies.pop_back();
	}

	// create screen background
	sf::Texture bgtexture;
	if (!bgtexture.loadFromFile("Resources/output-2.png"))
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
	std::vector<shared_ptr<sf::Vector2f>> completed_path = A_Star(window, start, end);

	//Give Path to Enemies
	auto ite = enemies.begin();
	auto itp = preppedEnemies.begin();
	for (int e = 0; e < 1; e++)
	{
		for (shared_ptr<sf::Vector2f> v : completed_path)
		{
			ite->get()->currentPath.insert(ite->get()->currentPath.begin(), sf::Vector2f(v->x * cellHeightWidth, v->y * cellHeightWidth));
			itp->get()->currentPath.insert(itp->get()->currentPath.begin(), sf::Vector2f(v->x * cellHeightWidth, v->y * cellHeightWidth));
		}
		ite++;
		itp++;
	}

    // create the particle system
    ParticleSystem particles(1000);

    // create a clock to track the elapsed time
    sf::Clock clock;

    // run the main loop
    while (window.isOpen())
    {
		window.clear();

		// update level
		levelTracker.enemyTimer += clock.getElapsedTime().asMilliseconds();
		if (levelTracker.enemyTimer > levelTracker.enemyRate) {
			levelTracker.enemyTimer = 0;
			if (!preppedEnemies.empty())
			{
				enemies.push_back(preppedEnemies.back());
				preppedEnemies.pop_back();
			}
			else
			{
				if (enemies.empty()) {
					levelTracker.completed = true;
				}
			}
		}
		if (levelTracker.completed) {
			levelTracker.completed = false;
			levelTracker.IncreaseLevel();

			// ResetEnemies
			for (size_t i = 0; i < levelTracker.enemyNumber; i++)
			{
				preppedEnemies.push_back(make_shared<Enemy>(sf::Vector2f(start->x, start->y), levelTracker.enemyHealth, levelTracker.enemySpeed));
			}
		}

        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

			// New Path
			if (event.mouseButton.button == sf::Mouse::Button::Left && event.type == event.MouseButtonPressed)
			{
				// new turret
				int mouseCellX = sf::Mouse::getPosition(window).x / cellHeightWidth;
				int mouseCellY = sf::Mouse::getPosition(window).y / cellHeightWidth;
				sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });
				rect.setFillColor(sf::Color::White);
				rect.setPosition(mouseCellX * cellHeightWidth, mouseCellY * cellHeightWidth);
				window.draw(rect);
				Turret temp(sf::Vector2f(mouseCellX * cellHeightWidth, mouseCellY * cellHeightWidth), 0.5);
				bool occupied = false;
				for (size_t i = 0; i < turrets.size(); i++)
				{
					if (turrets[i].position == sf::Vector2f(mouseCellX * cellHeightWidth, mouseCellY * cellHeightWidth))
					{	
						if (money >= 250)
						{
							money -= 250;
							turrets[i].Upgrade();
						}
						occupied = true;
					}
				}
				if (!occupied)
				{
					if (money >= 250)
					{
						money -= 250;

						turrets.push_back(temp);
						map[mouseCellX][mouseCellY] = 1;
						obstacles.push_back(sf::Vector2f(mouseCellX, mouseCellY));
						// New obstacle = new path needed
						completed_path = A_Star(window, start, end);
						ite = enemies.begin();
						itp = preppedEnemies.begin();
						while (ite != enemies.end())
						{
							ite->get()->currentPath.clear();

							for (auto v : completed_path)
							{
								ite->get()->currentPath.insert(ite->get()->currentPath.begin(), sf::Vector2f(v->x * cellHeightWidth, v->y * cellHeightWidth));
							}

							//need to find current nearest step in the path and remove eveything before it
							float minDist = Math::length(sf::Vector2f(0, 0) - sf::Vector2f(screenHeight, screenWidth));
							sf::Vector2f closestvec;
							for (sf::Vector2f var : ite->get()->currentPath)
							{
								//cout << var << "->" << ite->get()->position / (float)cellHeightWidth << "\t" << Math::length(var - ite->get()->position / (float)cellHeightWidth) << endl;
								if (Math::length(var - ite->get()->position) < minDist)
								{
									closestvec = var;
									minDist = Math::length(var - ite->get()->position);
								}
							}
							sf::Vector2f closest = closestvec;
							auto currentPathClosestStep = find(ite->get()->currentPath.begin(), ite->get()->currentPath.end(), closest);
							ite->get()->currentPath.erase(ite->get()->currentPath.begin(), currentPathClosestStep);
							ite++;
						}
						while (itp != preppedEnemies.end())
						{
							itp->get()->currentPath.clear();
							for (auto v : completed_path)
							{
								itp->get()->currentPath.insert(itp->get()->currentPath.begin(), sf::Vector2f(v->x * cellHeightWidth, v->y * cellHeightWidth));
							}
							itp++;
						}
					}
				}
				else
				{
					//cout << "Spot already taken" << endl;
				}

			}
        }
		int mouseCellX = sf::Mouse::getPosition(window).x / cellHeightWidth;
		int mouseCellY = sf::Mouse::getPosition(window).y / cellHeightWidth;
		sf::RectangleShape rect({ (float)cellHeightWidth, (float)cellHeightWidth });
		rect.setFillColor(sf::Color::White);
		rect.setPosition(mouseCellX * cellHeightWidth, mouseCellY * cellHeightWidth);
		window.draw(rect);

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

		int prevSize = enemies.size();
		enemies.remove_if([](shared_ptr<Enemy> elem) { return elem->toDelete; });
		money += (prevSize - enemies.size()) * levelTracker.reward;
		ite = enemies.begin();
		while (ite != enemies.end())
		{
			//cout << ite->get() << "\t" << ite->get()->toDelete << endl;
			ite->get()->update();
			//e.UpdatePosition(Math::clamp(e.position, screen_boundary.getSize(), screen_boundary.getPosition()));
			ite->get()->draw(window);
			ite++;
		}

		//display UI
		sf::Font MyFont;
		MyFont.loadFromFile("arial.ttf");
		
		sf::Text text("£"+to_string(money), MyFont, cellHeightWidth);
		text.setColor(sf::Color::White);
		text.setPosition(sf::Vector2f(10, screenWidth / 2));
		window.draw(text);


        window.display();
    }

    return 0;
}