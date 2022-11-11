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

   // check if upperbound is solution

	if (lowerBound == initialUpperBound)
	{
		_queue.pop();
	}


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

		#pragma omp parallel for shared(lowerBound,_queue), num_threads(1)
		for (int k=0; k < A.size(); ++k)
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

			
			ub1 = _instance->computeProfit(_instance->DPUpperBound(t, visited));


			upperBound = std::min(ub1, upperBound);



			if (incumbentProfit > lowerBound)
			{
				lowerBound = incumbentProfit;
				#pragma omp critical
				{
					bestSequence = sequence;
				}
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
	std::cout << delim << _instance->getTotalImpact(bestSequence);
	std::cout << delim << "BandB" ;

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
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_A, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_B, start, durationLB),
		_instance->Heuristic(core::heuristic_name::INSERT_DP, core::heuristic_ratio::RATIO_C, start, durationLB), 		
		_instance->Heuristic(core::heuristic_name::INSERT_INTV, core::heuristic_ratio::NO_RATIO, start, durationLB),
		_instance->Heuristic(core::heuristic_name::DPH, core::heuristic_ratio::NO_RATIO, start, durationLB)
	};

	bestSequence = *std::max_element(heuristics.begin(), heuristics.end(),
		[this](const std::vector<uint16_t>& x, const std::vector<uint16_t>& y) { return _instance->computeProfit(x) <= _instance->computeProfit(y); });

	bestInitialLowerBound = _instance->computeProfit(bestSequence);


	std::vector <double> durationUB;
	std::vector < std::vector<uint16_t> > upperBounds =
	{
		_instance->UpperBound(core::upperBound_name::Moore,0,{},omp_get_thread_num(), start, durationUB),
		_instance->UpperBound(core::upperBound_name::DP,0,{},omp_get_thread_num(), start, durationUB)
	};

	std::vector<uint16_t> bestSequenceUB;
	bestSequenceUB = *std::max_element(upperBounds.begin(), upperBounds.end(),
		[this](const std::vector<uint16_t> x, const std::vector<uint16_t> y) { return _instance->computeProfit(x) >= _instance->computeProfit(y); });


	bestInitialUpperBound = _instance->computeProfit(bestSequenceUB);

	char delim(';');


	std::vector<std::string> hNames = { "H_11","H_12","H_13","H_21","H_22","H_23","H_3","H_4" };

	/* OUTPUT */
	for (size_t k(0); k < heuristics.size(); ++k)
	{
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << _instance->computeProfit(heuristics.at(k));
		std::cout << delim << _instance->getTotalImpact(heuristics.at(k));
		std::cout << delim << hNames.at(k);
		std::cout << delim << durationLB.at(k) << std::endl;
	}

	for (size_t k(0); k < upperBounds.size(); ++k)
	{
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << _instance->computeProfit(upperBounds.at(k));
		std::cout << delim << _instance->getTotalImpact(upperBounds.at(k));
		std::cout << delim << "UB_" << k;
		std::cout << delim << durationUB.at(k) << std::endl;

	}

	//exit(0);
}

void Solver::runDPUpperBoundClassic()
{
	char delim(';');
	std::vector<uint16_t> UBDP(_instance->DPUpperBound(0, {}));
	_instance->printCharacteristics(std::cout,delim);
	std::cout << " " << _instance->computeProfit(UBDP) << " DP" << std::endl;
	
}
