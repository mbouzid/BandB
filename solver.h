#pragma once

#include "instance.h"
#include "node.h"
#include <queue>


class Solver
{
	private:

	Instance* _instance;
	std::queue<Node*> _queue;


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