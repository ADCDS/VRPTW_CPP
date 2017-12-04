#pragma once
#include <list>

class Vehicle;
class Point;
class Solution
{
public:
	std::list<Vehicle*> vehicles;
	double weight;

	double total_weight();

	/**
	 * improved is a boolean value, at the end of this function, it teels if the mutation was a success or a failure
	 */
	void mutate1(bool& improved);

	bool has_node(Point* p);

	~Solution();


};
