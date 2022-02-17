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

/* @deprecated */
utils::Matrix Instance::DP(std::vector<uint16_t>& A, uint16_t a, uint16_t b) const
{
	if (A.empty())
		return utils::Matrix();

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
			std::vector<uint16_t> sub(A.begin() + (k + 1), A.end());

			for (uint16_t x : sub)
			{
				T += _p[x];
			}
			T += 1;

			for (uint16_t t(0); t <= T; ++t)
			{
				if (a + _p[j] + t <= std::min((uint16_t)b,(uint16_t)(_d[j]+1)))
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
			for (uint16_t t(0); t <= T; ++t)
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

		for (uint16_t t(0); t <= T; ++t)
		{
			uint16_t PHI1(0);
			uint16_t PHI2(0);

			if (t + _p[j] + a <= std::min((uint16_t)b, (uint16_t)(_d[j]+1)))
			{
				PHI1 = _w[j] + f[jprev][t + _p[j]];
			}
			else
			{
				PHI1 = f[jprev][t + _p[j]];
			}

			uint16_t sum2(0);
			for (size_t l(0); l < k + 1; ++l)
			{
				sum2 += _p[A.at(l)];
			}


			if (sum2 + t + a <= std::min((uint16_t)b, (uint16_t)(_d[j] + 1)))
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

	return f;
}

utils::Matrix Instance::DP1(std::vector<uint16_t>& A, uint16_t a, uint16_t b) const
{
	if (A.empty())
		return utils::Matrix();

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
		for (uint16_t x : A)
		{
			T += _p[x];
		}

		if (f.find(j) == f.end())
		{
			f.emplace(j, std::map <uint16_t, uint16_t>());
		}

		if (j == i)
		{

			for (uint16_t t(a); t <= b; ++t)
			{
				if (t  + _p[j] <= _d[j] + 1)
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
			for (uint16_t t(a); t <= b; ++t)
			{
				f[j].emplace(t, 0);
			}
		}


	}



	k = 1;
	uint16_t jprev(i);

	for (size_t ii(1); ii < A.size(); ++ii)
	{
		uint16_t j(A.at(ii));

		for (uint16_t t(a); t <= b-_p[j]; ++t)
		{
			uint16_t PHI1(0);
			uint16_t PHI2(0);

			if (t + _p[j]  <= _d[j] + 1)
			{
				PHI1 = _w[j] + f[jprev][t + _p[j]];
			}
			else
			{
				PHI1 = f[jprev][t + _p[j]];
			}

			uint16_t sum2(0);
			for (size_t l(0); l < k + 1; ++l)
			{
				sum2 += _p[A.at(l)];
			}


			if (sum2 + t <= _d[j] + 1)
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

	return f;
}

std::vector<uint16_t> Instance::Heuristic1() const
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
		std::map<uint16_t, double> ratio;

		computeRatio(ratio, orders, t);

		std::vector<uint16_t> L(orders);
		for (uint16_t i : orders)
		{
			std::vector<uint16_t>::iterator found(std::find(L.begin(), L.end(), i));
			if (ratio.at(i) == -1 and found != L.end())
			{
				L.erase(found);
			}
		}

		std::sort
		(
			L.begin(),
			L.end(),
			[&ratio](uint16_t x,  uint16_t  y)
			{ 
				return ratio.at(x) <= ratio.at(y);
			}
		);


		if (not L.empty())
		{
			
			uint16_t j(*L.begin());

			int16_t C = _earliestCompletionTime[j][t];
			if (C != -1)
			{
				t = C;
				profit += _w[j];
				orders.erase(std::find(orders.begin(),orders.end(),j));
				seq.push_back(j);
			}
			/*else
			{
				break;
			}*/
		}
		else
		{
			break;
		}

	}

	return seq;
}

std::vector<uint16_t> Instance::Heuristic2() const
{					

	std::set<uint16_t> L;
	for (uint16_t i(0); i < _n; ++i)
	{
		L.insert(i);
	}

	uint16_t pmax(*std::max_element(_p, _p + _n));
	uint16_t profit(0);

	uint16_t a(0), b(pmax - 1);
	std::vector<uint16_t> seq;

	while (b <= _dmax)
	{
		uint16_t Emin(energyMinimalInInterval(a, b));

		std::vector<uint16_t> A;
		for (uint16_t i : L)
		{
			if (_e[i] <= Emin and b <= _d[i])
			{
				A.push_back(i);
			}
		}

		utils::Matrix f(DP1(A, a, b));
		
		if (not f.empty())
		{
			uint16_t j(A.at(A.size() - 1));
			seq.push_back(j);
			profit += _w[j];
			a += _p[j];
			b += _p[j];
			L.erase(j);
		}
		else
		{
			a += 1;
			b += 1;
		}

	}
	
	return seq;
}

std::vector<uint16_t> Instance::Heuristic3() const
{

	int16_t t(0);

	std::vector<uint16_t> sequence;
	std::vector<uint16_t> L;

	std::map<uint16_t, double> ratio;

	for (uint16_t i(0); i < _n; ++i)
	{
		L.push_back(i);
		ratio.emplace(i, -1.0);
	}



	while (t <= _dmax)
	{

		for (uint16_t i : L)
		{
			std::set<uint16_t> A;
			for (uint16_t j : L)
			{
				if (i != j)
				{
					A.insert(j);
				} 
			}
			ratio[i] = (_w[i]/_earliestCompletionTime[i][t]) + DPUpperBound(t + _p[i], A);
		} 

		std::sort
		(
			L.begin(),
			L.end(),
			[&ratio](uint16_t x, uint16_t y)
			{
				return ratio.at(x) >= ratio.at(y);
			}

		);

		if (not L.empty())
		{
			uint16_t j(*L.begin());

			if (_earliestCompletionTime[j][t] != -1)
			{
				sequence.push_back(j);
				L.erase(std::find(L.begin(), L.end(), j));
				t += _earliestCompletionTime[j][t];
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



	}

	return sequence;
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



uint16_t Instance::DPUpperBound(uint16_t tinit, const std::set<uint16_t> & visited) const
{
	if (visited.size() == _n)
	{
		return 0;
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
		return 0;
	}


	utils::Matrix f(DP1(A, tinit, _dmax+1));


	uint16_t last(A.at(A.size() - 1));

	return f.at(last).at(tinit);
}

void Instance::printSequence(const std::vector<uint16_t>& seq) const
{
		  
	utils::Matrix x;

	for (uint16_t i(0); i < _n; ++i)
	{
		x.emplace(0, std::map<uint16_t, uint16_t>());
		for (uint16_t t(0); t <= _dmax; ++t)
		{
			x[i].emplace(t,0);
		}
	}

	uint16_t t(0);
	std::vector<uint16_t> L(seq);
	while (t <= _dmax+1)
	{
		if (not seq.empty())
		{
			uint16_t j(*L.begin());

			int16_t tend(_earliestCompletionTime[j][t]);
			if (tend != -1)
			{
				int16_t tbegin(tend - _p[j]);
				for (uint16_t tt(tbegin); tt < tend; ++tt)
				{
					x[j][tt] = 1;
				}

				t = tend;
			}
			else
			{
				std::cout << "could not place j=" << j << " , t= " << t << " =" << _earliestCompletionTime[j][t]<< std::endl;
				break;
			}
			L.erase(std::find(L.begin(), L.end(), j));
		}
	}

	for (uint16_t i(0); i < _n; ++i)
	{
		std::cout << i << " : ";
		for (uint16_t t(0); t <= _dmax; ++t)
		{
			std::cout << x.at(i).at(t) << " ";
		}
		std::cout << std::endl;
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
				T = (n * PMAX) -1 ;	
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


	return new Instance(n, p, d, w, e, E, T, dmax);
}