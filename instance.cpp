#include "instance.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>

int16_t Instance::computeEarliestCompletionTime(uint16_t i, uint16_t t) const
{
	int16_t delta(-1);
				   
	if (t > _d[i])
		return delta;

	if (t > _d[i]-_p[i] +1)
		return delta;


	for (uint16_t tt(t); tt <= _d[i]-_p[i]+1; tt++)
	{
		bool energyConstraint(true);
		for (uint16_t ttt(tt); ttt < tt + _p[i]; ttt++)
		{
			if (_e[i] > _E[ttt])
			{
				energyConstraint = false;

				break;
			}

		}	  
		
		if (energyConstraint)
		{
			
			delta = tt + _p[i];
			return delta;
		}
	}

	return delta;
}

uint16_t Instance::energyMinimalInInterval(uint16_t a, uint16_t b) const
{
	return *std::min_element(_E+a,_E+b);
}

std::vector<uint16_t> Instance::getSeqFromDP(const core::Matrix& f,  std::vector<uint16_t> & A, int16_t a, int16_t b) const
{
	std::vector<uint16_t> sequence;

	uint16_t first(*A.begin());


	if (f.at(first).at(a).getAccept())
	{
		sequence.push_back(first);
	}

	int16_t t(f.at(first).at(a).getTime());

	A.erase(A.begin());

	for (uint16_t j : A)
	{
        if (f.find(j) != f.end())
        {
            if (f.at(j).at(t).getAccept()) {
                sequence.push_back(j);
            }

            t = f.at(j).at(t).getTime();
        }
	}


	return sequence;
}

uint16_t Instance::checkProfit(int16_t tinit, const std::set<uint16_t>& visited) const
{

	std::vector<uint16_t> A;

	for (uint16_t i(0); i < _n; ++i)
	{
		if ((visited.find(i) == visited.cend()) and (tinit <= _d[i] - _p[i] + 1) and (_earliestCompletionTime[i][tinit] != -1))
		{
			A.push_back(i);
		}
	}


	// sort by EDD
	std::sort
	(
		A.begin(), A.end(),
		[this](uint16_t i, uint16_t j)
		{
			return _d[i] <= _d[j];
		}
	);


	uint16_t profit(0);

	int16_t t(tinit);
	while (not A.empty())
	{
		uint16_t j(*A.begin());

		if (_earliestCompletionTime[j][t] != -1)
		{

			t = _earliestCompletionTime[j][t];
			profit += _w[j];
		}
		
		A.erase(A.begin());
	}


	return profit;
}

core::Matrix Instance::DP(std::vector<uint16_t>& A, uint16_t a, uint16_t b) const
{
	if (A.empty())
		return core::Matrix();

	uint16_t n(A.size());

	
	std::sort(A.begin(), A.end(), [this](uint16_t x, uint16_t y) {return _d[x] <= _d[y]; });
	uint16_t i(A.at(n - 1));


	core::Matrix f;

	int16_t T(b);

	for (int16_t t(a); t <= b; ++t)
	{
		f.emplace(i, std::map<uint16_t, core::tupleDP>());
		int16_t tt(t+_p[i]);
		if (tt > _d[i]+1 or tt > b)
		{
			// rejet = t
			f[i].emplace(t, core::tupleDP(_w[i], false, t));
		}
		else
		{
			// accept = t+p[i]
			f[i].emplace(t, core::tupleDP(0, true, tt));
		}
	}


	std::vector<uint16_t> B(A);
	std::sort(B.begin(), B.end(), [this](uint16_t x, uint16_t y) {return _d[x] >= _d[y]; });

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
			int16_t tt(t + _p[j]);
			if ((tt > _d[j]+1) or (tt >= b))
			{
				f[j][t].setProfit(f.at(i).at(t).getProfit() + _w[j]);
			}
			else
			{
				if (f.at(i).at(t).getProfit() + _w[j] <= f.at(i).at(tt).getProfit())
				{
					f[j][t].setProfit(f.at(i).at(t).getProfit() + _w[j]);
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

core::Matrix Instance::DP1(std::vector<uint16_t>& A, uint16_t a, uint16_t b) const
{
	if (A.empty())
		return core::Matrix();

	// sort by LDD
	std::sort
	(
		A.begin(), A.end(),
		[this](uint16_t i, uint16_t j)
		{
			return _d[i] > _d[j];
		}
	);

	uint16_t i(*A.begin());
	core::Matrix f;

	uint16_t k(0);
	for (size_t ii(0); ii < A.size(); ++ii)
	{
		uint16_t j(A.at(ii));
		
		if (f.find(j) == f.end())
		{
			f.emplace(j, std::map <uint16_t, core::tupleDP>());
		}

		if (j == i)
		{
			uint16_t T(0);


			std::vector<uint16_t> sub(A.begin()+(k+1), A.end());
			for (uint16_t x : sub)
			{
				T += _p[x];
			}
			T += 1;
			
			T = std::min(b, T);

			for (uint16_t t(0); t <= T; ++t)
			{
				if (a + t  + _p[j] <= std::min(b,(uint16_t)(_d[j] + 1)))
				{
					f[j].emplace(t, core::tupleDP(_w[j],false,a+t+_p[j]));
				}
				else
				{	
					f[j].emplace(t, core::tupleDP(0,true,t+_p[j]));
				}
			}
		}
		else
		{

			uint16_t T(0);

			for (uint16_t x : A)
			{
				T += _p[x];
			}
			T += 1;
			T = std::min(b, T);

			for (uint16_t t(0); t <= T; ++t)
			{
				f[j].emplace(t, core::tupleDP(0,false,t));
			}
		}


	}



	k = 1;
	uint16_t jprev(i);

	for (size_t ii(1); ii < A.size(); ++ii)
	{
		uint16_t j(A.at(ii));

		uint16_t T(0);
		std::vector<uint16_t> sub(A.begin() + (k + 1), A.end());
		for (uint16_t x : sub)
		{
			T += _p[x];
		}
		T += 1;

		T = std::min(T, b);
		for (uint16_t t(0); t <= T; ++t)
		{
			core::tupleDP PHI1;
			core::tupleDP PHI2;

			uint16_t tt(std::min(T,(uint16_t)(t + _p[j])));

			if (a + t + _p[j]  <= std::min(b, (uint16_t)(_d[j] + 1)))
			{
				PHI1 = core::tupleDP(_w[j] + f.at(jprev).at(tt).getProfit(),false,a+t);
			}
			else
			{
				PHI1 = core::tupleDP(f.at(jprev).at(tt).getProfit(),true,a+t+_p[j]);
			}

			uint16_t sum2(0);
			for (uint16_t l(0); l < k + 1; ++l)
			{
				sum2 += _p[A.at(l)];
			}
			sum2 += 1;


			if (a + sum2 + t <= std::min(b, (uint16_t)(_d[j] + 1)))
			{
				PHI2 = core::tupleDP(f.at(jprev).at(t).getProfit() + _w[j],true,a+sum2+t);
			}
			else
			{

				PHI2 = f.at(jprev).at(t);
			}

			if (PHI1.getProfit() < PHI2.getProfit())
			{
				f[j][t] = PHI2;
			}
			else
			{
				f[j][t] = PHI1;
			}
		}


		k++;
		jprev = j;

	}

	return f;
}

int Instance::mainloop_insertionHeuristic(int16_t & t, std::vector <uint16_t> & orders, std::vector <uint16_t>& seq, uint16_t & profit, core::heuristic_ratio HeuristicRatio) const
{
	std::map<uint16_t, double> ratio;

	switch (HeuristicRatio)
    {
		case core::heuristic_ratio::RATIO_A:
        {

			std::function<double (int16_t, uint16_t, uint16_t)> computation = [&](int16_t C, uint16_t p, uint16_t w)
			{ return (double)C / (double)w; };
			computeRatioInsertion(ratio, orders, t, computation);

			break;
		}

		case core::heuristic_ratio::RATIO_B:
		{

            std::function<double(int16_t, uint16_t, uint16_t)> computation = [&](int16_t C, uint16_t p, uint16_t w)
			{ return ((double)C - (double)p) / (double)w; };
			computeRatioInsertion(ratio, orders, t, computation);


			break;
		}

		case core::heuristic_ratio::RATIO_C:
		{

            std::function<double(int16_t, uint16_t, uint16_t)> computation = [&](int16_t C, uint16_t p, uint16_t w)
			{ return ((double)C / (double)w) + (((double)C - (double)p) / (double)w); };
			computeRatioInsertion(ratio, orders, t, computation);

			break;
		}


		default:
		{
			break;
		}
	}

    std::vector<uint16_t> X;
	//std::vector<uint16_t> L;
    //L.clear();
    X.insert(X.begin(), orders.begin(), orders.end());

    for (uint16_t i : orders)
    {
        std::vector<uint16_t>::iterator found(std::find(X.begin(), X.end(), i));
        if (ratio.at(i) == std::numeric_limits<double>::infinity() and found != X.end())
        {
            X.erase(found);
        }
    }


	if (not X.empty())
	{

        std::sort
        (
                X.begin(),
                X.end(),
                [&ratio](const uint16_t & x, const uint16_t &  y)
                {
                    if (ratio.find(x) == ratio.end() or ratio.find(y) == ratio.end())
                        return false;
                    else
                        return ratio.at(x) <= ratio.at(y);
                }
        );

		uint16_t j(*X.begin());

		if (_earliestCompletionTime[j][t] != -1)
		{
			t = _earliestCompletionTime[j][t];
			profit += _w[j];


			orders.erase(std::find(orders.begin(), orders.end(), j));
			seq.push_back(j);
		}
	}
	else
	{
		return -1;
	}


	return 0;
}

std::vector<uint16_t> Instance::Heuristic(core::heuristic_name hName, core::heuristic_ratio hRatio, std::chrono::time_point<std::chrono::system_clock>& start, std::vector<double> & duration)	const
{

	std::vector <uint16_t> seq;


	switch (hName)
	{

		case core::heuristic_name::INSERT:
		{
			seq = Heuristic1(hRatio);
			break;
		}

		case core::heuristic_name::INSERT_INTV:
		{
			seq = Heuristic2();
			break;
		}


		case core::heuristic_name::INSERT_DP:
		{
			seq = Heuristic3(hRatio);
			break;
		}

		case core::heuristic_name::DPH:
		{
			seq = Heuristic4();
			break;
		}

		default:
		{
			break;
		}

	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	duration.push_back(elapsed_seconds.count());

	start = end;


	return seq;
}

std::vector<uint16_t> Instance::Heuristic1(core::heuristic_ratio HeuristicRatio) const
{
	int16_t t(0);
	uint16_t profit(0);

	std::vector <uint16_t> orders, seq;

	for (uint16_t i(0); i < _n; ++i)
	{
		orders.push_back(i);
	}


	while (t <= _dmax)
	{
		int res = mainloop_insertionHeuristic(t, orders, seq, profit, HeuristicRatio);
		if (res == -1)
		{
			break;
		}
	}

	return seq;

}



std::vector<uint16_t> Instance::Heuristic2() const
{					

	std::vector<uint16_t> L;

	for (uint16_t i(0); i < _n; ++i)
	{
		L.push_back(i);
	}

	std::sort
	(
		L.begin(),
		L.end(),
		[this](uint16_t x, uint16_t y)
		{
			return _d[x] <= _d[y];
		}	 
	);


	uint16_t pmax(*std::max_element(_p, _p + _n));
	uint16_t profit(0);


	uint16_t a(0), b(pmax - 1);
	std::vector<uint16_t> seq;

	while (b <= _dmax+1)
	{

		uint16_t Emin(energyMinimalInInterval(a, b));

		std::vector<uint16_t> A;
		for (uint16_t i : L)
		{
			if (_e[i] <= Emin and b <= _d[i]+1)
			{
				A.push_back(i);
			}
		}

		if (not A.empty())
		{
			core::Matrix f(DP(A, a, b));
			std::vector<uint16_t> subseq(getSeqFromDP(f, A, a, b));

			if (not subseq.empty())
			{

				for (uint16_t j : subseq)
				{
					seq.push_back(j);

					std::vector<uint16_t>::iterator toRemove(std::find(L.begin(), L.end(), j));

					L.erase(toRemove);
				}
				profit += computeProfit(subseq);
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
	
	return seq;
}

std::vector<uint16_t> Instance::Heuristic3(core::heuristic_ratio HeuristicRatio) const
{
	int16_t t(0);

	std::vector<uint16_t> sequence;
	std::vector<uint16_t> orders;

	std::map<uint16_t, double> ratio;

	for (uint16_t i(0); i < _n; ++i)
	{
		auto found = std::find(sequence.begin(), sequence.end(), i);
		if (found == sequence.end())
		{
			orders.push_back(i);
			ratio.emplace(i, std::numeric_limits<double>::infinity());
		}
	} 


	while (t <= _dmax)
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
			
			if (_earliestCompletionTime[i][t] == -1)
			{
				ratio[i] = std::numeric_limits<double>::infinity();
			}
			else
			{
				
				switch (HeuristicRatio)
				{
					
					case core::heuristic_ratio::NO_RATIO:
					{
						ratio[i] = (double)_w[i] - (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A));
						break;
					}


					case core::heuristic_ratio::RATIO_A:
					{

						std::function<double(int16_t, uint16_t, uint16_t)> computationA = [&](int16_t C, uint16_t p, uint16_t w)
						{ return (double)w/(double)C; };

						int di = _d[i];
						for (int16_t t(0); t <= _T; ++t)
						{
								if (_earliestCompletionTime[i][t + 1] == -1)
								{
									di =  _earliestCompletionTime[i][t];
								}
							
						}
						
						double W = (double)_w[i];
						double C = (double)_earliestCompletionTime[i][t];
						std::vector<uint16_t> seqDP(DPUpperBound(_earliestCompletionTime[i][t], A));
						double DP = (double)computeProfit(seqDP);
						double Tmax = (double)_dmax + 1;

						int tj = _earliestCompletionTime[i][t];

						int tnext = Tmax;
						uint16_t nextJob = i;
						for (uint16_t x : seqDP)
						{
							if (_earliestCompletionTime[x][tj] < tnext and _earliestCompletionTime[x][tj] != -1)
							{
								tnext = _earliestCompletionTime[x][tj];
								nextJob = x;
							}
						}

						


						if (DP == 0)
						{
							ratio[i] = (C/ W);
						}
						else
						{
							ratio[i] = (C/W) + ((double)tnext/DP);
						}
				

						break;
					}

					case core::heuristic_ratio::RATIO_B:
					{
						std::function<double(int16_t, uint16_t, uint16_t)> computationB = [&](int16_t C, uint16_t p, uint16_t w)
						{ 
							if (C - p != 0)
								return  (double)w / ((double)C - (double)p);
							else
								return (double)w;
						};
					
						double W = (double)_w[i];

						double P = (double)_p[i];
						double C = (double)_earliestCompletionTime[i][t];
						int tj = _earliestCompletionTime[i][t];
						std::vector<uint16_t> seqDP(DPUpperBound(_earliestCompletionTime[i][t], A));
						double DP = (double)computeProfit(seqDP);
						double Tmax = (double)_dmax + 1;
						//ratio[i] = (W / (C-P)) + (DP / (Tmax - C));
						//

						int tnext = Tmax;
						uint16_t nextJob = i;
						for (uint16_t x : seqDP)
						{
							if (_earliestCompletionTime[x][tj] < tnext and _earliestCompletionTime[x][tj] != -1)
							{
								tnext = _earliestCompletionTime[x][tj];
								nextJob = x;
							}
						}




						if (DP == 0)
						{
							ratio[i] = ((C - P) / W);
						}
						else
						{
							ratio[i] = (((C - P) / W) + ((tnext - _p[nextJob]) / DP));
						}
						
						//ratio[i] = computationB(_earliestCompletionTime[i][t], _p[i], _w[i]) + (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A)) / (double)((double)(_dmax+1) - (double)_earliestCompletionTime[i][t]);

						/*double X = computationB(_earliestCompletionTime[i][t], _p[i], _w[i]);
						double Y = (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A));
						double Z = (double)((double)(_dmax + 1) - (double)_earliestCompletionTime[i][t]);
						ratio[i] = X + Y / (double)_earliestCompletionTime[i][t];*/

						break;
					}

					case core::heuristic_ratio::RATIO_C:
					{
						std::function<double (int16_t, uint16_t, uint16_t)> computationC = [&](int16_t C, uint16_t p, uint16_t w)
						{ 
							if (C - p != 0)
								return ((double)w / (double)C) + ((double)w / ((double)C - (double)p));
							else
								return ((double)w / (double)C) + ( (double)w); 
						
						};

						double W = (double)_w[i];

						double P = (double)_p[i];
						double C = (double)_earliestCompletionTime[i][t];
						double DP = (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A));
						double Tmax = (double)_dmax + 1;
						

						if (DP == 0)
						{
							ratio[i] = (C/W) + ((C - P) / W);
						}
						else
						{
							ratio[i] = ((C / W) + (C / DP)) + (((C - P) / W) + (C / DP));
						}
						
						//ratio[i] = ((C / W) + (C / DP)) + (( (C-P) / W) + (C/DP));

						//ratio[i] = computationC(_earliestCompletionTime[i][t], _p[i], _w[i]) + (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A)) / (double)((double)(_dmax+1) - (double)_earliestCompletionTime[i][t]);
						//ratio[i] = computationC(_earliestCompletionTime[i][t], _p[i], _w[i]) + (double)computeProfit(DPUpperBound(_earliestCompletionTime[i][t], A)) / (double)((double)(_dmax + 1) - (double)_earliestCompletionTime[i][t]);



						break;
					}

					default:
					{
						break;
					}

				}

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


		std::sort
		(
			L.begin(),
			L.end(),
			[&ratio](uint16_t x, uint16_t y)
			{
				return ratio.at(x) <= ratio.at(y);
			}

		);
		

		if (not L.empty())
		{
			uint16_t j(*L.begin());

			if (_earliestCompletionTime[j][t] != -1)
			{
				sequence.push_back(j);
				orders.erase(std::find(orders.begin(), orders.end(), j));
				t = _earliestCompletionTime[j][t];
			}
		}
		else
		{
			break;
		}



	}

	return sequence;
}

std::vector<uint16_t> Instance::Heuristic4() const
{

	std::vector<uint16_t> sequence;
	std::vector<uint16_t> orders;

	std::vector<uint16_t> R;

	// remove non processable orders
	for (int16_t i(0); i < _n; ++i)
	{
		if (_earliestCompletionTime[i][0] == -1)
		{
			R.push_back(i);
	   }

	}

	// candidates jobs 
	std::vector<uint16_t> A;
	for (uint16_t i(0); i < _n; ++i)
	{
		std::vector<uint16_t>::const_iterator it(std::find(R.cbegin(), R.cend(), i));
		if (it == R.cend())
		{
			A.push_back(i);
		}
	}

	if (A.empty())
	{
		return sequence;
	}

	std::map<uint16_t, uint16_t> d;
	// get deadline 
	for (uint16_t i : A)
	{
		for (int16_t t(0); t <= _T; ++t)
		{
			if (_earliestCompletionTime[i][t + 1] == -1)
			{
				d.emplace(i, _earliestCompletionTime[i][t] - _p[i]);

			}
		}
	}




	// sort by deadline
	std::sort(A.begin(), A.end(), [&d](uint16_t x, uint16_t y) { return d[x] <= d[y]; });

	int16_t T(_T);

	uint16_t n(A.size());
	uint16_t i(A.at(n-1));

	core::Matrix f;

	for (int16_t t(0); t <= T; ++t)
	{
		f.emplace(i, std::map<uint16_t, core::tupleDP>());
		int16_t tt(_earliestCompletionTime[i][t]);
		if ( tt == -1)
		{
			f[i].emplace(t, core::tupleDP(_w[i],false,t));
		}
		else
		{
			f[i].emplace(t, core::tupleDP(0, true, tt));
		}
	}
												  

	std::vector<uint16_t> B(A);
	std::sort(B.begin(), B.end(), [&d](uint16_t x, uint16_t y) {return d[x] >= d[y]; });

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

			int16_t tt (_earliestCompletionTime[j][t]);
			if (tt == -1)
			{
				f[j][t].setProfit(f.at(i).at(t).getProfit() + _w[j]);
			}
			else
			{
				if (f.at(i).at(t).getProfit() + _w[j] <= f.at(i).at(tt).getProfit())
				{
					f[j][t].setProfit(f.at(i).at(t).getProfit() + _w[j]);
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

	return getSeqFromDP(f,A,0,_T);
}

uint16_t Instance::computeProfit(const std::vector<uint16_t>& seq) const
{
	uint16_t profit(0);
	for (uint16_t i : seq)
	{
		profit += _w[i];
	}

	return profit;
}


std::vector<uint16_t> Instance::DPUpperBound(uint16_t tinit, const std::set<uint16_t> & visited) const
{
	if (visited.size() == _n)
	{
		return {};
	}


	std::vector<uint16_t> A;
	
	for (uint16_t i(0); i < _n; ++i)
	{
		if ((visited.find(i) == visited.cend()) and ( tinit <= _d[i] - _p[i] +1) and (_earliestCompletionTime[i][tinit] != -1) )
		{
			A.push_back(i);
		}
	}

	if (A.empty())
	{
		return {};
	}


	core::Matrix f(DP(A, tinit, _dmax+1));

	return getSeqFromDP(f, A, tinit, _dmax + 1);
}

std::vector<uint16_t> Instance::DPUpperBoundClassic(uint16_t tinit, const std::set<uint16_t>& visited) const
{
	if (visited.size() == _n)
	{
		return {};
	}


	std::vector<uint16_t> A;

	for (uint16_t i(0); i < _n; ++i)
	{
		A.push_back(i);
	}

	if (A.empty())
	{
		return {};
	}


	core::Matrix f(DP(A, tinit, _dmax + 1));



	return getSeqFromDP(f,A,tinit,_dmax+1);
}

std::vector<uint16_t> Instance::MooreUpperBound(uint16_t tinit, const std::set<uint16_t>& visited) const
{
	// get list of candidate jobs
	std::vector<uint16_t> orders;
	for (uint16_t i(0); i < _n; ++i)
	{
		if (visited.find(i) == visited.end())
		{
			orders.push_back(i);
		}
	}

	// sort orders by EDD
	std::sort
	(
		orders.begin(),
		orders.end(),
		[this](uint16_t x, uint16_t y)
		{
			return _d[x] <= _d[y];
		}
	);

	std::vector<uint16_t> A;
	int16_t pA(tinit);
	
	for (uint16_t i : orders)
	{

		if (pA + _p[i] <= _d[i]+1)
		{
			A.push_back(i);
			pA += _p[i];
		}
		else
		{
			A.push_back(i);

			std::vector<uint16_t>::iterator longestAcceptedJob
			(
				std::max_element
				(
					A.begin(),
					A.end(),
					[this](uint16_t x, uint16_t y)
					{
						return (double)_p[x]/(double)_w[x] <= (double)_p[y]/(double)_w[y];
					}
				)
			);


			uint16_t k(*longestAcceptedJob);
			std::vector<uint16_t>::iterator toRemove(std::find(A.begin(), A.end(), k));

			pA = pA + _p[i] - _p[k];

			A.erase(toRemove);
		
		}
	}


	if (A.empty())
		return {};

	std::vector<uint16_t> N;
	for (uint16_t k(0); k < _n; ++k)
	{
		N.push_back(k);
	}

	
	// sort by increasing w
	std::sort(N.begin(), N.end(),
	[this]	(uint16_t x, uint16_t y)
	{
			return _w[x] >= _w[y];
	}
	);

	std::vector<uint16_t> Nl;
	for (uint16_t l(0); l < A.size(); ++l)
	{
		Nl.push_back(N.at(l));
	}


	return Nl;
}

std::vector<uint16_t> Instance::MILPUpperBound(uint16_t tinit, const std::set<uint16_t>& visited, int numThread) const
{


	std::vector<uint16_t> A;

	for (uint16_t i(0); i < _n; ++i)
	{
		if ((visited.find(i) == visited.cend()) and (tinit <= _d[i] - _p[i] + 1) and (_earliestCompletionTime[i][tinit] != -1))
		{
			A.push_back(i+1);
		}
	}

	if (A.empty())
	{
		return {};
	}


	uint16_t profit = 0;

	/* TODO */
	return { profit };
}

std::vector<uint16_t> Instance::UpperBound(core::upperBound_name ubName, int16_t tinit, const std::set<uint16_t>& visited, int numThread, std::chrono::time_point<std::chrono::system_clock>& start, std::vector<double>& duration) const
{
	std::vector<uint16_t>  ub;

	switch (ubName)
	{
		case core::upperBound_name::Milp:
		{

			ub = MILPUpperBound(tinit, visited, numThread);
			break;
		}

		case core::upperBound_name::Moore:
		{
			ub = MooreUpperBound(tinit, visited);


			break;
		}

		case core::upperBound_name::DP:
		{
			ub = DPUpperBound(tinit, visited);
			break;
		}

		default:
		{
			break;
		}


	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	duration.push_back(elapsed_seconds.count());

	start = end;


	return ub;
}

uint16_t Instance::Heuristic1LowerBound(uint16_t tinit, const std::vector<uint16_t>& seq,  const std::set<uint16_t>& visited, core::heuristic_ratio HeuristicRatio) const
{
	std::vector<uint16_t> orders, sequence(seq);
	for (uint16_t i(0); i < _n; ++i)
	{
		if (visited.find(i) == visited.cend())
		{
			orders.push_back(i);
		}
	}

	uint16_t estim(0);
	int16_t t(tinit);
	int res = mainloop_insertionHeuristic(t, sequence, orders, estim, HeuristicRatio);

	if (res == -1)
    {
        return 0;
    }
    else
    {
        return computeProfit(sequence);
    }
}

std::ostream & Instance::printCharacteristics(std::ostream& oss, char delim) const
{
	oss << getN() << delim << getDL() << delim << getDU() << delim << utils::datfile::fromCorrLevel(getCorrLevel()) << delim << getInstNum();
	return oss;
}

void Instance::printSequence(const std::vector<uint16_t>& seq) const
{
		  
	std::map<uint16_t, std::map<int16_t,uint16_t>> x;

	for (uint16_t i(0); i < _n; ++i)
	{
		x.emplace(0, std::map<int16_t, uint16_t>());
		for (uint16_t t(0); t < _dmax; ++t)
		{
			x[i].emplace(t,0);
		}
	}

	uint16_t t(0);
	std::vector<uint16_t> L(seq);
	while (not L.empty())
	{
		if (not L.empty())
		{
			uint16_t j(*L.begin());

			int16_t tend(_earliestCompletionTime[j][t]);
			if (tend != -1)
			{
				int16_t tbegin(tend - _p[j]);
				for (int16_t tt(tbegin); tt < tend; ++tt)
				{
					x[j][tt] = 1;
				}


				t = tend;
			}
			else
			{
				break;
			}
			std::vector<uint16_t>::iterator it = std::find(L.begin(), L.end(), j);

			if (it != L.end()) 
				L.erase(it);
		}
	}

}

void Instance::printEarliestCompletionTimes() const
{
	for (uint16_t i(0); i < _n; ++i)
	{
		std::cout << i << " : ";
		for (uint16_t t(0); t <= _dmax; ++t)
		{
			std::cout << "\tt="<< t << " -> " << _earliestCompletionTime[i][t] << std::endl;
		}
		std::cout << std::endl;
	}
}

std::ostream& operator<<(std::ostream& os, const Instance& o)
{

	os << "n=" << o.getN() << std::endl;
	os << "p=";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getP(i) << " ";
	os << std::endl;
	os << "d= ";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getD(i) << " ";
	os << std::endl;
	os << "w= ";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getW(i) << " ";
	os << std::endl;
	os << "e= ";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getE(i) << " ";
	os << std::endl;
	os << "E= ";
	for (size_t t(0); t < o.getT(); ++t)
		os << o.getEE(t) << " ";
	os << std::endl;

	return os << "";
}

Instance* Instance::load(const char* datname)
{

	std::ifstream f(datname);
	std::string line;

	utils::datfile::s_characteristics * s = utils::datfile::getCharacteristicsFromName(datname);


	size_t n(0);
	uint16_t T(0), dmax(0);
	uint16_t* p = nullptr, * d = nullptr, * w = nullptr, * e = nullptr, * E = nullptr;


	while (std::getline(f, line))
	{
		if (not (line[0] == '/') and not (line[1] == '/'))
		{
			std::stringstream iss(line);
			std::string paramName, values;

			std::getline(iss, paramName, '=');
			utils::trim(paramName);

			std::getline(iss, values, ';');

			std::vector<std::string> tokens(utils::tokenize(values, { '[',',',']' }));

			utils::cleanVector(tokens);



			switch (utils::resolveOption(paramName))
			{

			case  utils::OptionsParams::OrderNb:
			{
				n = atoi(values.c_str());
				T = TMAX;
				break;
			}

			case utils::OptionsParams::ProcessingTime:
			{
				p = (uint16_t*)calloc(n, sizeof(uint16_t));

				for (size_t i(0); i < n; ++i)
				{
					p[i] = atoi(tokens.at(i).c_str());
				}

				break;
			}

			case utils::OptionsParams::DueDate:
			{
				d = (uint16_t*)calloc(n, sizeof(uint16_t));

				for (size_t i(0); i < n; ++i)
				{
					d[i] = atoi(tokens.at(i).c_str());
				}

				dmax = std::min(T,*std::max_element(d, d + n));

				break;
			}

			case utils::OptionsParams::Weight:
			{

				w = (uint16_t*)calloc(n, sizeof(uint16_t));

				for (size_t i(0); i < n; ++i)
				{
					w[i] = atoi(tokens.at(i).c_str());
				}

				break;
			}

			case utils::OptionsParams::Energy:
			{

				e = (uint16_t*)calloc(n, sizeof(uint16_t));

				for (size_t i(0); i < n; ++i)
				{
					e[i] = atoi(tokens.at(i).c_str());
				}

				break;
			}

			case utils::OptionsParams::EnergyPeriods:
			{
				E = (uint16_t*)calloc(T, sizeof(uint16_t));

				for (uint16_t t(0); t < T; ++t)
				{
					E[t] = atoi(tokens.at(t).c_str());
				}

				break;
			}

			
			case utils::OptionsParams::Invalid:
			{
				throw("ERROR: invalid parameter.");
				break;
			}

			}
		}


	}

	f.close();


	return new Instance(s,datname, n, p, d, w, e, E, T, dmax);
}