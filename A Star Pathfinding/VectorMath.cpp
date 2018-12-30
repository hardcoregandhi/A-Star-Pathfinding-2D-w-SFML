#ifndef VECTOR_MATH_CPP
#define VECTOR_MATH_CPP



#include <SFML/Graphics.hpp>
#include <math.h>

namespace Math{
	static float length(sf::Vector2f v) { // Magnitude
		return sqrt(v.x*v.x + v.y*v.y);
	}
	static sf::Vector2f normalize(sf::Vector2f v) {
		float len = length(v);
		v.x = v.x / len;
		v.y = v.y / len;
		return v;
	}
	static sf::Vector2f clamp(sf::Vector2f v, float m)
	{
		if (length(v) > m)
			normalize(v)*m;

		return v;
	}
	template <class T>
	static sf::Vector2f truncate(sf::Vector2f v, T f) {
		if (length(v) > f)
			return normalize(v) * f;
		else
			return v;
	}
	static sf::Vector2f clamp(sf::Vector2f v1, sf::Vector2f max, bool andZero)
	{
		if (v1.x > max.x)
			v1.x = max.x;
		if (v1.y > max.y)
			v1.y = max.y;
		if (v1.x < 0 && andZero)
			v1.x = 0;
		if (v1.y < 0 && andZero)
			v1.y = 0;

		return v1;
	}
	static sf::Vector2f clamp(sf::Vector2f v1, sf::Vector2f max, sf::Vector2f min)
	{
		if (v1.x > max.x)
			v1.x = max.x;
		if (v1.y > max.y)
			v1.y = max.y;
		if (v1.x < min.x)
			v1.x = min.x;
		if (v1.y < min.y)
			v1.y = min.y;

		return v1;
	}
	template <class T>
	static T clamp(T v, T f) {
		if (length(v) > f)
			return normalize(v) * f;
		else
			return v;
	}
	template <class T>
	bool contains(sf::RectangleShape r, sf::Vector2<T> v)
	{
		if (v.x > r.getPosition().x &&
			v.y > r.getPosition().y &&
			v.x < r.getSize().x &&
			v.y < r.getSize().y)
			return true;
	}
}

#endif // !VECTOR_MATH_CPP