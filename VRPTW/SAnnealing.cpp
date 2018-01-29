#include "SAnnealing.h"
#include "Utils.h"
#include <iostream>
#include <ppltasks.h>


Solution* SAnnealing::run(double initial_temp, double final_temp, double t_decrese)
{
	auto best_solution = Utils::pfih();	
	auto candidate = best_solution;	 
	auto current_temp = initial_temp;
	while(current_temp > final_temp)
	{		
		auto next_solution = candidate->clone();
		next_solution->mutate();
		auto delta = next_solution->weight - candidate->weight;
		if(delta < 0 || exp(-delta/current_temp) > Utils::random_0_1())
		{
			if(candidate != best_solution)
				delete candidate;

			candidate = next_solution;
			std::cout << "SA -> Next solution: " << candidate->weight << ", Delta: " << delta << "\n";

			if (best_solution->weight > candidate->weight)
				best_solution = candidate;
		}
		current_temp -= t_decrese;
		std::cout << "T: " << current_temp << "\n";
	}
	return best_solution;
}
