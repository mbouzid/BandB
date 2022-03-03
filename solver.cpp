#include "solver.h"
#include <set>
#include <iostream>
#include <fstream>


#include <chrono>
#include <ctime>

void Solver::run()
{




	std::vector < std::vector<uint16_t> > heuristics = 
	{ 
		_instance->Heuristic1(core::heuristic_ratio::RATIO_A), 
		_instance->Heuristic1(core::heuristic_ratio::RATIO_B),
		_instance->Heuristic1(core::heuristic_ratio::RATIO_C) ,
		_instance->Heuristic3() 
	};
	
	std::vector<uint16_t> bestSequence(*std::max_element(heuristics.begin(), heuristics.end(),
		[this](const std::vector<uint16_t>& x, const std::vector<uint16_t>& y){ return _instance->computeProfit(x) <= _instance->computeProfit(y); }));

	uint16_t lowerBound(_instance->computeProfit(bestSequence));	
	
	// upperBound
	uint16_t initialUpperBound(0);
	for (uint16_t i(0); i < _instance->getN(); ++i)
	{
		initialUpperBound += _instance->getW(i);
	}

	initialUpperBound = std::min(initialUpperBound, _instance->MILPUpperBound(0, {}, omp_get_thread_num()));
	initialUpperBound = std::min(initialUpperBound, _instance->DPUpperBound(0, {}));



	_queue.push(Node(-1, 0, 0, initialUpperBound, 0, utils::emptySet(), utils::emptyVector()));

	for (size_t k(0); k < heuristics.size(); ++k)
		std::cout << _instance->computeProfit(heuristics.at(k)) << " ";


	/*auto start = std::chrono::system_clock::now();
	std::ofstream f("save.out"); */



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

		#pragma omp parallel for shared(lowerBound), num_threads(8)
		for (int k(0); k < A.size(); ++k)
		{
			uint16_t j(A.at(k));   				

			int16_t earliestEndtime(_instance->getEarliestCompletionTime(j, u.getT()));

			

			uint16_t incumbentProfit(u.getProfit());
			uint16_t upperBound(u.getUpperBound());
			uint16_t t(u.getT());

			std::set<uint16_t> visited(u.getVisited());


			std::vector<uint16_t> sequence(u.getSequence());


			t = earliestEndtime;
			incumbentProfit += _instance->getW(j);
			sequence.push_back(j);
			visited.insert(j);

				
			uint16_t ratio((double)_instance->getW(j) /(double) t);
				
			uint16_t ub1 (ratio + _instance->DPUpperBound(t, visited));
			if (ub1 == upperBound)
			{
				continue;
			}

			upperBound = std::min(ub1, upperBound);
			
			

			
			// UPPERBOUND
			/*if (visited.size() <= std::round(0.1 * _instance->getN()))
			{
				uint16_t ub2 =  _instance->MILPUpperBound(t, visited, omp_get_thread_num());
				upperBound = std::min(upperBound, ub2);
			}*/ 

			/*if (visited.size() >= 0.8 * _instance->getN())
			{
				uint16_t ub3( _instance->MooreUpperBound(t, visited));
				upperBound = std::min(upperBound, ub3);
			}*/

			/*
				// LOWER BOUNDS
				uint16_t lb1(_instance->Heuristic1LowerBound(t, sequence, visited, core::heuristic_ratio::RATIO_A));
				uint16_t lb2(_instance->Heuristic1LowerBound(t, sequence, visited, core::heuristic_ratio::RATIO_B));
				uint16_t lb3(_instance->Heuristic1LowerBound(t, sequence, visited, core::heuristic_ratio::RATIO_C));

				uint16_t lbmax(std::max(lb1, std::max(lb2, lb3)));
				if (lbmax >= lowerBound)
				{
					lowerBound = lbmax;
					continue;
				 }

				if (lbmax == upperBound)
				{
					continue;
				}*/



			if (incumbentProfit > lowerBound)
			{
				lowerBound = incumbentProfit;
				bestSequence = sequence;
			}
				

			#pragma omp critical
			{
				if (upperBound + incumbentProfit > lowerBound)
				{

					_queue.push(Node(j, u.getLevel() + 1, incumbentProfit, upperBound, t, visited, sequence));

				}
			}
		}

	}	   
	
	std::cout << " " << lowerBound << " \"[";
	utils::print<uint16_t>(std::cout, bestSequence);		 
	std::cout << "]\" ";		 


	//f.close();
	/*auto end1 = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds1 = end1 - start;
	std::time_t end_time1 = std::chrono::system_clock::to_time_t(end1);

	f << elapsed_seconds1.count() << " " << maxProfit << std::endl;
	f.close();*/

}