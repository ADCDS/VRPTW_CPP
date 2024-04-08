#include "Point.h"
#include <cmath>


double Point::distance(Point* p)
{
	return sqrt(pow(p->x - this->x, 2) + pow(p->y - this->y, 2));
}

Point::Point()
{
}


Point::~Point()
{
}
