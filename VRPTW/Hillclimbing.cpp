#include "Hillclimbing.h"
#include "Solution.h"
#include "Utils.h"
#include <iostream>

Solution* Hillclimbing::solution;

Solution* Hillclimbing::run(int max_fails)
{
		
	solution = Utils::pfih();
	//std::cout << "Push Foward Insert Heuristic calculated\n";
	//std::cout << "Initial weight: " << solution->total_weight() << "\n";

	auto consecutive_fails = 0;
	auto improved = false;
	while(consecutive_fails < max_fails)
	{
		solution->mutate1(improved);
		if (!improved) {
			consecutive_fails++;
		}else
		{
			consecutive_fails = 0;
		}

	}
	return solution;
}