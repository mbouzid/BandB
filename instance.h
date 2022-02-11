#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <set>

class Instance
{

	private:

	size_t _n;

	uint16_t* _p;
	uint16_t* _d;
	uint16_t* _w;
	uint16_t* _e;
	uint16_t* _E;

	size_t _T;
	size_t _dmax;

	int16_t ** _earliestCompletionTime;


	// constructor
	Instance
	(
		size_t n,
		uint16_t* p,
		uint16_t* d,
		uint16_t* w,
		uint16_t* e,
		uint16_t* E,
		size_t T,
		size_t dmax,
		int16_t ** earliestCompletionTime
	):
		_n(n),
		_p(p),
		_d(d),
		_w(w),
		_e(e),
		_E(E),
		_T(T),
		_dmax(dmax),
		_earliestCompletionTime(earliestCompletionTime)
	{}

	bool inRange_n(size_t idx) const
	{
		return ((0 >= idx) and (idx <= _n));
	}

protected:

	void computeRatio(std::map<uint16_t, double>& ratio, const std::vector <uint16_t> & orders, size_t t)	const
	{
		for (size_t i : orders)
		{
			int16_t C(_earliestCompletionTime[i][t]);

			if (ratio.find(i) == ratio.end())
			{
				ratio.emplace(i, -1);
			}
			
			if (C == -1)
				ratio[i] = -1;
			else
				ratio[i] = ((double)_d[i] - (double)_earliestCompletionTime[i][t]) / (double)_w[i];
			
		}

	}


public:

	Instance(const Instance& _) :
		_n(_._n),
		_p((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_d((uint16_t*)calloc(_n, sizeof(uint16_t))),  
		_w((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_e((uint16_t*)calloc(_n, sizeof(uint16_t))),	
		_E((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_T(_._T),
		_dmax(_._dmax),
		_earliestCompletionTime((int16_t**)calloc(_n,sizeof(int16_t*)))
	{
		memcpy(_p, _._p, sizeof(uint16_t) * (_n));
		memcpy(_d, _._d, sizeof(uint16_t) * (_n));
		memcpy(_w, _._w, sizeof(uint16_t) * (_n));
		memcpy(_e, _._e, sizeof(uint16_t) * (_n));
		memcpy(_E, _._E, sizeof(uint16_t) * _n);

		memcpy(_earliestCompletionTime, _._earliestCompletionTime, sizeof(int16_t*) * _n);

		for (size_t i(0); i < _n; ++i)
		{
			_earliestCompletionTime[i] = (int16_t*)calloc(_d[i]+1, sizeof(int16_t));
			memcpy(_earliestCompletionTime[i], _._earliestCompletionTime[i], sizeof(int16_t) * (_dmax));
		}
	}

	static Instance* load(const char* datname);

	size_t getT() const
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

	size_t getDmax() const
	{
		return _dmax;
	}

	int16_t getEarliestCompletionTime(size_t i, size_t t) const
	{
		return _earliestCompletionTime[i][t];
	}


	uint16_t Heuristic1() const;

	uint16_t DPUpperBound(size_t tinit, const std::set<uint16_t>& visited) const;


	~Instance()
	{
		delete _p;
		delete _d;
		delete _w;
		delete _e;
		delete _E;
		delete [] _earliestCompletionTime;

	}
};


std::ostream& operator << (std::ostream&, const Instance&);