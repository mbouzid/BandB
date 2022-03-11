#include "solver.h"
#include <set>
#include <iostream>
#include <fstream>


#include <chrono>
#include <ctime>

void Solver::run()
{

	uint16_t initialLowerBound, initialUpperBound;
	std::vector<uint16_t> bestSequence;


	runHeuristics(initialLowerBound, initialUpperBound, bestSequence);


	//exit(0);
	uint16_t lowerBound (initialLowerBound);


	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> _queue;


	_queue.push(Node(-1, 0, 0, initialUpperBound, 0, utils::emptySet(), utils::emptyVector()));


	/*auto start = std::chrono::system_clock::now();
	std::ofstream f("save.out"); */



	while (not _queue.empty())
	{	   

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


			uint16_t ub1(upperBound);



			if (incumbentProfit + upperBound <= lowerBound)
			{
				continue;
			} 


			double gap(((double)(incumbentProfit + upperBound) - (double)lowerBound) / (double)(incumbentProfit+upperBound));
			//std::cout << gap << std::endl;

			if (gap <= 0.01)
			{

				//std::cout << "gap=" << gap << ", UB=" << upperBound << ", LB=" << lowerBound << std::endl;
				//std::cout << "profit + UB = " << upperBound + incumbentProfit << std::endl;
				//ub1 = _instance->MILPUpperBound(t, visited, omp_get_thread_num());
				ub1 = _instance->DPUpperBound(t, visited);

				uint16_t profitUB1(_instance->checkProfit(t,visited));


				// promesse == profit total
				if (profitUB1 == ub1)
				{
					//std::cout << "profitUB1=" << profitUB1 << std::endl;
					if (profitUB1 + incumbentProfit > lowerBound)
					{
						lowerBound = profitUB1 + incumbentProfit;
					}
					continue;
				}
				//std::cout << "ub1=" << ub1 << std::endl;

			}
			else
			{
				ub1 = _instance->DPUpperBound(t, visited);

			}
			
		
			/*if (ub1 < upperBound)
			{
				std::cout << "UB=" << ub1 << std::endl;
			}*/
			upperBound = std::min(ub1, upperBound);


			/*#pragma omp critical
			{
				u.setUpperBound(upperBound);
			}*/

			
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

	char delim(';');
	
	_instance->printCharacteristics(std::cout,delim);
	std::cout << delim << lowerBound;
	std::cout << delim << "BandB" ;


	
	//_instance->printSequence(bestSequence);

	//f.close();
	/*auto end1 = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds1 = end1 - start;
	std::time_t end_time1 = std::chrono::system_clock::to_time_t(end1);

	f << elapsed_seconds1.count() << " " << maxProfit << std::endl;
	f.close();*/

}

void Solver::runHeuristics(uint16_t& bestInitialLowerBound, uint16_t& bestInitialUpperBound, std::vector<uint16_t> & bestSequence)
{
	auto start = std::chrono::system_clock::now();
	std::vector<double> durationLB;

	std::vector < std::vector<uint16_t> > heuristics =
	{
		_instance->Heuristic(core::heuristic_name::INSERT, core::heuristic_ratio::RATIO_A, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT, core::heuristic_ratio::RATIO_B, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT, core::heuristic_ratio::RATIO_C, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::NO_RATIO, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_A, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_B, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_C, start, durationLB)
	};

	bestSequence = *std::max_element(heuristics.begin(), heuristics.end(),
		[this](const std::vector<uint16_t>& x, const std::vector<uint16_t>& y) { return _instance->computeProfit(x) <= _instance->computeProfit(y); });

	bestInitialLowerBound = _instance->computeProfit(bestSequence);


	std::vector <double> durationUB;
	std::vector < uint16_t > upperBounds =
	{
		_instance->UpperBound(core::upperBound_name::Moore,0,{},omp_get_thread_num(), start, durationUB),
		_instance->UpperBound(core::upperBound_name::DP,0,{},omp_get_thread_num(), start, durationUB)
	};

	bestInitialUpperBound = *std::max_element(upperBounds.begin(), upperBounds.end(),
		[this](uint16_t x, uint16_t y) { return x >= y; });


	char delim(';');

	/* OUTPUT */
	for (size_t k(0); k < heuristics.size(); ++k)
	{
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << _instance->computeProfit(heuristics.at(k));
		std::cout << delim << "H_" << k;
		std::cout << delim << durationLB.at(k) << std::endl;
	}

	for (size_t k(0); k < upperBounds.size(); ++k)
	{
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << upperBounds.at(k);
		std::cout << delim << "UB_" << k;
		std::cout << delim << durationUB.at(k) << std::endl;

	}

	//exit(0);
}

void Solver::runDPUpperBoundClassic()
{
	char delim(';');
	uint16_t UBDP(_instance->DPUpperBound(0, {}));
	_instance->printCharacteristics(std::cout,delim);
	std::cout << " " << UBDP << " DP" << std::endl;
	
}
