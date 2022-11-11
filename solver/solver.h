#pragma once

#include "generator/instance.h"
#include "solution.h"
#include "upper_bound.h"
#include "node.h"
#include <queue>
#include <omp.h>



class Solver
{
	private:

	Instance* _instance;

	public:

	Solver
	(
		Instance * instance
	):
		_instance(instance)
	{}

	void run();

	void runHeuristics(uint16_t& bestInitialLowerBound, uint16_t & bestInitialUpperBound, Solution*& bestSequence);

	void runDPUpperBoundClassic();
	~Solver()
	{

	}
};