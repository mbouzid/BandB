#include "solver.h"
#include <set>
#include <iostream>
#include <fstream>


#include <chrono>
#include <ctime>

void Solver::run()
{

	uint16_t initialLowerBound, initialUpperBound;

	runHeuristics(initialLowerBound, initialUpperBound);

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

        int K (A.size());
		#pragma omp parallel for shared(lowerBound,_queue), num_threads(8)
		for (int k=0; k < K; ++k){
            //std::cout << "thread #" << omp_get_thread_num() << std::endl;
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

			


			if (incumbentProfit + upperBound <= lowerBound){
				continue;
			}



			UpperBound * UB1(UpperBound::UpperBounds(_instance,core::upperBound::DP,t,_instance->getT(),visited));
			ub1 = UB1->getProfit(_instance);
            delete UB1;


            uint16_t ub2 = UpperBound::MinCostFlowBound(_instance, t, _instance->getT(), visited);
            //std::cout << "ub1=" << ub1 << ", ub2=" << ub2 << std::endl;
             upperBound = std::min(std::min(ub1,ub2), upperBound);



			if (incumbentProfit > lowerBound)
			{
				lowerBound = incumbentProfit;
				#pragma omp critical
				{
                    _bestSolution = nullptr;
                    _bestSolution = new Solution(sequence);
                //    std::cout << "new solution found! Objective:" << lowerBound << std::endl;

                }
			}
				

			
			#pragma omp critical
			{
				if (upperBound + incumbentProfit > lowerBound)
				{
                   // std::cout << "node!" << std::endl;
					_queue.push(Node(j, u.getLevel() + 1, incumbentProfit, upperBound, t, visited, sequence));
				}
			}
			
		}

	}	   


}

void Solver::runHeuristics(uint16_t& bestInitialLowerBound, uint16_t& bestInitialUpperBound)
{
    int nbHeuristics (12);
	auto start = std::chrono::system_clock::now();
	std::vector<double> durationLB(nbHeuristics,0);

	std::vector < Solution * > heuristics ={
		Solution::Heuristic(_instance,core::heuristic::name::INSERT, core::heuristic::ratio::RATIO_A),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT, core::heuristic::ratio::RATIO_B),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT, core::heuristic::ratio::RATIO_C),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT_DP, core::heuristic::ratio::RATIO_A),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT_DP, core::heuristic::ratio::RATIO_B),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT_DP, core::heuristic::ratio::RATIO_C),
        Solution::Heuristic(_instance,core::heuristic::name::INSERT_INTV, core::heuristic::ratio::NO_RATIO),
        Solution::Heuristic(_instance,core::heuristic::name::DPH, core::heuristic::ratio::NO_RATIO),
      // H_4
        Solution::Heuristic(_instance,core::heuristic::name::H4Variant, core::heuristic::ratio::RATIO_A),
        Solution::Heuristic(_instance,core::heuristic::name::H4Variant, core::heuristic::ratio::RATIO_B),
        Solution::Heuristic(_instance,core::heuristic::name::H4Variant, core::heuristic::ratio::RATIO_C),
        Solution::Heuristic(_instance,core::heuristic::name::H4Variant, core::heuristic::ratio::NO_RATIO)
	};


    Solution * lb = *std::max_element(heuristics.begin(), heuristics.end(),
		[this](const Solution * x, const Solution * y) { return x->getProfit(_instance) <= y->getProfit(_instance); });
    _bestSolution = new Solution(*lb);

	bestInitialLowerBound = _bestSolution->getProfit(_instance);

    //UpperBound * UBHS = UpperBound::UpperBounds(_instance,core::upperBound::name::HochbaumShamir,0,_instance->getT(),{});

    //std::cout << "UBHS=" << UBHS->getProfit(_instance) << std::endl;

    //delete UBHS;
    /*int nbUB(3);
	std::vector <double> durationUB(nbUB,0);
	std::vector < UpperBound * > upperBounds =
	{
		UpperBound::UpperBounds(_instance,core::upperBound::name::Moore,0,_instance->getT(),{}),
        UpperBound::UpperBounds(_instance,core::upperBound::name::DP,0,_instance->getT(),{}),
        UpperBound::UpperBounds(_instance,core::upperBound::name::HochbaumShamir,0,_instance->getT(),{})
	};

	UpperBound * bestSequenceUB;
	bestSequenceUB = *std::max_element(upperBounds.begin(), upperBounds.end(),
		[this](const UpperBound * x, const UpperBound * y) { return x->getProfit(_instance) >= y->getProfit(_instance); });*/

    uint16_t UBx = UpperBound::MinCostFlowBound(_instance, 0, _instance->getT(), {});

    bestInitialUpperBound = UBx;

	char delim(';');

	std::vector<std::string> hNames = { "H_11","H_12","H_13","H_21","H_22","H_23","H_3","H_4","H_41","H_42","H_43","H_44"};

	/* OUTPUT */
	for (size_t k(0); k < heuristics.size(); ++k)
	{
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << heuristics.at(k)->getProfit(_instance);
		std::cout << delim << heuristics.at(k)->getTotalImpact(_instance);
		std::cout << delim << hNames.at(k);
		std::cout << delim << durationLB.at(k) << std::endl;
	}

	/*for (size_t k(0); k < upperBounds.size(); ++k)
	{*/
		_instance->printCharacteristics(std::cout, delim);
		std::cout << delim << UBx;
		std::cout << delim << 0;
		std::cout << delim << "UB_" << 0;
		std::cout << delim << 0.0 << std::endl;

	//}

}


void * runWrapper(void * args)
{
    t_arg * castArgs = (t_arg*)args;
    castArgs->_solver->run();
    castArgs->_hasResult = 1;
    pthread_exit(nullptr);
    return reinterpret_cast<void *>(1);
}
