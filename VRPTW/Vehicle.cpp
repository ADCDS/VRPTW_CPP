#include "Vehicle.h"
#include "Utils.h"
#include <iostream>
#include "Utils.h"
#include "VehicleState.h"
#include "Point.h"

/*void Vehicle::valdebug()
{
	int i = 0;
	for (auto node : nodes)
	{
		if(node->p->state[solution].second != i++)
		{
			std::cout << "erro";
		}
	}
}*/
void Vehicle::reevalueate_route(int start, int end)
{
	std::list<VehicleState*>::iterator li;
	VehicleState* last;	
	//start = 0;
	//end = nodes.size();
	
	if (start == 0)
	{
		li = nodes.begin();
		last = Utils::origin;		
	}
	else
	{
		li = nodes.begin();		
		int tmp_start = start;
		while (tmp_start-- > 0)
			++li;
		last = *std::prev(li);
		
	}
	VehicleState* curr;
	while (++start <= end && li != nodes.end())
	{
		curr = *li;
		li++;
		curr->current_cargo = last->current_cargo - curr->p->demand;
		curr->current_distance = last->current_distance + Utils::distances[last->p->id][curr->p->id];
		curr->current_time = last->current_time + Utils::distances[last->p->id][curr->p->id];
		curr->p->state[solution].second = start - 1;

		if (curr->current_time < curr->p->ready_time)
		{
			curr->current_time = curr->p->ready_time;
		}
		curr->current_time += curr->p->service_time;

		last = curr;
	}	
}

bool Vehicle::is_point_on_vehicle(Point* p)
{
	for (auto value_ : nodes)
	{
		if (value_->p->id == p->id)
		{
			return true;
		}
	}
	return false;
}

void Vehicle::validate_states(VehicleState* possible_state, int position, int &ec)
{
	int c_position = position;
	
	if (!nodes.empty())
	{
		std::list<VehicleState*>::iterator li;
		VehicleState* last_state;
		VehicleState* current_state;
		int last_state_position;
		if (position == 0)
		{
			li = nodes.begin();
			last_state = Utils::origin;
			last_state_position = -1;
		}else
		{
			li = nodes.begin();
			int tmp_position = position;
			while (tmp_position-- > 0)
				++li;

			last_state = *std::prev(li);
			last_state_position = last_state->p->state[solution].second;
		}
		current_state = possible_state;
		int iter_num = position;
		double dist;
		do
		{
			dist = Utils::distances[last_state->p->id][current_state->p->id];
			current_state->current_distance = last_state->current_distance + dist;
			current_state->current_time = last_state->current_time + dist;

			if(current_state->current_time > current_state->p->due_date)
			{
				if(iter_num > 0)
				{
					reevalueate_route(position, iter_num);
				}
				ec = 1;
				return;
			}

			current_state->current_cargo = last_state->current_cargo - current_state->p->demand;

			if(current_state->current_time < current_state->p->ready_time)
			{
				current_state->current_time = current_state->p->ready_time;
			}
			current_state->current_time += current_state->p->service_time;
			current_state->p->state[solution].second = last_state_position + 1;
			last_state_position = current_state->p->state[solution].second;
			last_state = current_state;			
			iter_num++;
		} while (li != nodes.end() && (current_state = *li++)->True);
		last_node = current_state;
	}
	else
	{
		possible_state->current_distance = Utils::distances[Utils::origin->p->id][possible_state->p->id];
		possible_state->current_time = possible_state->current_distance;
		if (possible_state->current_time > possible_state->p->due_date)
		{
			ec = 2;
			return;
		}
		if (possible_state->current_time < possible_state->p->ready_time)
		{
			possible_state->current_time = possible_state->p->ready_time;
		}
		possible_state->current_cargo -= possible_state->p->demand;
		possible_state->current_time += possible_state->p->service_time;
		possible_state->p->state[solution].second = 0;
		last_node = possible_state;
	}

	if (last_node->current_time + Utils::distances[last_node->p->id][Utils::origin->p->id] > Utils::origin->p->due_date)
	{
		ec = 3;
		return;
	}
	
}

void Vehicle::add_node(Point* p, int pos, int &ec)
{
	is_point_on_vehicle(p);

	auto possible_state = new VehicleState(p);
	validate_states(possible_state, pos, ec);
	if(ec)
	{
		delete possible_state;
		return;
	}
	auto it = nodes.begin();
	auto tmp_pos = pos;
	while(tmp_pos-- > 0)
		++it;

	nodes.insert(it, possible_state);
	p->state[solution].first = this;	
}

void Vehicle::remove_node(int pos) throw(std::exception)
{
	std::list<VehicleState*>::iterator li = nodes.begin();
	
	VehicleState* last;
	
	int c_pos = pos;
	while (c_pos-- > 0)
		++li;

	delete *li;
	nodes.erase(li++);

	if (pos == 0)
	{		
		last = Utils::origin;
	}
	else
	{				
		last = *std::prev(li);
	}

	VehicleState* curr;
	while (li != nodes.end())
	{
		curr = *li;
		curr->current_cargo = last->current_cargo - curr->p->demand;
		curr->current_distance = last->current_distance + Utils::distances[last->p->id][curr->p->id];
		curr->current_time = last->current_time + Utils::distances[last->p->id][curr->p->id];

		if (curr->current_time < curr->p->ready_time)
		{
			curr->current_time = curr->p->ready_time;
		}
		curr->current_time += curr->p->service_time;

		curr->p->state[solution].second = curr->p->state[solution].second - 1;
		last = curr;
		++li;
	}
	last_node = last;	
}

double Vehicle::get_weight()
const
{
	return last_node->current_distance + Utils::distances[last_node->p->id][Utils::origin->p->id];
}

Vehicle::Vehicle(Solution *s)
{
	solution = s;
}

Vehicle::~Vehicle()
{
	for (auto node : nodes)
	{
		delete node;
	}	
}
