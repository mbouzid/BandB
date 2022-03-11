#pragma once

#include "instance.h"
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

	void runHeuristics(uint16_t& bestInitialLowerBound, uint16_t & bestInitialUpperBound, std::vector<uint16_t>& bestSequence);

	void runDPUpperBoundClassic();
	~Solver()
	{

	}
};