#pragma once
#include <iostream>
using namespace std;

const float M_PI = 3.14159265358979323846f;   // pi
const int mapHeight = 15;
const int mapWidth = 15;
const int cellHeightWidth = 50;
const int cellHeightWidthHalf = cellHeightWidth / 2;
const int screenWidth = mapWidth * cellHeightWidth;
const int screenHeight = mapHeight * cellHeightWidth;

struct Level {
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
