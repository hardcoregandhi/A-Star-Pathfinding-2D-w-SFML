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
#include "Constants.h"
#include "Turret.h"
#include "Pathfinding.h"
#pragma warning(disable: 4244)
using namespace std;

std::vector<Turret>	turrets;
std::list<shared_ptr<Enemy>> enemies;
std::list<shared_ptr<Enemy>> preppedEnemies;


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