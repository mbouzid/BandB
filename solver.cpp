#include "solver.h"
#include <set>


void Solver::run()
{

	uint16_t maxProfit(_instance->Heuristic1());
	_queue.push(Node(-1, 0, 0, 0, 0, utils::emptySet(), utils::emptySet()));

	while (not _queue.empty())
	{
		Node u = _queue.top();

		_queue.pop();

		std::vector<uint16_t> A;

		for (uint16_t i(0); i < _instance->getN(); ++i)
		{
			if (not u.findInVisited(i) and _instance->getEarliestCompletionTime(i, u.getT() != -1))
			{
				A.push_back(i);
			}
		}

		for (int k(0); k < A.size(); k++)
		{

			uint16_t j(A.at(k));   				

			int16_t earliestEndtime(_instance->getEarliestCompletionTime(j, u.getT()));

			if (earliestEndtime == -1 or earliestEndtime >= _instance->getD(j) + 1)
			{
				u.addToVisited(j);
				continue;
			}

			uint16_t profit(u.getProfit());
			uint16_t upperBound(0);
			uint16_t t(u.getT());

			std::set<uint16_t> visited(u.getVisited());
			std::set<uint16_t> sequence(u.getSequence());


			if (earliestEndtime != -1)
			{
				t = earliestEndtime;
				profit += _instance->getW(j);
				sequence.insert(j);
				visited.insert(j);
				upperBound = profit + _instance->DPUpperBound(u.getT(), u.getVisited());

			}

			if (profit > maxProfit)
			{
				maxProfit = profit;
			}


			if (upperBound > maxProfit)
			{
				_queue.push(Node(j, u.getLevel() + 1, profit, upperBound, t, visited, sequence));

			}
		}

	}	   
	
	std::cout << "maxProfit=" << maxProfit << std::endl;

}