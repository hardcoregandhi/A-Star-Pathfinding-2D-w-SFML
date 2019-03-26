
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

#include "Enemy.h"
using namespace std;

Enemy::Enemy(sf::Vector2f pos, float _health, float _max_speed) {
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

void Enemy::draw(sf::RenderWindow& window) {
	//window.draw(baseRect);
	window.draw(*enemySprite);
}

void Enemy::RotateToFaceTarget(sf::Vector2<float> target) {
	float dx = position.x - target.x;
	float dy = position.y - target.y;

	rotation = (atan2(dy, dx)) * 180.0f / M_PI;
	baseRect.setRotation(rotation + rotationOffset);
	enemySprite->setRotation(rotation - 90);

}

sf::Vector2f Enemy::seek(sf::Vector2f from, sf::Vector2f to, float length) {
	return Math::normalize(from - to) * length;
}
sf::Vector2f Enemy::seek(sf::Vector2f to) {
	return Math::normalize(to - position) * max_speed;
}

void Enemy::update() {
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

void Enemy::UpdatePosition(sf::Vector2f p) {
	position = p;
	baseRect.setPosition(position);
	enemySprite->setPosition(position);
}

void Enemy::Hit(const float dmg) {
	health -= dmg;

	if (health <= 0)
	{
		toDelete = true;
	}

	//cout << "Enemy:" << this << "Heath: " << health << (toDelete ?  "DEAD" : "") << endl;
}
