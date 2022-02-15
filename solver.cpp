#include "solver.h"
#include <set>


void Solver::run()
{

	std::vector < std::vector<uint16_t> > heuristics = { _instance->Heuristic1()  , _instance->Heuristic2(), _instance->Heuristic3() };
	
	std::vector<uint16_t> bestSequence(*std::max_element(heuristics.begin(), heuristics.end(),
		[this](const std::vector<uint16_t>& x, const std::vector<uint16_t>& y){ return _instance->computeProfit(x) <= _instance->computeProfit(y); }));

	uint16_t maxProfit(_instance->computeProfit(bestSequence));	


	_queue.push(Node(-1, 0, 0, 0, 0, utils::emptySet(), utils::emptyVector()));

	std::cout << maxProfit << " " ;

	
	while (not _queue.empty())
	{
		Node u (_queue.top());
		_queue.pop();


		for (uint16_t i(0); i < _instance->getN(); ++i)
		{
			if (not u.findInVisited(i))
			{
				if (_instance->getEarliestCompletionTime(i, u.getT()) == -1 )
				{
					u.addToVisited(i);
				 }
			}
		}
		std::vector<uint16_t> A;		

		for (uint16_t i(0); i < _instance->getN(); ++i)
		{
			if ( i != u.getJob() and not u.findInVisited(i) and not u.findInSequence(i))
			{
				A.push_back(i);
			}
		}

		for (int k(0); k < A.size(); ++k)
		{

			uint16_t j(A.at(k));   				

			int16_t earliestEndtime(_instance->getEarliestCompletionTime(j, u.getT()));

			

			uint16_t profit(u.getProfit());
			uint16_t upperBound(0);
			uint16_t t(u.getT());

			std::set<uint16_t> visited(u.getVisited());


			std::vector<uint16_t> sequence(u.getSequence());


			/*if (earliestEndtime != -1)
			{*/
				t = earliestEndtime;
				profit += _instance->getW(j);
				sequence.push_back(j);
				visited.insert(j);
				
				upperBound = profit + _instance->DPUpperBound(t, visited);

			/*}
			else
			{
				upperBound = profit;
				visited.insert(j);
				u.addToVisited(j);
			}*/


			if (profit > maxProfit)
			{
				maxProfit = profit;
				bestSequence = sequence;
			}


			if (upperBound > maxProfit)
			{
				_queue.push(Node(j, u.getLevel() + 1, profit, upperBound, t, visited, sequence));

			}
		}

	}	   
	
	std::cout << " " << maxProfit << " [";
	utils::print<uint16_t>(std::cout, bestSequence);		 
	std::cout << "] ";		 


}