#include "solver.h"

void Solver::run()
{

	uint16_t maxProfit(_instance->Heuristic1());

	_queue.push(Node::dummyNode());
	while (not _queue.empty())
	{
		Node* u = _queue.front();

		_queue.pop();


	}

}