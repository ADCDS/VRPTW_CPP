#pragma once
#include <unordered_map>
#include <mutex>

class Vehicle;
class Solution;

class Point
{
public:
	int id;
	int x;
	int y;
	int demand;
	int ready_time;
	int due_date;
	int service_time;


    /**
     * Every point is attended by a vehicle in a solution
     */
	std::unordered_map<Solution*, std::pair<Vehicle*, int /*pos*/>> state;
	

	Point(int id, int x, int y, int demand, int ready_time, int due_date, int service_time)
		: id(id),
		  x(x),
		  y(y),
		  demand(demand),
		  ready_time(ready_time),
		  due_date(due_date),
		  service_time(service_time)
	{
	}

	double distance(Point* p);
	Point();
	~Point();
};

