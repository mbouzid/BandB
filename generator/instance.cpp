#include "instance.h"
#include "common/utils.h"
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
	std::stable_sort
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

/*


	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	duration.push_back(elapsed_seconds.count());

	start = end;


	return ub;
}
*/


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