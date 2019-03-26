#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <math.h>
#include "Enemy.h"
using namespace std;

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
	
	Bullet(sf::Vector2f pos, shared_ptr<Enemy> tar, float _distanceToTravel, float dmg);

	sf::Vector2f seek(sf::Vector2f to);

	void print();

	void RotateToFaceTarget(sf::Vector2<float> target);

	void update();

	void draw(sf::RenderWindow & window);

	void UpdatePosition(sf::Vector2f p);

};

