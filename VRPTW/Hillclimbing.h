#pragma once

class Solution;

class Hillclimbing
{
public:
	static Solution* solution;
	static Solution* run(int max_fails = 10000);
};

