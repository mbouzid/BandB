//
// Created by maria on 11/11/2022.
//
#include "solution.h"

Solution * Solution::Heuristic(const Instance * instance, core::heuristic::name hName, core::heuristic::ratio hRatio)
{
    Solution * solution = nullptr;
    switch (hName)
    {
        case core::heuristic::name::INSERT:
        {
            solution = Heuristics1(hRatio,instance);
            break;
        }

        case core::heuristic::name::INSERT_DP:
        {
            solution = Heuristics2(hRatio,instance);
            break;
        }

        case core::heuristic::name::INSERT_INTV:
        {
            solution = Heuristic3(instance);
            break;
        }

        case core::heuristic::name::DPH:
        {
            solution = Heuristic4(instance);
            break;
        }

        case core::heuristic::name::H4Variant:
        {
            solution = Heuristic4Variant(hRatio,instance);
            break;
        }

        default:
        {
            break;
        }

    }

    return solution;
}

Solution * Solution::Heuristics1(core::heuristic::ratio hRatio, const Instance * instance)
{
    // starting time
    int16_t t(0);
    // ending horizon
    int16_t T(std::min(instance->getT(),instance->getDmax()));
    // initial profit
    uint16_t profit(0);
    // vector of candidates
    std::vector <uint16_t> candidates;
    // vector of orders
    std::vector<uint16_t> sequence;

    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        candidates.push_back(i);
    }


    while (t <= T)
    {
        int res = insertBest(t, profit, hRatio,candidates, instance);

        if (res == -1)
        {
            break;
        }
        else
        {
            std::vector<uint16_t>::iterator found = std::find(candidates.begin(),candidates.end(),res);
            candidates.erase(found);
            sequence.push_back(res);
        }
    }

    return new Solution(sequence);
}

Solution * Solution::Heuristics2(core::heuristic::ratio hRatio, const Instance *instance)
{
    int16_t t(0);
    // candidates orders
    std::vector<uint16_t> orders;

    std::vector<uint16_t> sequence;

    std::map<uint16_t, double> ratio;

    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        auto found = std::find(sequence.begin(), sequence.end(), i);
        if (found == sequence.end())
        {
            orders.push_back(i);
            ratio.emplace(i, std::numeric_limits<double>::infinity());
        }
    }


    while (t <= instance->getDmax())
    {

        std::vector<uint16_t> L(orders);


        for (uint16_t i : L)
        {
            std::set<uint16_t> A;

            for (uint16_t j : sequence)
            {
                A.insert(j);
            }

            A.insert(i);

            if (instance->getEarliestCompletionTime(i,t)== -1)
            {
                ratio[i] = std::numeric_limits<double>::infinity();
            }
            else
            {
                int16_t C = instance->getEarliestCompletionTime(i,t);
                UpperBound * UB(UpperBound::UpperBounds(instance,core::upperBound::name::DP,C,instance->getT(),A));
                double expectProfit = (double)UB->getProfit(instance);
                delete UB;

                switch (hRatio)
                {
                    case core::heuristic::RATIO_A:
                    {

                        // (C[i][t]/w[i]) + (C[i][t]/UB2(L,C[i][t],T))
                        double RA = core::heuristic::ratioA(C,instance->getP(i),instance->getW(i));
                        if (expectProfit == 0) {
                            ratio[i] = RA;
                        }
                        else
                        {
                            ratio[i] = RA + (double)C/(double)expectProfit;
                        }

                        break;
                    }

                    case core::heuristic::RATIO_B:
                    {
                        double RB = core::heuristic::ratioB(C,instance->getP(i),instance->getW(i));
                        if (expectProfit == 0)
                        {
                            ratio[i] = RB ;
                        }
                        else
                        {
                            ratio[i] = RB + (double) C / (double) expectProfit;
                        }
                        break;
                    }

                    case core::heuristic::RATIO_C:
                    {
                        double RC = core::heuristic::ratioC(C,instance->getP(i),instance->getW(i));
                        if (expectProfit == 0) {
                            ratio[i] = RC;
                        }
                        else {
                            ratio[i] = RC + (double) C / (double) expectProfit;
                        }

                        break;
                    }

                        default:
                        {
                            break;
                        }


                }
                //switchRatio(t,ratio,orders,hRatio,instance);

            }

        }// fin for i

        for (uint16_t i : orders)
        {
            std::vector<uint16_t>::iterator found(std::find(L.begin(), L.end(), i));
            if ( ratio.at(i) == std::numeric_limits<double>::infinity())
            {
                L.erase(found);
            }
        }


        std::stable_sort(L.begin(),L.end(),[&ratio](uint16_t x, uint16_t y)
        {
            return ratio.at(x) <= ratio.at(y);
        });


        if (not L.empty())
        {
            uint16_t j(*L.begin());

            if (instance->getEarliestCompletionTime(j,t) != -1)
            {
                sequence.push_back(j);
                orders.erase(std::find(orders.begin(), orders.end(), j));
                t = instance->getEarliestCompletionTime(j,t);
            }
        }
        else
        {
            break;
        }
    }

    return new Solution(sequence);
}

Solution * Solution::Heuristic3(const Instance * instance)
{
    std::vector<uint16_t> L;

    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        L.push_back(i);
    }

    std::stable_sort(L.begin(),L.end(),[instance](uint16_t x, uint16_t y)
    {
        return instance->getD(x) <= instance->getD(y);
    });


    uint16_t pmax(instance->getPmax());
    uint16_t profit(0);


    uint16_t a(0), b(pmax - 1);
    std::vector<uint16_t> sequence;

    while (b <= instance->getDmax()+1)
    {

        uint16_t Emin(instance->energyMinimalInInterval(a, b));
        std::vector<uint16_t> A;

        // A = candidates jobs
        for (uint16_t i : L)
        {
            if (instance->getE(i) <= Emin and b <= instance->getD(i)+1)
            {
                A.push_back(i);
            }
        }

        if (not A.empty())
        {
            std::set<uint16_t> complementA(utils::getComplementFromVector(A,instance->getN()));
            UpperBound * UB = UpperBound::UpperBounds(instance,core::upperBound::DP,a,b,complementA);
            uint16_t expectedProfit = UB->getProfit(instance);
            std::vector<uint16_t> subseq(UB->getSequence());

            delete UB;
            if (not subseq.empty())
            {

                for (uint16_t j : subseq)
                {
                    sequence.push_back(j);

                    std::vector<uint16_t>::iterator toRemove(std::find(L.begin(), L.end(), j));

                    L.erase(toRemove);
                }
                profit += expectedProfit;
                a += pmax;
                b += pmax;

            }
            else
            {
                a += 1;
                b += 1;

            }
        }
        else
        {
            a += 1;
            b += 1;
        }

    }


    return new Solution(sequence);
}

Solution * Solution::Heuristic4(const Instance *instance)
{
    std::vector<uint16_t> sequence;
    std::vector<uint16_t> orders;

    std::vector<uint16_t> R;

    // remove non processable orders
    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        if (instance->getEarliestCompletionTime(i,0)== -1)
        {
            R.push_back(i);
        }

    }

    // candidates jobs
    std::vector<uint16_t> A;
    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        std::vector<uint16_t>::const_iterator it(std::find(R.cbegin(), R.cend(), i));
        if (it == R.cend())
        {
            A.push_back(i);
        }
    }

    if (A.empty())
    {
        return new Solution(sequence);
    }

    std::map<uint16_t, uint16_t> d;
    // get deadline
    for (uint16_t i : A)
    {
        for (int16_t t(0); t <= instance->getT(); ++t)
        {
            if (instance->getEarliestCompletionTime(i,t + 1) == -1)
            {
                d.emplace(i, instance->getEarliestCompletionTime(i,t) - instance->getP(i));
            }
        }
    }




    // sort by deadline
    std::stable_sort(A.begin(), A.end(), [&d](uint16_t x, uint16_t y) { return d[x] <= d[y]; });

    int16_t T(instance->getT());

    uint16_t n(A.size());
    uint16_t i(A.at(n-1));

    core::Matrix f;

    for (int16_t t(0); t <= T; ++t)
    {
        f.emplace(i, std::map<uint16_t, core::tupleDP>());
        int16_t tt(instance->getEarliestCompletionTime(i,t));
        if ( tt == -1)
        {
            f[i].emplace(t, core::tupleDP(instance->getW(i),false,t));
        }
        else
        {
            f[i].emplace(t, core::tupleDP(0, true, tt));
        }
    }


    std::vector<uint16_t> B(A);
    std::stable_sort(B.begin(), B.end(), [&d](uint16_t x, uint16_t y) {return d[x] >= d[y]; });

    B.erase(B.begin());

    for (uint16_t j : B)
    {
        f.emplace(j, std::map<uint16_t, core::tupleDP>());
        for (int16_t t(0); t <= T; ++t)
        {
            f[j].emplace(t, core::tupleDP(0, false, t));
        }
    }

    for (uint16_t j : B)
    {
        for (int16_t t(0); t <= T; ++t)
        {

            int16_t tt (instance->getEarliestCompletionTime(j,t));
            if (tt == -1)
            {
                f[j][t].setProfit(f.at(i).at(t).getProfit() + instance->getW(j));
            }
            else
            {
                if (f.at(i).at(t).getProfit() + instance->getW(j) <= f.at(i).at(tt).getProfit())
                {
                    f[j][t].setProfit(f.at(i).at(t).getProfit() + instance->getW(j));
                }
                else
                {
                    f[j][t].setAccept (true);
                    f[j][t].setProfit(f.at(i).at(tt).getProfit());
                    f[j][t].setTime(tt);

                }

            }
        }
        i = j;
    }

    return new Solution(UpperBound::getSequenceFromDP(instance,f,A,0,instance->getT()));

}

Solution *Solution::Heuristic4Variant(core::heuristic::ratio hRatio, const Instance * instance) {

    std::function<uint16_t (uint16_t, uint16_t )> pred = [](uint16_t a, uint16_t b) {return 1;};

    switch(hRatio){
        case core::heuristic::RATIO_A:{
            pred = [instance](uint16_t i, uint16_t t) {return instance->getEarliestCompletionTime(i,t)+instance->getD(i);};
            break;
        }
        case core::heuristic::RATIO_B:{
            pred = [instance](uint16_t i, uint16_t t) {return instance->getEarliestCompletionTime(i,0);};
            break;
        }
        case core::heuristic::RATIO_C:{
            pred = [instance](uint16_t i, uint16_t t) {return instance->getEarliestCompletionTime(i,t);};
            break;
        }

        case core::heuristic::NO_RATIO:{
            pred = [instance](uint16_t i, uint16_t t) {return instance->getD(i);};
            break;
        }
    }


    std::vector<uint16_t> sequence;
    std::vector<uint16_t> orders;

    std::vector<uint16_t> R;

    // remove non processable orders
    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        if (instance->getEarliestCompletionTime(i,0)== -1)
        {
            R.push_back(i);
        }

    }

    // candidates jobs
    std::vector<uint16_t> A;
    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        std::vector<uint16_t>::const_iterator it(std::find(R.cbegin(), R.cend(), i));
        if (it == R.cend())
        {
            A.push_back(i);
        }
    }

    if (A.empty())
    {
        return new Solution(sequence);
    }

    std::map<uint16_t, uint16_t> d;
    // get deadline
    for (uint16_t i : A)
    {
        for (int16_t t(0); t <= instance->getT(); ++t)
        {
            if (instance->getEarliestCompletionTime(i,t + 1) == -1)
            {
                d.emplace(i,pred(i,t));
            }
        }
    }




    // sort by deadline
    std::stable_sort(A.begin(), A.end(), [&d](uint16_t x, uint16_t y) { return d[x] <= d[y]; });

    int16_t T(instance->getT());

    uint16_t n(A.size());
    uint16_t i(A.at(n-1));

    core::Matrix f;

    for (int16_t t(0); t <= T; ++t)
    {
        f.emplace(i, std::map<uint16_t, core::tupleDP>());
        int16_t tt(instance->getEarliestCompletionTime(i,t));
        if ( tt == -1)
        {
            f[i].emplace(t, core::tupleDP(instance->getW(i),false,t));
        }
        else
        {
            f[i].emplace(t, core::tupleDP(0, true, tt));
        }
    }


    std::vector<uint16_t> B(A);
    std::stable_sort(B.begin(), B.end(), [&d](uint16_t x, uint16_t y) {return d[x] >= d[y]; });

    B.erase(B.begin());

    for (uint16_t j : B)
    {
        f.emplace(j, std::map<uint16_t, core::tupleDP>());
        for (int16_t t(0); t <= T; ++t)
        {
            f[j].emplace(t, core::tupleDP(0, false, t));
        }
    }

    for (uint16_t j : B)
    {
        for (int16_t t(0); t <= T; ++t)
        {

            int16_t tt (instance->getEarliestCompletionTime(j,t));
            if (tt == -1)
            {
                f[j][t].setProfit(f.at(i).at(t).getProfit() + instance->getW(j));
            }
            else
            {
                if (f.at(i).at(t).getProfit() + instance->getW(j) <= f.at(i).at(tt).getProfit())
                {
                    f[j][t].setProfit(f.at(i).at(t).getProfit() + instance->getW(j));
                }
                else
                {
                    f[j][t].setAccept (true);
                    f[j][t].setProfit(f.at(i).at(tt).getProfit());
                    f[j][t].setTime(tt);

                }

            }
        }
        i = j;
    }

    return new Solution(UpperBound::getSequenceFromDP(instance,f,A,0,instance->getT()));

}

int Solution::insertBest(int16_t & t, uint16_t & profit, core::heuristic::ratio hRatio, const std::vector<uint16_t> & orders, const Instance * instance)
{
    int insertedOrder (-1);
    std::map<uint16_t, double> ratio;

    switchRatio(t,ratio,orders,hRatio,instance);

    std::vector<uint16_t> L;
    L.insert(L.begin(), orders.begin(), orders.end());

    for (uint16_t i : orders)
    {
        std::vector<uint16_t>::iterator found(std::find(L.begin(), L.end(), i));
        if (ratio.at(i) == std::numeric_limits<double>::infinity() and found != L.end())
        {
            L.erase(found);
        }
    }

    if (not L.empty())
    {

        std::stable_sort(L.begin(),L.end(),[&ratio](const uint16_t & x, const uint16_t &  y){
            return ratio.at(x) <= ratio.at(y);
        });

        uint16_t j(*L.begin());

        if (instance->getEarliestCompletionTime(j,t)  != -1)
        {
            t = instance->getEarliestCompletionTime(j,t);
            profit += instance->getW(j);

            insertedOrder = j;
        }
    }
    else
    {
        return insertedOrder;
    }


    return insertedOrder;
}

void Solution::switchRatio(int16_t & t, std::map<uint16_t, double> & ratio, const std::vector <uint16_t> & orders, core::heuristic::ratio hRatio, const Instance * instance)
{
    switch (hRatio)
    {
        case core::heuristic::ratio::RATIO_A:
        {
            computeRatioInsertion(ratio, orders, t, core::heuristic::ratioA,instance);
            break;
        }

        case core::heuristic::ratio::RATIO_B:
        {
            computeRatioInsertion(ratio, orders, t, core::heuristic::ratioB,instance);
            break;
        }

        case core::heuristic::ratio::RATIO_C:
        {
            computeRatioInsertion(ratio, orders, t, core::heuristic::ratioC,instance);
            break;
        }


        default:
        {
            break;
        }
    }
}

void Solution::computeRatioInsertion(std::map<uint16_t, double>& ratio, const std::vector <uint16_t>& orders, int16_t t, const std::function<double (int16_t, uint16_t ,uint16_t)> & computation, const Instance * instance)
{
    for (uint16_t i : orders)
    {

        if (ratio.find(i) == ratio.end())
        {
            ratio.emplace(i, std::numeric_limits<double>::infinity());
        }

        if (instance->getEarliestCompletionTime(i,t) != -1)
        {
            ratio[i] = computation(instance->getEarliestCompletionTime(i,t),  instance->getP(i), instance->getW(i));
        }
    }

}

std::vector<uint16_t> Solution::getSequence() const
{
    return _sequence;
}

uint16_t Solution::getTotalImpact(const Instance * instance) const
{
    uint16_t impact(0);
    for (uint16_t j : _sequence)
    {
        impact += instance->getE(j)*instance->getP(j);
    }

    return impact;
}

uint16_t Solution::getProfit(const Instance * instance) const
{
    uint16_t profit(0);
    for (uint16_t i : _sequence)
    {
        profit += instance->getW(i);
    }

    return profit;
}



std::ostream &operator<<(std::ostream &os, const Solution &solution)
{
    for (uint16_t elt : solution.getSequence())
    {
        os << elt << " ";
    }
    return os;
}
