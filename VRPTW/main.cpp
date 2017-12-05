#include <iostream>
#include <ctime>
#include "Utils.h"
#include "Hillclimbing.h"
#include "Genetic.h"
#include "Solution.h"
#include <thread>
#include <stdio.h>
#include <vector>
#define DETERMINISTIC
#define CORE_NUM 1

int main()
{
	Utils::read_file();
	std::cout << "File has been loaded\n";

	Utils::calculate_distances();
	std::cout << "Distances have been calculated\n";

	//Hillclimbing::run();
	srand(time(nullptr));

	std::vector<std::thread> threads;
	for (int i = 0; i < CORE_NUM; ++i)
	{
		threads.push_back(std::thread([]()
		{
			while (true)
			{
#ifdef DETERMINISTIC
				int seed = 1;
#else
				int seed = rand();
				srand(seed);
#endif
				Genetic::population_limit = 100;
				auto solution = Genetic::run();				
				std::cout << "Genetic solution,\tpopulation:\t" << Genetic::population_limit << "\tseed:\t" << seed << "\t\tweight:\t" << solution->total_weight() << "\n";
				Genetic::clear();
			}
		}
		));
	}
	for (auto it = threads.begin(); it != threads.end(); ++it)
	{
		(*it).join();
	}


	std::cout << "End.\n";
	system("pause");
}
