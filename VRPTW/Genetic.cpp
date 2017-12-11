#include "Genetic.h"
#include "Utils.h"
#include "Solution.h"
#include <iostream>
#include "Hillclimbing.h"
#include "Vehicle.h"
#include "VehicleState.h"
#include "Point.h"
#include "math.h"
#include <algorithm>
#include <unordered_map>


std::list<std::pair<Solution*, double/*avaliation*/>> Genetic::solutions;
double Genetic::ticket_max = 0;
double Genetic::max_weight = 0;
int Genetic::population_limit = 0;

struct solutions_comparator
{
	bool operator()(const std::pair<Solution*, double>& inst1, const std::pair<Solution*, double>& inst2)
	{
		return inst1.first->weight < inst2.first->weight;
	}
};

void Genetic::generate_population()
{
	std::cout << "Generating population\n";
	//The first solution is the best that hillclimbing can get	
	auto solution = Hillclimbing::run();
	//auto solution = Utils::random_solution();

	solutions.push_back(std::pair<Solution*, double>(solution, 0));
	for (int i = 1; i < population_limit; ++i)
	{
		solution = Utils::random_solution();

		if (solution->weight > max_weight)
			max_weight = solution->weight;

		solutions.push_back(std::pair<Solution*, double>(solution, 0));
	}
	//ticket_max = pow(ticket_max, 2); // to much
	max_weight *= 2;
	std::cout << "Initial solutions calculated\n";
}

bool Genetic::avaliate_population()
{
	double last_weight = 0;
	for (auto it = solutions.begin(); it != solutions.end(); ++it)
	{
		(*it).second = (max_weight - (*it).first->weight) + last_weight;
		last_weight = (*it).second;
	}
	ticket_max = last_weight;
	if (static_cast<int>(ticket_max) == 0)
	{
		return false;
	}
	return true;
}

std::list<std::pair<Solution*, Solution*>> Genetic::select_parents()
{
	std::list<std::pair<Solution*, Solution*>> res;
	Solution* p1;
	Solution* p2;

	for (int j = 0; j < static_cast<int>(population_limit / 2); ++j)
	{
		int i = rand() % static_cast<int>(ticket_max);
		for (auto solution : solutions)
		{
			if (i <= solution.second)
			{
				p1 = solution.first;
				break;
			}
		}
		i = rand() % static_cast<int>(ticket_max);
		for (auto solution : solutions)
		{
			if (i <= solution.second)
			{
				p2 = solution.first;
				break;
			}
		}
		res.push_back(std::pair<Solution*, Solution*>(p1, p2));
	}

	return res;
}

/*
Solution* Genetic::old_generate_child(Solution* p1, Solution* p2)
{
	auto solution = new Solution;
	//edge recombination crossover

	//TODO: improve neighboors
	std::unordered_map<Point*, std::list<Point*>> neighboors;

	int total_nodes = 0;
	for (auto vehicle : p1->vehicles)
	{				
		for(auto nid = vehicle->nodes.begin(); nid != vehicle->nodes.end(); ++nid)
		{		
			auto next = std::next(nid);
			auto n = &neighboors[(*nid)->p];
			if(next != vehicle->nodes.end())
			{
				n->push_back((*next)->p);
			}			
		}
	}
	for (auto vehicle : p2->vehicles)
	{		
		for (auto nid = vehicle->nodes.begin(); nid != vehicle->nodes.end(); ++nid)
		{
			auto next = std::next(nid);
			auto n = &neighboors[(*nid)->p];
			if (next != vehicle->nodes.end())
			{
				n->push_back((*next)->p);
			}
		}
	}
	auto it = neighboors.begin();
	std::advance(it, ((double)rand() / (RAND_MAX)));
	Point* node = (*it).first;
	neighboors.erase(it++);
	Vehicle * v = new Vehicle(solution);	
	solution->vehicles.push_back(v);

	total_nodes = neighboors.size();
	while(!neighboors.empty())
	{
		auto ec = 0;
		v->add_node(node, v->nodes.size(), ec);
		if(ec)
		{
			ec = 0;
			v = new Vehicle(solution);
			v->add_node(node, 0, ec);
			solution->vehicles.push_back(v);
		}

		//remove node from neighboors list
		for (auto it = neighboors.begin(); it != neighboors.end(); ++it)
		{			
			(*it).second.remove(node);
		}

		//if x neighbor list is empty
		if(neighboors[node].empty())
		{
			neighboors.erase(node);
			//Find a random node
			if (neighboors.size() > 0) {
				auto offset = rand() % neighboors.size();
				auto it = neighboors.begin();

				std::advance(it, offset);

				node = (*it).first;

			}else
			{
				return solution;
			}
		}else
		{
			auto node_neighboors = neighboors[node];
			auto n_it = node_neighboors.begin()++;
			auto neighboor = *n_it;
			std::pair<Point*, int> fewest_neighboors(neighboor, neighboors[neighboor].size());

			for(;n_it != node_neighboors.end(); ++n_it)
			{
				neighboor = *n_it;
				auto size = neighboors[neighboor].size();
				//Todo: if theres a tie, chose randomly one
				if(size < fewest_neighboors.second)
				{
					fewest_neighboors.first = neighboor;
					fewest_neighboors.second = size;
				}
			}

			node = fewest_neighboors.first;			
		}
	}


	return solution;
}
*/

Solution* Genetic::generate_child(Solution* p1, Solution* p2)
{
	auto solution = new Solution;
	//Route-Based Crossover
	std::list<Vehicle*> vehicle_pool;
	for (auto vehicle : p1->vehicles)
	{
		vehicle_pool.push_back(vehicle);
	}
	for (auto vehicle : p2->vehicles)
	{
		vehicle_pool.push_back(vehicle);
	}

	auto total_nodes = Utils::raw_rows.size() - 1;

	while (!vehicle_pool.empty())
	{
		auto it = vehicle_pool.begin();
		std::advance(it, rand() % vehicle_pool.size());
		auto pool = *it;
		vehicle_pool.erase(it);

		auto nv = new Vehicle(solution);
		solution->vehicles.push_back(nv);
		auto pos = 0;
		for (auto node : pool->nodes)
		{
			if (!solution->has_node(node->p))
			{
				auto ec = 0;
				nv->add_node(node->p, pos++, ec);
				total_nodes--;
			}
		}
		if (nv->nodes.empty())
		{
			solution->vehicles.remove(nv);
			delete nv;
		}
	}

	solution->total_weight();
	return solution;
}

Solution* Genetic::run()
{
	generate_population();
	solutions.sort(solutions_comparator());
	while (true)
	{
		if (!avaliate_population())
		{
			return (*solutions.begin()).first;
		}
		std::list<std::pair<Solution*, Solution*>> parents = select_parents();

		//Generate childs and add to solutions pool
		for (auto parent : parents)
		{
			solutions.push_back(std::pair<Solution*, double>(generate_child(parent.first, parent.second), 0));
		}

		//Mutate solutions
		max_weight = 0;
		for (auto solution : solutions)
		{			
			if(rand() % 5 > 1)
				solution.first->mutate();

			if (solution.first->weight > max_weight)
				max_weight = solution.first->weight;
		}


		//Kill the worst		
		solutions.sort(solutions_comparator());
		auto it = solutions.begin();
		std::advance(it, population_limit);

		auto it2 = it;
		for (; it2 != solutions.end(); ++it2)
			delete (*it2).first;

		solutions.erase(it, solutions.end());

		//std::cout << "Genetic iteration, best solution: " << (*solutions.begin()).first->weight << "\n";
	}
}

void Genetic::clear()
{
	for (auto solution : solutions)
	{
		delete solution.first;
	}
	solutions.clear();
	ticket_max = 0;
	max_weight = 0;
}
