#pragma once
#include <memory>
class Point;

class VehicleState
{
public:
	Point* p;
	int current_cargo = 1000;
	double current_time = 0;
	double current_distance = 0;
	std::shared_ptr<VehicleState> clone() const;



	explicit VehicleState(Point* p)
		: p(p)
	{
	}



	VehicleState();
	~VehicleState();
};

