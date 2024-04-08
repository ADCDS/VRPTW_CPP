#include "Solution.h"
#include "Vehicle.h"
#include "Utils.h"
#include "SubCandidate.h"
#include "Point.h"
#include "Utils.h"
#include "VehicleState.h"
#include <iostream>
#include <chrono>

double Solution::total_weight() {
    double ret = 0;
    for (auto vehicle: vehicles) {
        ret += vehicle->get_weight();
    }
    weight = ret;
    return ret;
}

void Solution::validate() {
    for (auto vehicle: vehicles) {
        auto i = 0;
        for (auto it = vehicle->nodes.begin(); it != vehicle->nodes.end(); ++it) {
            auto vs = *it;
            auto pos = vs->p->state[this].second;
            if (i != pos) {
                std::cout << "invalid state";
            }
            i++;
        }
    }
}


void Solution::mutate1(bool &improved) {
    Point *p = nullptr;
    int ec = 0;
    double new_weight, original_weight = this->total_weight();

    int index = (rand() % (Utils::raw_rows.size() - 1)) + 1; // We do this to avoid targetting the depot point (id == 0 point)

    auto it = Utils::raw_rows.begin();

    while (index-- > 0)
        ++it;

    p = *it;

    if (p->state.find(this) == p->state.end()) {
        std::cerr << "Solution has no instance of point " << p->id << std::endl;
        exit(0);
    }

    auto original_vehicle = p->state[this].first;
    int original_pos = p->state[this].second;
    //std::cout << "Mutating " << p->id << "\n";
    original_vehicle->remove_node(original_pos);

    /**
     * These two lines bellow are just for test, We are trying to remove node from a vehicle route
     * then insert into a new one, if this insertion provides a valid solution and it is better than the previous one
     * then that solution becomes the base solution.
     *
     * The thing is, most of the times, that insertion will fail, and then we need to put the node back
     * where it once was. This happens in this function down bellow: candidate.v->add_node(p, candidate.pos, ec);
     * But, sadly it is not working.
     *
     * Those commented two lines down bellow this comment are here just for test, after calling ->add_node() ec should
     * be == 0 because we are just inserting a node that we just removed. This bugs happens because Vehicle::validate_states
     * are not implemented correctly. I'll leave a comment there if you want to see.
     */
//    ec = 0;
//    original_vehicle->add_node(p, original_pos, ec);

    SubCandidate candidate(original_weight, original_pos, original_vehicle);

    for (auto vehicle: this->vehicles) {
        //std::cout << "Vehicle " << vehicle->id << "\n";
        for (int i = 0; i <= vehicle->nodes.size(); ++i) {
            //std::cout << "Node " << i << "\n";
            ec = 0;
            vehicle->add_node(p, i, ec);
            if (!ec) {
                new_weight = this->total_weight();
                if (new_weight < candidate.weight) {
                    candidate.weight = new_weight;
                    candidate.v = vehicle;
                    candidate.pos = i;
                    //std::cout << "Best position for node " << p->id << ", vehicle " << candidate.v->id << " position " << candidate.pos << ", total weight " << candidate.weight << "\n";
                    improved = true;

                    return;
                }
                vehicle->remove_node(i);
            }
            //validate();
        }
    }

    ec = 0;

    // Add the node back to its original vehicle
    candidate.v->add_node(p, candidate.pos, ec);
    improved = false;
}

/**
 *	Insertion mutate operator, it can be intra-route or inter-route
 *
 */
void Solution::mutate_insertion() {
    auto vit = vehicles.begin();
    Vehicle *v1;
    Vehicle *v2;
    std::advance(vit, rand() % vehicles.size());
    v1 = *vit;

    if (v1->nodes.size() == 1)
        return;

    vit = vehicles.begin();
    std::advance(vit, rand() % vehicles.size());
    v2 = *vit;

    auto nid = v1->nodes.begin();
    std::advance(nid, rand() % v1->nodes.size());
    auto node = (*nid)->p;
    auto original_position = node->state[this].second;

    v1->remove_node(original_position);
    //tries to insert at a random position
    auto ec = 0;
    v2->add_node(node, rand() % v2->nodes.size(), ec);
    if (ec != 0)//Error
    {
        ec = 0;
        v1->add_node(node, original_position, ec);
    }
}

void Solution::mutate_swap() {

    Vehicle *v1;
    Vehicle *v2;

    auto off = rand() % (vehicles.size() - 2 + 1) + 1;
    auto vit = vehicles.begin();
    std::advance(vit, off);
    v1 = *vit;

    vit = vehicles.begin();
    std::advance(vit, rand() % off);
    v2 = *vit;

    auto nid = v1->nodes.begin();

    std::advance(nid, rand() % v1->nodes.size());
    auto node1 = (*nid)->p;
    nid = v2->nodes.begin();
    std::advance(nid, rand() % v2->nodes.size());
    auto node2 = (*nid)->p;

    auto node1_original_position = node1->state[this].second;
    v1->remove_node(node1_original_position);

    auto node2_original_position = node2->state[this].second;
    v2->remove_node(node2_original_position);

    auto ec = 0;
    v2->add_node(node1, node2_original_position, ec);
    if (ec == 0) {
        v1->add_node(node2, node1_original_position, ec);
        if (ec == 0)//Success
            return;
        else {
            v2->remove_node(node2_original_position);
        }
    }
    //error state
    ec = 0;
    v1->add_node(node1, node1_original_position, ec);
    v2->add_node(node2, node2_original_position, ec);
}

void Solution::mutate_inversion() {
    auto vit = vehicles.begin();
    std::advance(vit, rand() % vehicles.size());
    auto v1 = *vit;

    if (v1->nodes.size() < 3)
        return;
    auto range_max = rand() % (v1->nodes.size() - 2) + 2;
    auto range_min = rand() % range_max;

    auto nit = v1->nodes.begin();
    std::advance(nit, range_min);
    auto nit2 = v1->nodes.begin();
    std::advance(nit2, range_max);

    std::list<std::pair<Point *, int>> points_list;

    while (nit++ != nit2) {
        auto p = (*nit)->p;
        points_list.push_back(std::pair<Point *, int>(p, p->state[this].second));
        //v1->remove_node(p->state[this].second);//invalidate iterators
    }

    int remove_pos = (*points_list.begin()).second;
    for (auto pair: points_list) {
        v1->remove_node(remove_pos);
    }

    auto i = 0;
    for (auto rit = points_list.rbegin(); rit != points_list.rend(); ++rit) {
        auto p = (*rit).first;
        auto ec = 0;
        v1->add_node(p, range_min + i++, ec);
        if (ec != 0) {//Error, repair route
            ec = 0;
            while (--i > 0) {
                v1->remove_node(range_min);
            }
            for (auto pair: points_list) {
                v1->add_node(pair.first, pair.second, ec);
            }
            break;
        }
    }


}


void Solution::mutate() {
    auto improvement = false;
    auto r_operator = rand() % 4;
    //auto r_operator = 3;

    switch (r_operator) {
        default:
        case 0: //Insertion
            mutate1(improvement);
            break;
        case 1:
            mutate_insertion();//leak
            break;
        case 2:
            mutate_swap();//leak
            break;
        case 3:
            mutate_inversion();//leak
            break;
    }
    //mutate1(improvement);

    auto it = vehicles.begin();
    double ret = 0;
    while (it != vehicles.end()) {
        auto v = *it;
        ret += v->get_weight();
        auto pit = it++;
        if (v->nodes.empty()) {
            vehicles.erase(pit);
            delete v;
        }

    }
    weight = ret;

}

bool Solution::has_node(Point *p) {
    for (auto vehicle: vehicles) {
        if (vehicle->is_point_on_vehicle(p))
            return true;
    }
    return false;
}


Solution::~Solution() {
    for (auto vehicle: vehicles) {
        delete vehicle;
    }
    for (auto p: Utils::raw_rows) {
        p->state.erase(this);
    }
}


void Solution::print() {
    for (auto point: Utils::raw_rows) {
        point->state.erase(this);
    }

    for (auto vehicle: vehicles) {
        for (auto node: vehicle->nodes) {
            std::cout << node->p->id << " ";
        }
        std::cout << "\n";
    }
}


Solution *Solution::clone() {
    auto s = new Solution();
    for (auto vehicle: this->vehicles) {
        s->vehicles.push_back(vehicle->clone(s));
    }
    s->weight = this->weight;
    return s;
}
