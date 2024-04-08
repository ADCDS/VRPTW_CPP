#include "SAnnealing.h"
#include "Utils.h"
#include <iostream>
#include <ppltasks.h>


Solution* SAnnealing::run(double initial_temp, double final_temp, double t_decrese)
{	
	auto candidate = Utils::pfih();
	auto best_solution = candidate->clone();
	auto current_temp = initial_temp;
	while(current_temp > final_temp)
	{		
		auto next_solution = candidate->clone();
		next_solution->mutate();
		auto delta = next_solution->weight - candidate->weight;
		if(delta < 0 || exp(-delta/current_temp) > Utils::random_0_1())
		{
			
			delete candidate;
			candidate = next_solution;
			std::cout << "SA -> Next solution: " << candidate->weight << ", T: " << current_temp << ", " << "Best: " << best_solution->weight << ", Delta: " << delta << "\n";

			
			if (best_solution->weight > candidate->weight) {
				delete best_solution;
				best_solution = candidate->clone();
			}
		}else
		{
			delete next_solution;
		}
		current_temp -= t_decrese;
		//std::cout << "T: " << current_temp << "\n";
	}
	return best_solution;
}
