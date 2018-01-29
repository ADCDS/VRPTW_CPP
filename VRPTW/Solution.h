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
	void mutate();
	void validate();

	void mutate_insertion();
	void mutate_swap();
	void mutate_inversion();


	void print();

	bool has_node(Point* p);
	Solution* clone();

	~Solution();

};
