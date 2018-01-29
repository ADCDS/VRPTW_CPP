
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "Utils.h"
#include "Point.h"
#include "Vehicle.h"
#include "SubCandidate.h"
#include "VehicleState.h"
#include "Solution.h"
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

VehicleState* Utils::origin;
std::list<Point*> Utils::raw_rows;
double** Utils::distances;

void Utils::read_file()
{
	std::ifstream inst;

	inst.open("../Instances/S-RC2-1000/RC21010.TXT");
	//inst.open("../Instances/C101.TXT");
	//inst.open("../Instances/RC105.txt");
	if (inst.is_open())
	{
		std::string line;
		std::regex e("([0-9]+)");
		std::smatch sm;
		int line_count = 0;
		int aux[7];
		int rr_count = 0;
		while (std::getline(inst, line))
		{
			line_count++;
			int i = 0;
			if (line_count > 9)
			{
				while (std::regex_search(line, sm, e))
				{
					auto s = sm.str();
					aux[i++] = atoi(s.data());
					line = sm.suffix();
				}
				Utils::raw_rows.push_back(new Point(aux[0], aux[1], aux[2], aux[3], aux[4], aux[5], aux[6]));
			}
		}
		Utils::origin = new VehicleState(*Utils::raw_rows.begin());		
		Utils::distances = (double**)malloc(sizeof(double*) * Utils::raw_rows.size());
		for (int i = 0; i < Utils::raw_rows.size(); ++i)
		{
			Utils::distances[i] = (double*)malloc(sizeof(double) * Utils::raw_rows.size());
		}
		inst.close();
	}
	else
	{
		std::cout << "Unable to open file";
		exit(1);
	}
}

void Utils::calculate_distances()
{
	for (Point* point : Utils::raw_rows)
	{
		for (Point* point2 : Utils::raw_rows)
		{
			Utils::distances[point->id][point2->id] = point->distance(point2);
		}
	}
}

Solution* Utils::pfih()
{
	std::vector<std::pair<Point*, double>> points(Utils::raw_rows.size() - 1);
	auto it = Utils::raw_rows.begin();
	auto origin = *it;
	int ec = 0;
	++it;

	Point* curr;
	double degreesdeposit = atan2(origin->y, origin->x) * (180.0 / M_PI);
	double newdegree = 0;
	while (it != Utils::raw_rows.end())
	{
		curr = *it;
		newdegree = atan2(curr->y, curr->x) * (180.0 / M_PI);		
		double d = Utils::distances[0][curr->id];
		points[curr->id - 1] = std::pair<Point*, double>(curr, (d * -0.7) + (0.1 * curr->due_date) + (0.2 * (newdegree - degreesdeposit) / 360 * d));
		++it;
	}


	std::sort(points.begin(), points.end(), Utils::pfih_comparator());


	//Creates a solution
	auto solution = new Solution();
	
	auto v = new Vehicle(solution);
	v->id = 0;
	solution->vehicles.push_back(v);
	v->add_node(points[0].first, 0, ec);
	std::list<SubCandidate> candidates;

	for (auto it = ++points.begin(); it != points.end(); ++it)
	{
		auto point = *it;
		for (auto vit = solution->vehicles.begin(); vit != solution->vehicles.end(); ++vit)
		{
			auto vehicle = *vit;
			for (int i = 0; i <= vehicle->nodes.size(); i++)
			{
				ec = 0;
				vehicle->add_node(point.first, i, ec);
				if (ec == 0)
				{
					SubCandidate sb(solution->total_weight(), i, vehicle);
					candidates.push_back(sb);
					vehicle->remove_node(i);
				}
			}
		}

		if (!candidates.empty())
		{
			candidates.sort();
			auto chosen_one = *(candidates.begin());
			ec = 0;
			chosen_one.v->add_node(point.first, chosen_one.pos, ec);
			point.first->state.insert({ solution, std::pair<Vehicle*, int>(chosen_one.v, chosen_one.pos) });
			candidates.clear();
		}
		else
		{
			auto nv = new Vehicle(solution);
			nv->id = solution->vehicles.size();
			solution->vehicles.push_back(nv);
			ec = 0;
			nv->add_node(point.first, 0, ec);
			point.first->state.insert({ solution, std::pair<Vehicle*, int>(nv, 0) });
		}
	}

	return solution;
}

Solution* Utils::random_solution()
{
	auto solution = new Solution;
	auto v = new Vehicle(solution);
	solution->vehicles.push_back(v);

	auto point_pool = Utils::raw_rows;
	point_pool.erase(point_pool.begin());
	while(!point_pool.empty())
	{
		auto pit = point_pool.begin();
		std::advance(pit, rand() % point_pool.size());
		auto p = *pit;
		point_pool.erase(pit);
		auto ec = 0;
		for (auto vehicle : solution->vehicles)
		{
			for (auto i = 0; i <= vehicle->nodes.size(); ++i)
			{			
				ec = 0;
				vehicle->add_node(p, i, ec);
				if (ec == 0)
					goto leave;
			}			
		}

		if(ec != 0)
		{
			v = new Vehicle(solution);
			solution->vehicles.push_back(v);
			auto ec = 0;
			v->add_node(p, 0, ec);
		}
	leave:;
	}
	solution->total_weight();
	return solution;
}


double Utils::random_0_1()
{
	return ((double)rand() / (RAND_MAX));
}
