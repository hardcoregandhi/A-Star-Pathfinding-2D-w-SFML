#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <math.h>

#include "Constants.h"
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
	sf::Vector2f velocity = sf::Vector2f(0.1f, 0);
	sf::Vector2f forward = sf::Vector2f(1, 0);
	sf::Vector2f side;
	sf::Vector2f up = sf::Vector2f(0, 1);
	float speed = 5;
	sf::RectangleShape baseRect;
	sf::Vector2f position;
	std::vector<sf::Vector2f> currentPath;
	float rotation;
	const float rotationOffset = 90;
	Enemy() {};
	~Enemy() {};
	Enemy(sf::Vector2f pos, float _health, float _max_speed);
	void draw(sf::RenderWindow & window);
	void RotateToFaceTarget(sf::Vector2<float> target);
	sf::Vector2f seek(sf::Vector2f from, sf::Vector2f to, float length);
	sf::Vector2f seek(sf::Vector2f to);
	void update();
	void UpdatePosition(sf::Vector2f p);
	void Hit(const float dmg);

};
