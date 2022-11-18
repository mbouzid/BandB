//
// Created by maria on 11/11/2022.
//

#ifndef BANDB_UPPER_BOUND_H
#define BANDB_UPPER_BOUND_H

#include "generator/instance.h"

class UpperBound
{
    private:

        std::vector<uint16_t> _sequence;
        bool _preemption;

        UpperBound(const std::vector<uint16_t> & sequence, bool preemption):
            _sequence(sequence),
            _preemption(preemption)
            {};

    protected:

        static UpperBound * MooreUpperBound(const Instance * instance, int16_t a, int16_t b, const std::set<uint16_t> & visited);
        static UpperBound * DPUpperBound(const Instance * instance, int16_t a, int16_t b, const std::set<uint16_t> & visited);
        static UpperBound * HochbaumShamirBound(const Instance * instance, int16_t a, int16_t b, const std::set<uint16_t> & visited);


    static core::Matrix DP(const Instance * instance, std::vector<uint16_t> & A, int16_t a, int16_t b);


    public:

        UpperBound(const UpperBound & ub):
        _sequence(ub._sequence)
        {}

        static UpperBound * UpperBounds(const Instance * instance, core::upperBound::name ubName, int16_t a, int16_t b, const std::set<uint16_t> & visited);

        static std::vector<uint16_t> getSequenceFromDP(const Instance * instance, const core::Matrix & f, std::vector<uint16_t> & A, int16_t a, int16_t b);
        const std::vector<uint16_t> & getSequence() const;
        uint16_t getProfit(const Instance * instance) const;
        uint16_t getTotalImpact(const Instance * instance) const;
        static uint16_t MinCostFlowBound(const Instance * instance, int16_t a, int16_t b, const std::set<uint16_t> & visited);

};

#endif //BANDB_UPPER_BOUND_H
