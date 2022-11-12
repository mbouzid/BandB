//
// Created by maria on 11/11/2022.
//

#include "upper_bound.h"

core::Matrix UpperBound::DP(const Instance * instance, std::vector<uint16_t> & A, int16_t a, int16_t b)
{
    if (A.empty())
        return core::Matrix();

    uint16_t n(A.size());


    std::stable_sort(A.begin(), A.end(),[instance](uint16_t x, uint16_t y){
        return instance->getD(x) <= instance->getD(y);
    });

    uint16_t i(A.at(n - 1));


    core::Matrix f;

    int16_t T(b);

    for (int16_t t(a); t <= b; ++t)
    {
        f.emplace(i, std::map<uint16_t, core::tupleDP>());
        int16_t tt(t+instance->getP(i));
        if (tt > instance->getD(i)+1 or tt > b)
        {
            // rejet = t
            f[i].emplace(t, core::tupleDP(instance->getW(i), false, t));
        }
        else
        {
            // accept = t+p[i]
            f[i].emplace(t, core::tupleDP(0, true, tt));
        }
    }


    std::vector<uint16_t> B(A);
    std::stable_sort(B.begin(), B.end(),[instance](uint16_t x, uint16_t y)
    {
        return instance->getD(x) >= instance->getD(y);
    });

    B.erase(B.begin());

    for (uint16_t j : B)
    {
        f.emplace(j, std::map<uint16_t, core::tupleDP>());
        for (int16_t t(a); t <= b; ++t)
        {
            f[j].emplace(t, core::tupleDP(0, false, t));
        }
    }

    for (uint16_t j : B)
    {
        for (int16_t t(a); t <= b; ++t)
        {
            int16_t tt(t + instance->getP(j));
            if ((tt > instance->getD(j)+1) or (tt >= b))
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
                    f[j][t].setAccept(true);
                    f[j][t].setProfit(f.at(i).at(tt).getProfit());
                    f[j][t].setTime(tt);
                }

            }
        }
        i = j;
    }


    return f;

}

const std::vector<uint16_t> & UpperBound::getSequence() const
{
    return _sequence;
}

uint16_t UpperBound::getTotalImpact(const Instance * instance) const
{
    uint16_t impact(0);
    for (uint16_t j : _sequence)
    {
        impact += instance->getE(j)*instance->getP(j);
    }

    return impact;
}

uint16_t UpperBound::getProfit(const Instance * instance) const
{
    uint16_t profit(0);
    if (_sequence.empty())
        return 0;
    for (uint16_t i : _sequence)
    {
        profit += instance->getW(i);
    }

    return profit;
}

UpperBound * UpperBound::UpperBounds(const Instance *instance, core::upperBound::name ubName, int16_t a, int16_t b, const std::set<uint16_t> &visited)
{
    UpperBound * upperBound = nullptr;

    switch (ubName)
    {

        case core::upperBound::name::Moore:
        {
            upperBound = MooreUpperBound(instance, a, b, visited);
            break;
        }

        case core::upperBound::name::DP:
        {
            upperBound = DPUpperBound(instance, a, b, visited);
            break;
        }

        default:
        {
            break;
        }

    }

    return upperBound;
}

UpperBound *UpperBound::MooreUpperBound(const Instance * instance, int16_t a, int16_t b, const std::set<uint16_t> &visited)
{
    // get list of candidate jobs
    std::vector<uint16_t> orders;

    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        if ((visited.find(i) == visited.cend()) and ( a <= instance->getD(i) - instance->getP(i) +1) and (instance->getEarliestCompletionTime(i,a) != -1) )
        {
            orders.push_back(i);
        }
    }

    // sort orders by EDD
    std::stable_sort(orders.begin(),orders.end(),[instance](uint16_t x, uint16_t y)
    {
        return instance->getD(x) <= instance->getD(y);
    });

    std::vector<uint16_t> A;
    int16_t pA(a);

    for (uint16_t i : orders)
    {

        if (pA + instance->getP(i) <= instance->getD(i)+1)
        {
            A.push_back(i);
            pA += instance->getP(i);
        }
        else
        {
            A.push_back(i);

            std::vector<uint16_t>::iterator longestAcceptedJob
            (
                    std::max_element(A.begin(),A.end(),[instance](uint16_t x, uint16_t y)
                    {
                        return (double)instance->getP(x)/(double)instance->getW(x) <= (double)instance->getP(y)/(double)instance->getW(y);
                    })
            );


            uint16_t k(*longestAcceptedJob);
            std::vector<uint16_t>::iterator toRemove(std::find(A.begin(), A.end(), k));

            pA = pA + instance->getP(i) - instance->getP(k);

            A.erase(toRemove);

        }
    }


    if (A.empty())
        return {};

    std::vector<uint16_t> N;
    for (uint16_t k(0); k < instance->getN(); ++k)
    {
        N.push_back(k);
    }


    // sort by increasing w
    std::stable_sort(N.begin(), N.end(),[instance]	(uint16_t x, uint16_t y)
    {
        return instance->getW(x) >= instance->getW(y);
    });

    std::vector<uint16_t> Nl;
    for (uint16_t l(0); l < A.size(); ++l)
    {
        Nl.push_back(N.at(l));
    }


    return new UpperBound(Nl);
}

UpperBound * UpperBound::DPUpperBound(const Instance *instance, int16_t a, int16_t b, const std::set<uint16_t> &visited)
{
    if (visited.size() == instance->getN())
    {
        return new UpperBound({});
    }


    std::vector<uint16_t> A;

    for (uint16_t i(0); i < instance->getN(); ++i)
    {
        if ((visited.find(i) == visited.cend()) and ( a <= instance->getD(i) - instance->getP(i) +1) and (instance->getEarliestCompletionTime(i,a) != -1) )
        {
            A.push_back(i);
        }
    }

    if (A.empty())
    {
        return new UpperBound({});
    }


    core::Matrix f(DP(instance,A, a, b));

    std::vector<uint16_t> sequence(getSequenceFromDP(instance,f,A,a,b));

    return new UpperBound(sequence);
}

std::vector<uint16_t> UpperBound::getSequenceFromDP(const Instance * instance, const core::Matrix &f, std::vector<uint16_t> &A, int16_t a, int16_t b)
{

    std::vector<uint16_t> sequence;

    uint16_t first(*A.begin());



    if (f.at(first).at(a).getAccept())
    {
        sequence.push_back(first);
    }

    int16_t t(f.at(first).at(a).getTime());

    if (t >= instance->getDmax())
        return sequence;

    A.erase(A.begin());

    for (uint16_t j : A)
    {
        //if (f.find(j) != f.end()) {
            if (f.at(j).at(t).getAccept()) {
                sequence.push_back(j);
            }
            t = f.at(j).at(t).getTime();
        }
        //else
        //{
        //    break;
        //}

    //}


    return sequence;
}

