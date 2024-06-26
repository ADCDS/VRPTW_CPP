#include "Vehicle.h"
#include "Utils.h"
#include <iostream>
#include "Utils.h"
#include "VehicleState.h"
#include "Point.h"

/*void Vehicle::valdebug()
{
	int i = 0;
	for (auto node : nodes)
	{
		if(node->p->state[solution].second != i++)
		{
			std::cout << "erro";
		}
	}
}*/
void Vehicle::reevaluate_route(int start, int end) {
    std::list<std::shared_ptr<VehicleState>>::iterator li;
    std::shared_ptr<VehicleState> last;
    //start = 0;
    //end = nodes.size();

    if (start == 0) {
        li = nodes.begin();
        last = Utils::origin;
    } else {
        li = nodes.begin();
        int tmp_start = start;
        while (tmp_start-- > 0)
            ++li;
        last = *std::prev(li);

    }
    std::shared_ptr<VehicleState> curr;
    while (++start <= end && li != nodes.end()) {
        curr = *li;
        li++;
        curr->current_cargo = last->current_cargo - curr->p->demand;
        curr->current_distance = last->current_distance + Utils::distances[last->p->id][curr->p->id];
        curr->current_time = last->current_time + Utils::distances[last->p->id][curr->p->id];
        curr->p->state[solution].second = start - 1;

        if (curr->current_time < curr->p->ready_time) {
            curr->current_time = curr->p->ready_time;
        }
        curr->current_time += curr->p->service_time;

        last = curr;
    }
}

bool Vehicle::is_point_on_vehicle(Point *p) {
    for (auto value_: nodes) {
        if (value_->p->id == p->id) {
            return true;
        }
    }
    return false;
}

void Vehicle::validate_states(std::shared_ptr<VehicleState> possible_state, int position, int &ec) {
    /**
     * So, here we are, validate_states should take a "possible_state" that is basically a point (or a customer)
     * and try to insert into the current vehicle's route. it should return ec == 0 if it is possible to add the point
     * into the route, otherwise it should return ec != 0
     */
    if (!nodes.empty()) {
        std::list<std::shared_ptr<VehicleState>>::iterator li;
        std::shared_ptr<VehicleState> last_state;

        int last_state_position;
        if (position == 0) {
            li = nodes.begin();
            last_state = Utils::origin;
            last_state_position = -1;
        } else {
            li = nodes.begin();
            int tmp_position = position;
            while (tmp_position-- > 0)
                ++li;

            last_state = *std::prev(li);
            last_state_position = last_state->p->state[solution].second;
        }
        int iter_num = position;
        double dist;

        std::shared_ptr<VehicleState> current_state;
        /**
         * This iteration bellow is causing the problems, sadly im not able to fix it.
         * But the idea is that I need to add the possible_point into the vehicle route
         * and then, for each point that is after the inserted, calculate its constraints and
         * determine if the route is feasible.
         */
        for (; li != nodes.end(); ++li) {
            current_state = *li;
            dist = Utils::distances[last_state->p->id][current_state->p->id];
            current_state->current_distance = last_state->current_distance + dist;
            current_state->current_time = last_state->current_time + dist;

            if (current_state->current_time > current_state->p->due_date) {
                if (iter_num > 0) {
                    reevaluate_route(position, iter_num);
                }
                ec = 1;
                return;
            }

            current_state->current_cargo = last_state->current_cargo - current_state->p->demand;

            if (current_state->current_time <
                current_state->p->ready_time) { // Arrived early, need to wait for the point to become active
                current_state->current_time = current_state->p->ready_time;
            }
            if (current_state->current_cargo < 0) {
                if (iter_num > 0) {
                    reevaluate_route(position, iter_num);
                }
                ec = 4;
                return;
            }

            current_state->current_time += current_state->p->service_time;
            current_state->p->state[solution].second = last_state_position + 1;
            last_state_position = current_state->p->state[solution].second;
            last_state = current_state;
            iter_num++;
        }
    } else {
        possible_state->current_distance = Utils::distances[Utils::origin->p->id][possible_state->p->id];
        possible_state->current_time = possible_state->current_distance;
        if (possible_state->current_time > possible_state->p->due_date) {
            ec = 2;
            return;
        }
        if (possible_state->current_time < possible_state->p->ready_time) {
            possible_state->current_time = possible_state->p->ready_time;
        }
        possible_state->current_cargo -= possible_state->p->demand;
        possible_state->current_time += possible_state->p->service_time;
        possible_state->p->state[solution].second = 0;
    }

    if(this->nodes.empty())
        return; // I guess it's safe to simply return here if there are no nodes

    if (get_last_node()->current_time + Utils::distances[get_last_node()->p->id][Utils::origin->p->id] >
        Utils::origin->p->due_date) {
        ec = 3;
        return;
    }

}

void Vehicle::add_node(Point *p, int pos, int &ec) {

    auto possible_state = std::make_shared<VehicleState>(p);
    validate_states(possible_state, pos, ec);
    if (ec)
        return;

    auto it = nodes.begin();
    auto tmp_pos = pos;
    while (tmp_pos-- > 0)
        ++it;

    nodes.insert(it, possible_state);
    p->state[solution].first = this;
}

void Vehicle::remove_node(int pos) {
    auto li = nodes.begin();

    std::shared_ptr<VehicleState> last;

    int c_pos = pos;
    while (c_pos-- > 0)
        ++li;

    if (pos == 0) {
        last = Utils::origin;
    } else {
        last = *std::prev(li);
    }

    nodes.erase(li++);

    std::shared_ptr<VehicleState> curr;
    while (li != nodes.end()) {
        curr = *li;
        curr->current_cargo = last->current_cargo - curr->p->demand;
        curr->current_distance = last->current_distance + Utils::distances[last->p->id][curr->p->id];
        curr->current_time = last->current_time + Utils::distances[last->p->id][curr->p->id];

        if (curr->current_time < curr->p->ready_time) {
            curr->current_time = curr->p->ready_time;
        }
        curr->current_time += curr->p->service_time;

        curr->p->state[solution].second = curr->p->state[solution].second - 1;
        last = curr;
        ++li;
    }
}

double Vehicle::get_weight()
const {
    return this->get_last_node()->current_distance + Utils::distances[get_last_node()->p->id][Utils::origin->p->id];
}

Vehicle::Vehicle(Solution *s) {
    solution = s;
}


Vehicle *Vehicle::clone(Solution *s) {
    auto v = new Vehicle(s);
    v->id = this->id;
    std::shared_ptr<VehicleState> ln = nullptr;
    for (auto node: this->nodes) {
        auto clone = node->clone();
        ln = clone;
        clone->p->state[s] = {v, node->p->state[this->solution].second};
        v->nodes.push_back(clone);
    }
    return v;
}
