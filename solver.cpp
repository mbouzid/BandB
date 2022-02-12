#include "solver.h"
#include <set>


void Solver::run()
{

	uint16_t maxProfit(_instance->Heuristic1());
	_queue.push(Node(-1, 0, 0, 0, 0, utils::emptySet(), utils::emptyVector()));

	while (not _queue.empty())
	{
		Node u = _queue.top();

		//std::cout << "priority=" << u.getPriority() << " ";
		//std::cout << "j=" << u.getJob() << " ,t=" << u.getT() << " ,ub=" << u.getUpperBound() << " ,profit=" << u.getProfit() << ", seq=";
		/*utils::print<uint16_t>(std::cout, u.getSequence());
		std::cout << std::endl;*/

		_queue.pop();

		std::vector<uint16_t> A;

		for (uint16_t i(0); i < _instance->getN(); ++i)
		{
			if ( i != u.getJob() and not u.findInVisited(i) and not u.findInSequence(i) and _instance->getEarliestCompletionTime(i, u.getT()) != -1)
			{
				A.push_back(i);
			}
		}

		/*std::cout << "A=";
		utils::print<uint16_t>(std::cout, A);
		std::cout << std::endl;	*/

		for (int k(0); k < A.size(); ++k)
		{

			uint16_t j(A.at(k));   				
			//std::cout << "j=" << j << std::endl;

			int16_t earliestEndtime(_instance->getEarliestCompletionTime(j, u.getT()));

			if (earliestEndtime == -1 or earliestEndtime >= _instance->getD(j) + 1)
			{
				u.addToVisited(j);
				continue;
			}
			//std::cout << "branch " << std::endl;

			uint16_t profit(u.getProfit());
			uint16_t upperBound(u.getUpperBound());
			uint16_t t(u.getT());

			std::set<uint16_t> visited(u.getVisited());
			std::vector<uint16_t> sequence(u.getSequence());


			if (earliestEndtime != -1)
			{
				t = earliestEndtime;
				profit += _instance->getW(j);
				sequence.push_back(j);
				visited.insert(j);
				upperBound = profit + _instance->DPUpperBound(u.getT(), u.getVisited());

			}
			else
			{
				visited.insert(j);
				u.addToVisited(j);
				std::cout << "not extended" << std::endl;
			}

			if (profit > maxProfit)
			{
				maxProfit = profit;
				std::cout << "maxProfit=" << maxProfit << std::endl;
			}


			if (upperBound > maxProfit)
			{
				_queue.push(Node(j, u.getLevel() + 1, profit, upperBound, t, visited, sequence));

			}
		}

	}	   
	
	std::cout <<  maxProfit << " ";

}