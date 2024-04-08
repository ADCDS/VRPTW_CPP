#include "VehicleState.h"
#include "Point.h"


VehicleState::VehicleState()
{
}


VehicleState::~VehicleState()
{
}


std::shared_ptr<VehicleState> VehicleState::clone()
const {
	auto vs = std::make_shared<VehicleState>();
	vs->p = this->p;
	
	vs->current_cargo = this->current_cargo;
	vs->current_time = this->current_time;
	vs->current_distance = this->current_distance;
	return vs;
}
