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
        Solution * _bestSolution;

	public:

	Solver
	(
		Instance * instance
	):
		_instance(instance),
        _bestSolution(new Solution({}))
	{}

	void run();

    Solution * getBestSolution() const
    {
        return _bestSolution;
    }
	void runHeuristics(uint16_t& bestInitialLowerBound, uint16_t & bestInitialUpperBound);

	~Solver()
	{
        delete _bestSolution;
    }
};

struct t_arg
{
    Solver * _solver;
    bool  _hasResult;
};
typedef struct t_arg t_arg;

void * runWrapper(void * args);