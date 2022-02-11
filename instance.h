#pragma once

#include <iostream>
#include <map>

class Instance
{

	private:

	size_t _n;

	uint16_t* _p;
	uint16_t* _d;
	uint16_t* _w;
	uint16_t* _e;
	uint16_t* _E;

	uint16_t _T;


	// constructor
	Instance
	(
		size_t n,
		uint16_t* p,
		uint16_t* d,
		uint16_t* w,
		uint16_t* e,
		uint16_t* E,
		size_t T	
	) :
		_n(n),
		_p(p),
		_d(d),
		_w(w),
		_e(e),
		_E(E),
		_T(T)
	{}

	bool inRange_n(size_t idx) const
	{
		return ((0 >= idx) and (idx <= _n));
	}


public:

	Instance(const Instance& _) :
		_n(_._n),
		_p((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_d((uint16_t*)calloc(_n, sizeof(uint16_t))),  
		_w((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_e((uint16_t*)calloc(_n, sizeof(uint16_t))),	
		_E((uint16_t*)calloc(_n, sizeof(uint16_t))),
		_T(_._T)
	{
		memcpy(_p, _._p, sizeof(uint16_t) * (_n));
		memcpy(_d, _._d, sizeof(uint16_t) * (_n));
		memcpy(_w, _._w, sizeof(uint16_t) * (_n));
		memcpy(_e, _._e, sizeof(uint16_t) * (_n));
		memcpy(_E, _._E, sizeof(uint16_t) * _n);

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



	~Instance()
	{
		delete _p;
		delete _d;
		delete _w;
		delete _e;
		delete _E;

	}
};


std::ostream& operator << (std::ostream&, const Instance&);