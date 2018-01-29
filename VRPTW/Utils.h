#pragma once
#include <list>


class VehicleState;
class Point;
class Vehicle;
class Solution;

class Utils
{
public:
	static VehicleState* origin;
	static std::list<Point*> raw_rows;	
	static double** distances;

	struct pfih_comparator
	{
		inline bool operator()  (const std::pair<Point*, double>& inst1, std::pair<Point*, double>& inst2){
			return inst1.second < inst2.second;
		}
	};

	
	static void read_file();
	static void calculate_distances();
	static Solution* pfih();		
	static Solution* random_solution();
	static double random_0_1();
	
};

