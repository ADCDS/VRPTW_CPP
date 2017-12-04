#pragma once
#include <list>

class Solution;

class Genetic
{
public:
	static int population_limit;
	static std::list<std::pair<Solution*, double/*avaliation*/>> solutions;
	static double ticket_max;
	static double max_weight;

	static void generate_population();	
	static std::list<std::pair<Solution*, Solution*>> select_parents();
	static Solution* generate_child(Solution* p1, Solution* p2);
	static bool avaliate_population();
	static Solution* run();
	static void clear();

	
};

