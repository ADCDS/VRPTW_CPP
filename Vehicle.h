#pragma once
#include <list>
#include <memory>
class Point;
class VehicleState;
class Solution;

class Vehicle
{
public:
	int id;
	std::list<std::shared_ptr<VehicleState>> nodes;
	std::shared_ptr<VehicleState> last_node;
	Solution* solution;

	void reevalueate_route(int start, int end);
	bool is_point_on_vehicle(Point* p);
	void validate_states(std::shared_ptr<VehicleState> possible_state, int position, int &ec);
	void add_node(Point* p, int pos, int &ec);
	void remove_node(int pos);
	double get_weight() const;
	Vehicle* clone(Solution* s);
	
	
	Vehicle(Solution * s);
};

