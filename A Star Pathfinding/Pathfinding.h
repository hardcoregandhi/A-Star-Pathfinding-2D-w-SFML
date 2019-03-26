#pragma once
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
				std::find(openSet.begin(), openSet.end(), make_shared<sf::Vector2f>(x, y));
			std::list<shared_ptr<sf::Vector2f>>::iterator findInClosedSet =
				std::find(closedSet.begin(), closedSet.end(), make_shared<sf::Vector2f>(x, y));

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

auto VectorSharedPtrFinder(std::vector<shared_ptr<sf::Vector2f>> _vector, shared_ptr<sf::Vector2f> target)
{
	for each (auto var in _vector)
	{
		if (var->x == target->x && var->y == target->y)
			return true;
	}
	return false;
}
auto ListSharedPtrFinder(std::list<shared_ptr<sf::Vector2f>> _list, shared_ptr<sf::Vector2f> target)
{
	for each (auto var in _list)
	{
		if (var->x == target->x && var->y == target->y)
			return true;
	}
	return false;
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

			if (ListSharedPtrFinder(closedSet, neighbour)) {
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

			if (VectorSharedPtrFinder(openSet, neighbour)) {
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