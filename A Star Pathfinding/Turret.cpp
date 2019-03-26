#include "Turret.h"
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
#include "Constants.h"
using namespace std;


Turret::Turret(sf::Vector2f pos, float dmg) {
	baseRect = sf::RectangleShape(sf::Vector2f(cellHeightWidthHalf, cellHeightWidthHalf));
	topRect = sf::RectangleShape(sf::Vector2f(cellHeightWidthHalf, cellHeightWidthHalf * 3));
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

void Turret::draw(sf::RenderWindow& window) {
	//window.draw(baseRect);
	//window.draw(topRect);
	window.draw(*turretSprite);
	for each (auto b in ammo)
	{
		b->draw(window);
	}
}

void Turret::rotateToFaceTarget(sf::Vector2<float> target) {
	float dx = position.x - target.x;
	float dy = position.y - target.y;

	rotation = (atan2(dy, dx)) * 180 / M_PI;
	//topRect.setRotation(rotation + rotationOffset);
	turretSprite->setRotation(rotation - 90);
}

void Turret::update(double elapsedTime) {
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
			ammo.push_back(make_shared<Bullet>(position, target, Math::length(target->position - position), damage));
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

void Turret::Upgrade()
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