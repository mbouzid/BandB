#pragma once

#include <cstdlib>
#include <iostream>
#include <map>

namespace core
{


	enum heuristic_name
	{
		INSERT,
		INSERT_INTV,
		INSERT_DP,
		DPH
	};

	enum heuristic_ratio
	{
		NO_RATIO,
		RATIO_A,
		RATIO_B,
		RATIO_C	
	};

	enum upperBound_name
	{
		Moore,
		Milp,
		DP
	};

	class tupleDP
	{
		uint16_t _profit;
		bool _accept;
		int16_t _time;


	public:

		tupleDP():
			_profit(0),
			_accept(false),
			_time(0)
		{}

		tupleDP(uint16_t profit, bool accept, int16_t time) :
			_profit(profit),
			_accept(accept),
			_time(time)
		{};

		tupleDP(const tupleDP& t) :
			_profit(t._profit),
			_accept(t._accept),
			_time(t._time)
		{};

		void setProfit(uint16_t profit)
		{
			_profit = profit;
		}

		uint16_t getProfit() const
		{
			return _profit;
		}

		void setAccept(bool accept)
		{
			_accept = accept;
		}

		bool getAccept() const
		{
			return _accept;
		}

		void setTime(int16_t time)
		{
			_time = time;
		}

		int16_t getTime() const
		{
			return _time;
		}
	};

	typedef std::map<uint16_t, std::map<uint16_t, tupleDP>> Matrix;

}