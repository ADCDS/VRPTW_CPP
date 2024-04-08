#pragma once
class Vehicle;

class SubCandidate
{
public:
	double weight;
	int pos;
	Vehicle* v;


	SubCandidate(double weight, int pos, Vehicle* v)
		: weight(weight),
          pos(pos),
          v(v)
	{
	}

	bool operator < (const SubCandidate& inst) const{
		return weight < inst.weight;
	}

	bool operator > (const SubCandidate& inst) const {
		return weight > inst.weight;
	}

	SubCandidate();
	~SubCandidate();
};

