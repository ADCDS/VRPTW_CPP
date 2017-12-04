#pragma once
class Vehicle;

class SubCandidate
{
public:
	double peso;
	int pos;
	Vehicle* v;


	SubCandidate(double peso, int pos, Vehicle* v)
		: peso(peso),
		  pos(pos),
		  v(v)
	{
	}

	bool operator < (const SubCandidate& inst) const{
		return peso < inst.peso;
	}

	bool operator > (const SubCandidate& inst) const {
		return peso > inst.peso;
	}

	SubCandidate();
	~SubCandidate();
};

