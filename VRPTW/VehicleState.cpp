#include "VehicleState.h"
#include "Point.h"


VehicleState::VehicleState()
{
}


VehicleState::~VehicleState()
{
}


VehicleState* VehicleState::clone() const {
	auto vs = new VehicleState(p);
	vs->current_cargo = current_cargo;
	vs->current_time = current_time;
	vs->current_distance = current_distance;
	return vs;
}
