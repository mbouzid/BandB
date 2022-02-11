#pragma once
#include "instance.h"
#include <set>

class node
{
	private:

		uint16_t _jobBranch;
		uint16_t _level;
		uint16_t _profit;
		uint16_t _upperBound;
		uint16_t _t;
		std::set<uint16_t> _visited;
		std::set<uint16_t> _sequence;
		uint16_t _priority;

	public:

		node
		(
			uint16_t jobBranch,
			uint16_t level,
			uint16_t profit,
			uint16_t upperBound,
			uint16_t t,
			const std::set<uint16_t> & visited,
			const std::set<uint16_t> & sequence
		) :
			_jobBranch(jobBranch),
			_level(level),
			_profit(profit),
			_upperBound(upperBound),
			_t(t),
			_visited(visited),
			_sequence(sequence),
			_priority(0)
		{
			_priority = _level * _profit * _upperBound * _t;
		}

		node(const node & _ ):
			_jobBranch(_._jobBranch),
			_level(_._level),
			_profit(_._profit),
			_upperBound(_._upperBound),
			_t(_._t),
			_visited(_._visited),
			_sequence(_._sequence),
			_priority(_._priority)
		{}

		node* dummyNode()
		{
			return new node(-1, 0, 0, 0, 0, std::set<uint16_t>(), std::set<uint16_t>());
		}

		bool operator <(const node& _) const
		{
			return _priority < _._priority;
		}

		~node()
		{

		}

};