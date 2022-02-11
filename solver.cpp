#include "solver.h"
#include <set>

void Solver::run()
{

	uint16_t maxProfit(_instance->Heuristic1());
	std::cout << "maxProfit=" << maxProfit << std::endl;

	_queue.push(Node::dummyNode());
	while (not _queue.empty())
	{
		Node* u = _queue.front();
		_queue.pop();


		std::cout << "creating children node" << std::endl;
		std::set<uint16_t> A;

		for (uint16_t i(0); i < _instance->getN(); ++i)
		{
			if (not u->findInVisited(i) and _instance->getEarliestCompletionTime(i,u->getT() != -1))
			{
				A.insert(i);
			}
		}

		for (uint16_t i : A)
		{

			std::cout << "Branching on " << i << std::endl;
			_instance->DPUpperBound(u->getT(), u->getVisited());

		}


	}

}