#pragma once

#include "instance.h"
#include "node.h"
#include <queue>
#include <omp.h>



class Solver
{
	private:

	Instance* _instance;
	std::priority_queue<Node, std::vector<Node>, std::less<Node>> _queue;


	public:

	Solver
	(
		Instance * instance
	):
		_instance(instance)
	{}

	void run();

	~Solver()
	{

	}
};