#pragma once
#include <SFML/Graphics.hpp>
#include "VectorMath.cpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <math.h>
#include "Bullet.h"
using namespace std;

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
	Turret() {};
	~Turret() {};
	
	Turret(sf::Vector2f pos, float dmg);

	void draw(sf::RenderWindow & window);

	void rotateToFaceTarget(sf::Vector2<float> target);

	void update(double elapsedTime);

	void Upgrade();

};

