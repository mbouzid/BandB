#pragma once
#include "common/utils.h"

class Node
{
	private:

		int16_t _jobBranch;
		uint16_t _level;
		uint16_t _profit;
		uint16_t _upperBound;
		int16_t _t;
		std::set<uint16_t> _visited;
		std::vector<uint16_t> _sequence;
		uint16_t _priority;

	public:

		Node
		(
			uint16_t jobBranch,
			uint16_t level,
			uint16_t profit,
			uint16_t upperBound,
			uint16_t t,
			const std::set<uint16_t> & visited,
			const std::vector<uint16_t> & sequence
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
			_priority = _level * (_profit + _upperBound) *_t;
		}


		Node(const Node & _ ):
			_jobBranch(_._jobBranch),
			_level(_._level),
			_profit(_._profit),
			_upperBound(_._upperBound),
			_t(_._t),
			_visited(_._visited),
			_sequence(_._sequence),
			_priority(_._priority)
		{}

		uint16_t getJob() const
		{
			return _jobBranch;
		}

		uint16_t getLevel() const
		{
			return _level;
		}

		uint16_t getProfit() const
		{
			return _profit;
		}

		uint16_t getUpperBound() const
		{
			return _upperBound;
		}

		void setUpperBound(uint16_t UB)
		{
			_upperBound = UB;
		}

		uint16_t getPriority() const
		{
			return _priority;
		}

		uint16_t getT() const
		{
			return _t;
		}

		

		bool findInVisited(uint16_t i) const
		{
			return _visited.find(i) != _visited.cend();
		}

		bool findInSequence(uint16_t i) const
		{
			return std::find(_sequence.cbegin(), _sequence.cend(), i) != _sequence.cend();
		}


		void addToVisited(uint16_t i)
		{
			_visited.insert(i);
		}

		const std::set<uint16_t>& getVisited()	const
		{
			return _visited;
		}

		const std::vector<uint16_t>& getSequence()	const
		{
			return _sequence;
		}

		static Node* dummyNode()
		{
			return new Node(-1, 0, 0, 0, 0, utils::emptySet(), utils::emptyVector());
		}

		bool operator <(const Node& _) const
		{
			return _priority < _._priority;
		}

		bool operator >(const Node& _) const
		{
			return _priority > _._priority;
		}

		~Node()
		{

		}

};