//
// Created by maria on 11/11/2022.
//

#ifndef BANDB_SOLUTION_H
#define BANDB_SOLUTION_H

#include "generator/instance.h"
#include "upper_bound.h"

/*
 * A solution is a vector of jobs id
 */
class Solution
{

    private:

        std::vector <uint16_t> _sequence;


    protected:


        static Solution * Heuristics1(core::heuristic::ratio hRatio, const Instance * instance);
        static Solution * Heuristics2(core::heuristic::ratio hRatio, const Instance * instance);
        static Solution * Heuristic3(const Instance * instance);
        static Solution * Heuristic4(const Instance * instance);

        // return : int
        static int insertBest(int16_t & t, uint16_t & profit, core::heuristic::ratio hRatio, const std::vector<uint16_t> & orders, const Instance * instance);
        static void switchRatio(int16_t & t, std::map<uint16_t, double> & ratio, const std::vector <uint16_t> & orders, core::heuristic::ratio hRatio, const Instance * instance);
        static void computeRatioInsertion(std::map<uint16_t, double>& ratio, const std::vector <uint16_t>& orders, int16_t t, const std::function<double (int16_t, uint16_t ,uint16_t)> & computation, const Instance * instance);

    public:

        Solution(const std::vector<uint16_t> & sequence):
        _sequence(sequence){};

        Solution(const Solution & solution):
            _sequence(solution._sequence)
        {}

        std::vector<uint16_t> getSequence() const;
        uint16_t getProfit(const Instance * instance) const;
        // solution factory
        static Solution * Heuristic(const Instance * instance, core::heuristic::name hName, core::heuristic::ratio hRatio);

        ~Solution()
        {
            _sequence.clear();
        }
};

std::ostream & operator << (std::ostream & os, const Solution & solution);

#endif //BANDB_SOLUTION_H
