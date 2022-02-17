#include "solver.h"
#include <set>
#include <iostream>
#include <fstream>


#include <chrono>
#include <ctime>

void Solver::run()
{

	//auto start = std::chrono::system_clock::now();

	std::vector < std::vector<uint16_t> > heuristics = { _instance->Heuristic1()  , _instance->Heuristic2() , _instance->Heuristic3() };
	
	std::vector<uint16_t> bestSequence(*std::max_element(heuristics.begin(), heuristics.end(),
		[this](const std::vector<uint16_t>& x, const std::vector<uint16_t>& y){ return _instance->computeProfit(x) <= _instance->computeProfit(y); }));

	uint16_t maxProfit(_instance->computeProfit(bestSequence));	
	

	_queue.push(Node(-1, 0, 0, 0, 0, utils::emptySet(), utils::emptyVector()));

	for (size_t k(0); k < heuristics.size(); ++k)
		std::cout << _instance->computeProfit(heuristics.at(k)) << " ";


	//std::ofstream f("save.out");
	
	/*auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	f << elapsed_seconds.count() << " "<< maxProfit << std::endl;
	start = std::chrono::system_clock::now();  */


	while (not _queue.empty())
	{	   
#
		Node u(_queue.top());
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

		#pragma omp parallel for shared(maxProfit), num_threads(4)
		for (int k(0); k < A.size(); ++k)
		{
			uint16_t j(A.at(k));   				

			int16_t earliestEndtime(_instance->getEarliestCompletionTime(j, u.getT()));

			

			uint16_t profit(u.getProfit());
			uint16_t upperBound(0);
			uint16_t t(u.getT());

			std::set<uint16_t> visited(u.getVisited());


			std::vector<uint16_t> sequence(u.getSequence());


				t = earliestEndtime;
				profit += _instance->getW(j);
				sequence.push_back(j);
				visited.insert(j);
				
				upperBound = profit + _instance->DPUpperBound(t, visited);

			if (profit > maxProfit)
			{
				maxProfit = profit;
				bestSequence = sequence;
			}


			#pragma omp critical
			{
				if (upperBound > maxProfit)
				{

					_queue.push(Node(j, u.getLevel() + 1, profit, upperBound, t, visited, sequence));

				}
			}
		}

	}	   
	
	std::cout << " " << maxProfit << " \"[";
	utils::print<uint16_t>(std::cout, bestSequence);		 
	std::cout << "]\" ";		 


	/*auto end1 = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds1 = end1 - start;
	std::time_t end_time1 = std::chrono::system_clock::to_time_t(end1);

	f << elapsed_seconds1.count() << " " << maxProfit << std::endl;
	f.close();*/

}