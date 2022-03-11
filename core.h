#pragma once


namespace core
{

	enum heuristic_name
	{
		INSERT,
		INSERT_DP
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

}