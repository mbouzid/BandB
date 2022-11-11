#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <chrono>
#include "common/utils.h"
#include "solver/core.h"



class Instance
{

	private:

	const char* _datfile;
	utils::datfile::s_characteristics * _characteristics;

	size_t _n;

	uint16_t* _p;
	uint16_t* _d;
	uint16_t* _w;
	uint16_t* _e;
	uint16_t* _E;

	uint16_t _T;
	uint16_t _dmax;

	int16_t ** _earliestCompletionTime;


	// constructor
	Instance
	(
		utils::datfile::s_characteristics * characteristics,
		const char * datfile,
		size_t n,
		uint16_t* p,
		uint16_t* d,
		uint16_t* w,
		uint16_t* e,
		uint16_t* E,
		uint16_t T,
		uint16_t dmax
	):
		_characteristics(characteristics),
		_datfile(datfile),
		_n(n),
		_p(p),
		_d(d),
		_w(w),
		_e(e),
		_E(E),
		_T(T),
		_dmax(dmax),
		_earliestCompletionTime((int16_t**)calloc(n, sizeof(int16_t*)))
	{

		for (size_t i(0); i < _n; ++i)
		{
			_earliestCompletionTime[i] = (int16_t*)calloc(_T+1, sizeof(int16_t));
			for (uint16_t t(0); t <= _T; ++t)
			{
				_earliestCompletionTime[i][t] = computeEarliestCompletionTime(i, t);
			}
		}

	
	}

	bool inRange_n(uint16_t idx) const
	{
		return ((0 >= idx) and (idx <= _n));
	}

protected:

	void computeRatioInsertion(std::map<uint16_t, double>& ratio, const std::vector <uint16_t>& orders, int16_t t, const std::function<double (int16_t, uint16_t ,uint16_t)> & computation) const
	{
		for (uint16_t i : orders)
		{

			if (ratio.find(i) == ratio.end())
			{
				ratio.emplace(i, std::numeric_limits<double>::infinity());
			}

			if (_earliestCompletionTime[i][t] != -1)
			{
				ratio[i] = computation(_earliestCompletionTime[i][t], _p[i], _w[i]);
			}
		}

	}

	int16_t computeEarliestCompletionTime(uint16_t i, uint16_t t) const;

    void ratioSwitch(int16_t & t, std::map<uint16_t, double> & ratio, std::vector <uint16_t> & orders, core::heuristic::ratio HeuristicRatio) const;


public:

	Instance(const Instance& _) :
		_characteristics((utils::datfile::s_characteristics*)calloc(1,sizeof(_._characteristics))),
		_datfile(_._datfile),
		_n(_._n),
		_p((uint16_t*)calloc(_._n, sizeof(uint16_t))),
		_d((uint16_t*)calloc(_._n, sizeof(uint16_t))),  
		_w((uint16_t*)calloc(_._n, sizeof(uint16_t))),
		_e((uint16_t*)calloc(_._n, sizeof(uint16_t))),
		_E((uint16_t*)calloc(_._n, sizeof(uint16_t))),
		_T(_._T),
		_dmax(_._dmax),
		_earliestCompletionTime((int16_t**)calloc(_._n,sizeof(int16_t*)))
	{
		memcpy(_characteristics, _._characteristics, sizeof(_._characteristics) * 1);
		memcpy(_p, _._p, sizeof(uint16_t) * _._n);
		memcpy(_d, _._d, sizeof(uint16_t) * _._n);
		memcpy(_w, _._w, sizeof(uint16_t) * _._n);
		memcpy(_e, _._e, sizeof(uint16_t) * _._n);
		memcpy(_E, _._E, sizeof(uint16_t) * _._n);

		memcpy(_earliestCompletionTime, _._earliestCompletionTime, sizeof(int16_t*) * _._n);

		for (size_t i(0); i < _n; ++i)
		{
			_earliestCompletionTime[i] = (int16_t*)calloc(_._T+1, sizeof(int16_t));
			memcpy(_earliestCompletionTime[i], _._earliestCompletionTime[i], sizeof(int16_t) * _._T+1);
		}
	}

	static Instance* load(const char* datname);

	
							   
	const char* getDatFile() const
	{
		return _datfile;
	}

	uint16_t getDL() const
	{
		return _characteristics->dL;
	}

	uint16_t getDU() const
	{
		return _characteristics->dU;
	}

	utils::datfile::e_corrLevel getCorrLevel() const
	{
		return _characteristics->corrLevel;
	}


	uint16_t getInstNum() const
	{
		return _characteristics->numInst;
	}

	uint16_t getT() const
	{
		return _T;
	}

	size_t getN() const
	{
		return _n;
	}

	uint16_t getP(size_t i) const
	{
		return _p[i];
	}

    uint16_t getPmax() const
    {
        return *std::max_element(_p, _p + _n);
    }

	uint16_t getD(size_t i)	const
	{
		return _d[i];
	}


	uint16_t getW(size_t i) const
	{
		return _w[i];

	}

	uint16_t getE(size_t i) const
	{
		return _e[i];
	}

	uint16_t getEE(size_t i) const
	{
		return _E[i];

	}

    uint16_t energyMinimalInInterval(uint16_t a, uint16_t b) const;


    uint16_t getDmax() const
	{
		return _dmax;
	}

	int16_t getEarliestCompletionTime(size_t i, size_t t) const
	{
		return _earliestCompletionTime[i][t];
	}


	std::vector<uint16_t> getSeqFromDP(const core::Matrix& f, std::vector<uint16_t>& A, int16_t a, int16_t b) const;

	/* check feasibility from tinit with sequence */
	uint16_t checkProfit(int16_t tinit, const std::set<uint16_t> & visited) const;

	core::Matrix DP(std::vector<uint16_t>& A, uint16_t a, uint16_t b) const;


	/*std::vector<uint16_t> Heuristic4() const;


	uint16_t computeProfit(const std::vector <uint16_t> & ) const;

	std::vector<uint16_t> DPUpperBound(uint16_t tinit, const std::set<uint16_t>& visited) const;
	std::vector<uint16_t> DPUpperBoundClassic(uint16_t tinit, const std::set<uint16_t>& visited) const;

	std::vector<uint16_t> MooreUpperBound(uint16_t tinit, const std::set<uint16_t>& visited) const;
	std::vector<uint16_t> MILPUpperBound(uint16_t tinit, const std::set<uint16_t>& visited, int numThread) const;
	
	std::vector<uint16_t> UpperBound(core::upperBound_name ubName, int16_t tinit, const std::set<uint16_t>& visited, int numThread, std::chrono::time_point<std::chrono::system_clock>& start, std::vector<double>& duration) const;
	*/

	uint16_t getTotalImpact(const std::vector<uint16_t>& sequence) const
	{
		uint16_t impact(0);
		for (uint16_t j : sequence)
		{
			impact += _e[j]*_p[j];
		}

		return impact;
	}
	


	~Instance()
	{
		free(_characteristics);
		delete _p;
		delete _d;
		delete _w;
		delete _e;
		delete _E;
		delete [] _earliestCompletionTime;

	}

	std::ostream& printCharacteristics(std::ostream& oss, char delim) const ;
	void printSequence(const std::vector<uint16_t>& seq) const;

	void printEarliestCompletionTimes() const;

};


std::ostream& operator << (std::ostream&, const Instance&);