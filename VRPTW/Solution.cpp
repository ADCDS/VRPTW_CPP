#include "Solution.h"
#include "Vehicle.h"
#include "Utils.h"
#include "SubCandidate.h"
#include "Point.h"
#include "Utils.h"
#include <iostream>

double Solution::total_weight()
{
	double ret = 0;
	for (auto vehicle : vehicles)
	{
		ret += vehicle->get_weight();
	}
	weight = ret;
	return ret;
}


void Solution::mutate1(bool &improved)
{
	improved = false;
	Point* p = nullptr;
	int ec = 0;
	double new_weight, original_weight = this->total_weight();

	int index = (rand() % (Utils::raw_rows.size() - 2)) + 1;

	auto it = Utils::raw_rows.begin();

	while (index-- > 0)
		++it;

	p = *it;
	auto psz = p->state.size();
	auto original_vehicle = p->state[this].first;
	int original_pos = p->state[this].second;
	original_vehicle->remove_node(original_pos);

	SubCandidate candidate(original_weight, original_pos, original_vehicle);

	for (auto vehicle : this->vehicles)
	{
		for (int i = 0; i <= vehicle->nodes.size(); ++i)
		{
			ec = 0;
			vehicle->add_node(p, i, ec);
			if (!ec) {
				new_weight = this->total_weight();
				if (new_weight < candidate.peso)
				{
					candidate.peso = new_weight;
					candidate.v = vehicle;
					candidate.pos = i;
					//std::cout << "Best position for node " << p->id << ", vehicle " << candidate.v->id << " position " << candidate.pos << ", total weight " << candidate.peso << "\n";
					improved = true;
				}
				vehicle->remove_node(i);
			}
		}
	}

	ec = 0;
	candidate.v->add_node(p, candidate.pos, ec);
	this->total_weight();
}

bool Solution::has_node(Point* p)
{
	for (auto vehicle : vehicles)
	{
		if (vehicle->is_point_on_vehicle(p))
			return true;
	}
	return false;
}


Solution::~Solution()
{
	for (auto vehicle : vehicles)
	{
		delete vehicle;		
	}
	for (auto p : Utils::raw_rows)
	{
		p->state.erase(this);
	}
}
