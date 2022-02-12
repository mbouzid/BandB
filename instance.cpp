#include "instance.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>

int16_t Instance::computeEarliestCompletionTime(uint16_t i, size_t t) const
{
	int16_t delta(-1);

	if (t + _p[i] <= _d[i])
	{
		for (size_t tt(t); tt <= _d[i] - _p[i] + 1; ++tt)
		{
			bool energyConstraint(true);
			for (size_t ttt(tt); ttt < tt + _p[i]; ++ttt)
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
	}

	return delta;
}

uint16_t Instance::Heuristic1()	const
{

	uint16_t t(0);
	uint16_t profit(0);

	uint16_t dmax = *std::max_element(_d,_d+_n);


	std::vector <uint16_t> orders(_n);
	std::map<uint16_t, double> ratio;

	size_t n(0);
	std::generate(orders.begin(), orders.end(), [&n] { return n++; });

	computeRatio(ratio, orders, t);


	while (t <= dmax)
	{
		std::vector<uint16_t>::iterator jiter = std::max_element
		(
			orders.begin(),
			orders.end(),
			[&ratio](uint16_t i,  uint16_t  j)
			{ 
				return ratio[i] < ratio[j];
			}
		);

		if (jiter != orders.end())
		{
			uint16_t j(*jiter);

			int16_t C = _earliestCompletionTime[j][t];
			if (C != -1)
			{
				t = C;
				profit += _w[j];
				orders.erase(jiter);
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

		computeRatio(ratio, orders, t);

	}


	return profit;
}

uint16_t Instance::DPUpperBound(size_t tinit, const std::set<uint16_t> & visited) const
{
	if (visited.size() == _n)
	{
		return 0;
	}

	std::vector<uint16_t> A;
	
	for (uint16_t i(0); i < _n; ++i)
	{
		if ((visited.find(i) == visited.cend()) and (_d[i] + 1 >= tinit + _p[i]) and (_earliestCompletionTime[i][tinit] != -1) )
		{
			A.push_back(i);
		}
	}

	if (A.empty())
	{
		return 0;
	}

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
	std::map < uint16_t, std::map<uint16_t, uint16_t> > f;

	size_t k(0);
	for (size_t ii(0); ii < A.size(); ++ii)
	{
		uint16_t j(A.at(ii));

		uint16_t T(0);
		if (f.find(j) == f.end())
		{
			f.emplace(j, std::map <uint16_t, uint16_t>());
		}

		if (j == i)
		{


			std::vector<uint16_t> sub(A.begin() + (k+1), A.end());
			
			for (uint16_t x : sub)
			{
				T += _p[x];
			}
			T += 1;
		
			for (size_t t(0); t < T; ++t)
			{
				if (tinit + _p[j] + t <= _d[j])
				{
					f[j].emplace(t, _w[j]);
				}
				else
				{
					f[j].emplace(t, 0);
				}
			}
		}
		else
		{
			for (uint16_t x : A)
			{
				T += _p[x];
			}
			T += 1;
			for (size_t t(0); t < T; ++t)
			{
				f[j].emplace(t, 0);
			}
		}

		++k;

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

		for (size_t t(0); t < T; ++t)
		{
			uint16_t PHI1(0);
			uint16_t PHI2(0);

			if (t + _p[j] + tinit <= _d[j])
			{
				PHI1 = _w[j] + f[jprev][t + _p[j]];
			}
			else
			{
				PHI1 = f[jprev][t + _p[j]];
			}
			
			uint16_t sum2(0);
			for (size_t l(0); l < k+1; ++l)
			{
				sum2 += _p[A.at(l)];
			}


			if (sum2 + t + tinit <= _d[j])
			{
				PHI2 = f[jprev][t] + _w[j];
			}
			else
			{
				PHI2 = f[jprev][t];
			}
				
			if (PHI1 < PHI2)
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



	uint16_t last(A.at(A.size() - 1));

	return f[last][0];
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

	size_t n(0), T(0), dmax(0);
	uint16_t* p = nullptr, * d = nullptr, * w = nullptr, * e = nullptr, * E = nullptr;
	int16_t **earliestCompletionTime = nullptr;


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
				T = (n * PMAX) -1;	
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

				dmax = *std::max_element(d, d + n);

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

				for (size_t t(0); t < T; ++t)
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


	return new Instance(n, p, d, w, e, E, T, dmax);
}