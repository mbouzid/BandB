//
// Created by maria on 11/11/2022.
//


#include "upper_bound.h"
#include <sstream>
#include <lemon/smart_graph.h>
#include <map>
#include <lemon/network_simplex.h>
#include <algorithm>
core::Matrix UpperBound::DP(const Instance * instance, std::vector<uint16_t> & A, int16_t a, int16_t b)
{
    if (A.empty()) {
        return core::Matrix();
    }

    uint16_t n(A.size());


    std::stable_sort(A.begin(), A.end(),[instance](uint16_t x, uint16_t y){
        return instance->getD(x) <= instance->getD(y);
    });

    uint16_t i(A.at(n - 1));


    core::Matrix f;


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
    if (_sequence.empty()) {
        return 0;
    }
    else {
        if (_preemption) {
            double profit (0.0);
            for (uint16_t i: _sequence) {
                profit += (double) instance->getW(i) / (double) instance->getP(i);
            }
            return std::ceil(profit);
        }
        else
        {

            uint16_t profit(0);
            for (uint16_t i: _sequence) {
                profit += instance->getW(i);
            }
            return profit;
        }
    }
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

        case core::upperBound::name::HochbaumShamir:
        {
            upperBound = HochbaumShamirBound(instance, a, b, visited);
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

        if (pA > b)
        {
            break;
        }

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
        return new UpperBound({},false);

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


    return new UpperBound(Nl,false);
}

UpperBound * UpperBound::DPUpperBound(const Instance *instance, int16_t a, int16_t b, const std::set<uint16_t> &visited)
{
    if (visited.size() == instance->getN())
    {
        return new UpperBound({},false);
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
        return new UpperBound({},false);
    }


    core::Matrix f(DP(instance,A, a, b));

    std::vector<uint16_t> sequence(getSequenceFromDP(instance,f,A,a,b));

    return new UpperBound(sequence,false);
}

std::vector<uint16_t> UpperBound::getSequenceFromDP(const Instance * instance, const core::Matrix &f, std::vector<uint16_t> &A, int16_t a, int16_t b)
{

    std::vector<uint16_t> sequence;

    uint16_t first(*A.begin());



    if (f.at(first).at(a).getAccept()){
        sequence.push_back(first);
    }

    int16_t t(f.at(first).at(a).getTime());


    A.erase(A.begin());

    for (uint16_t j : A) {
        if (t >= std::min((uint16_t)b,instance->getDmax()))
            break;
        if (f.at(j).at(t).getAccept()) {
            sequence.push_back(j);
        }
        t = f.at(j).at(t).getTime();
    }



    return sequence;
}

UpperBound * UpperBound::HochbaumShamirBound(const Instance *instance, int16_t a, int16_t b, const std::set<uint16_t> & visited) {

    std::vector<uint16_t> A;

    for (uint16_t i(0); i < instance->getN(); ++i){
        if ((visited.find(i) == visited.cend()) and ( a <= instance->getD(i) - instance->getP(i) +1) and (instance->getEarliestCompletionTime(i,a) != -1) ){
            A.push_back(i);
        }
    }

    if (A.empty()){
        return new UpperBound({},true);
    }

    // order by descending weights
    std::stable_sort(A.begin(), A.end(),[instance]	(const uint16_t & x, const uint16_t & y){
        return instance->getW(x) >= instance->getW(y);
    });

    // supply and demand node
    std::map<uint16_t,uint16_t> supply;

    for(uint16_t i : A){
        supply.emplace(i,instance->getP(i));
    }


    std::map <int16_t, bool> X;

    for (uint16_t t(a); t <= b; ++t) {
        X.emplace(t,false);
    }

    std::vector<uint16_t> sequence;

    for (uint16_t i : A){
        for (uint16_t k(0); k < instance->getP(i); ++k) {
            for (int16_t t(a); t <= b; ++t) {
                if (instance->getE(i) <= instance->getEE(t) and not X[t] and t <= instance->getD(i) and supply.at(i) > 0) {
                    X[t] = true;
                    supply[i] -= 1;
                    sequence.push_back(i);
                }
            }
        }
    }


    return new UpperBound(sequence,true);
}


uint16_t UpperBound::MinCostFlowBound(const Instance *instance, int16_t a, int16_t b, const std::set<uint16_t> &visited) {


    lemon::SmartDigraph g;
    lemon::SmartDigraph::NodeMap<int> supply_demands(g);

    int P = 0;
    for (uint16_t i(0); i < instance->getN(); ++i){
        P += instance->getP(i);
    }

    int dmax = instance->getDmax();
    double round_factor(10E5);
    int balance_node_index (P + dmax);
    int sink_id (balance_node_index + 1);

    int nbNodes (P+dmax+1);
    lemon::SmartDigraph::Node nodes[nbNodes];
    std::map<int, lemon::SmartDigraph::Node> nodeIndexMap;

    // Create Arcs
    int nbArcs(P*(dmax-1));
    lemon::SmartDigraph::Arc arcs[nbArcs];
    lemon::SmartDigraph::ArcMap<int> costs(g);
    lemon::SmartDigraph::ArcMap<int> capacities(g);

    for (int i(0); i < sink_id+1; ++i) {
        nodes[i] = g.addNode();
        nodeIndexMap[i] = nodes[i];
    }

    // Create arcs for each p_i to each node representing a time unit and an additional node
    // to balance the network
    int k(0);
    int aux_t (0);
    for (int i(0); i < instance->getN(); ++i){
        for (int j(0); j < instance->getP(i); ++j) {
            int index_node = aux_t;
            for (int t(P); t < P + dmax; ++t) {
                if (instance->getE(i) <= instance->getEE(t - P) and instance->getD(i) >= t - P and t >= a and visited.find(i) == visited.end()){
                    // index_node -> t
                    // capacity = 1
                    // weight = w[i]/p[i]
                    arcs[k] = g.addArc(nodeIndexMap[index_node] , nodeIndexMap[t]);
                    costs[arcs[k]] = (-1) * int((((double)instance->getW(i) / (double)instance->getP(i)) * round_factor));
                    capacities[arcs[k]] = 1;
                    k+=1;
                }
            }
            // index_node -> balance_node_index
            // weight = 0
            // capacity = 1
            arcs[k] = g.addArc(nodeIndexMap[index_node] , nodeIndexMap[balance_node_index]);
            costs[arcs[k]] = 0;
            capacities[arcs[k]] = 1;
            aux_t += 1;
            k+=1;
        }

    }


    //Create arcs from each node representing a time unit to the sink
    // t -> sink_id
    // capacity = 1
    // weight = 0
    for (int t(P); t < P+dmax; ++t){
        arcs[k] = g.addArc(nodeIndexMap[t] , nodeIndexMap[sink_id]);
        costs[arcs[k]] = 0;
        capacities[arcs[k]] = 1;
        k+=1;
    }
    //Create the arc from the balancing node to the sink
    // balance_node_index -> sink_id
    // capacity = P
    // weight = 0
    arcs[k] = g.addArc(nodeIndexMap[balance_node_index] , nodeIndexMap[sink_id]);
    costs[arcs[k]] = 0;
    capacities[arcs[k]] = P;
    k+=1;


    //Add the demands for all nodes. 1 for p_i nodes, and -P for the sink
    for (int i(0); i < sink_id+1; ++i){
        int demand_node;
        if (i < P){
            demand_node = 1;
        }
        else if (i < sink_id){
            demand_node = 0;
        }
        else{
            demand_node = (-1)*P;
        }
        supply_demands[nodes[i]] = demand_node;
    }


    lemon::NetworkSimplex<lemon::SmartDigraph, int, int> solver(g);
    solver.upperMap(capacities).costMap(costs).supplyMap(supply_demands);

    lemon::SmartDigraph::ArcMap<int> flows(g);

    lemon::NetworkSimplex<lemon::SmartDigraph, int, int>::ProblemType status = solver.run();

    uint16_t Objective(0);
    switch (status) {

        case lemon::NetworkSimplex<lemon::SmartDigraph, int, int>::INFEASIBLE: {
            std::cerr << "insufficient flow" << std::endl;
            break;
        }

        case lemon::NetworkSimplex<lemon::SmartDigraph, int, int>::OPTIMAL: {
            solver.flowMap(flows);

            Objective = (-1)*std::floor((double)solver.totalCost()/(double)round_factor);
            break;
        }

        case lemon::NetworkSimplex<lemon::SmartDigraph, int, int>::UNBOUNDED: {
            std::cerr << "infinite flow" << std::endl;
            break;
        }

        default:{
            break;
        }
    }

    return Objective;
}



