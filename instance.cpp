#include "instance.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <sstream>

uint16_t Instance::Heuristic1()	const
{

	uint16_t t(0);
	uint16_t profit(0);

	uint16_t dmax = *std::max_element(_d,_d+_n);

	std::cout << "dmax=" << dmax << std::endl;

	while (t <= dmax)
	{

		++t;
	}


	return profit;
}

std::ostream& operator<<(std::ostream& os, const Instance& o)
{

	os << "n=" << o.getN() << std::endl;
	os << "p=";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getP(i) << " ";
	os << std::endl;
	os << "d= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getD(i) << " ";
	os << std::endl;
	os << "w= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getW(i) << " ";
	os << std::endl;
	os << "e= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getE(i) << " ";
	os << std::endl;
	os << "E= ";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getEE(i) << " ";
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

	earliestCompletionTime = (int16_t**)calloc(n, sizeof(int16_t*));



	for (size_t i(0); i < n; ++i)
	{
		earliestCompletionTime[i] = (int16_t*)calloc(dmax, sizeof(int16_t));
		for (size_t t(0); t < dmax; ++t)
		{
			earliestCompletionTime[i][t] = -1;
			if (t + p[i] > d[i])
			{
				earliestCompletionTime[i][t] = -1;
			}
			else
			{
				for (size_t tt(t); tt < d[i] - p[i] + 1; ++tt)
				{
					bool energyConstraint(true);
					for (size_t ttt(tt); ttt < tt + p[i] + 1; ++ttt)
					{
						if (e[i] > E[ttt])
						{
							energyConstraint = false;
							break;
						}
					}

					if (energyConstraint)
					{
						earliestCompletionTime[i][t] = tt + p[i];
						break;
					}
				}
			}

		}
	}

	/*for (size_t i(0); i < n; ++i)
	{
		for (size_t t(0); t < dmax; ++t)
		{

			std::cout << "i=" << i << ",t=" << t << " : " << earliestCompletionTime[i][t] << std::endl;
		}
	}*/


	return new Instance(n, p, d, w, e, E, T, dmax,earliestCompletionTime);
}