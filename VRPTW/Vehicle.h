#pragma once
#include <list>
class Point;
class VehicleState;
class Solution;

class Vehicle
{
public:
	int id;
	std::list<VehicleState*> nodes;
	VehicleState* last_node;
	Solution* solution;

	void reevalueate_route(int start, int end);
	bool is_point_on_vehicle(Point* p);
	void validate_states(VehicleState* possible_state, int position, int &ec);	
	void add_node(Point* p, int pos, int &ec);
	void remove_node(int pos);
	double get_weight() const;
	Vehicle* clone(Solution* s);
	
	
	Vehicle(Solution * s);
	~Vehicle();
};

