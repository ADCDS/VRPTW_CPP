#define CORE_NUM 1
//#define DETERMINISTIC

#include <iostream>
#include <ctime>
#include "Utils.h"
#include "Hillclimbing.h"
#include "Genetic.h"
#include "Solution.h"
#include <thread>
#include <stdio.h>
#include <vector>
#include "SAnnealing.h"


int main()
{
	Utils::read_file();
	std::cout << "File has been loaded\n";

	Utils::calculate_distances();
	std::cout << "Distances have been calculated\n";

	//Hillclimbing::run();
	srand(time(nullptr));

	std::vector<std::thread> threads;
	for (auto i = 0; i < CORE_NUM; ++i)
	{
		threads.push_back(std::thread([]()
		{
#ifdef DETERMINISTIC
			auto seed = 1;
#else
			int seed = time(nullptr);
#endif
			srand(seed);
			//Hillclimbing
			/*std::cout << "Seed: " << seed << "\n";
			std::cout << "Hilclimbing, 200 solutions, max_fails: 10000\n";
			for (auto j = 0; j < 1000; ++j)
			{
				auto solution = Hillclimbing::run(10000);
				std::cout << solution->weight << ", ";
				delete solution;
			}*/

			//Genetic algorithm
			/*std::cout << "\nGenetic algorithm, 200 solutions, population limit: 10; max_iter: 10000\n";
			for(auto j = 0; j < 200; ++j)
			{				
				Genetic::population_limit = 30;
				auto solution = Genetic::run(10000);
				std::cout << solution->weight << ", ";
				Genetic::clear();				
			}*/

			//Simulated annealing
			std::cout << "\nSimulated annealing, 200 solutions, temp: 100; min_temp: 0; tmp_lost_per_it: 0.01\n";
			for (auto j = 0; j < 1; ++j)
			{
				auto solution = SAnnealing::run(100, 0, 0.00001);
				std::cout << solution->weight << ", ";
				delete solution;
			}
		}
		));
	}
	for (auto it = threads.begin(); it != threads.end(); ++it)
	{
		(*it).join();
	}


	std::cout << "\nEnd.\n";
	system("pause");
}
