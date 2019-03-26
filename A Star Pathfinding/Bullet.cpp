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
#include "Bullet.h"
#include "Turret.h"

#include "Constants.h"
using namespace std;


Bullet::Bullet(sf::Vector2f pos, shared_ptr<Enemy> tar, float _distanceToTravel, float dmg) {
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

sf::Vector2f Bullet::seek(sf::Vector2f to) {
	return Math::normalize(to - position) * max_speed;
}

void Bullet::print() {
	cout << position << endl << velocity << endl;
}

void Bullet::RotateToFaceTarget(sf::Vector2<float> target) {
	float dx = position.x - target.x;
	float dy = position.y - target.y;

	rotation = (atan2(dy, dx)) * 180 / M_PI;
	baseRect.setRotation(rotation + rotationOffset);
	bulletSprite->setRotation(rotation - 90);
}

void Bullet::update() {
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

void Bullet::draw(sf::RenderWindow& window) {
	//window.draw(baseRect);
	window.draw(*bulletSprite);
}

void Bullet::UpdatePosition(sf::Vector2f p) {
	distanceTravelled += Math::length(p - position);
	steps++;
	position = p;
	baseRect.setPosition(position);
	bulletSprite->setPosition(position);
}
