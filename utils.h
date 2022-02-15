#pragma once

#include <vector>
#include <set>
#include <string>
#include <random>
#include <map>
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define PMAX 10

namespace utils
{

	enum OptionsParams
	{
		Invalid,
		OrderNb,
		ProcessingTime,
		DueDate,
		Weight,
		Energy,
		EnergyPeriods,
	};

	typedef std::map<uint16_t, std::map<uint16_t, uint16_t>> Matrix;

	std::set<uint16_t> 	emptySet();
	std::vector<uint16_t> emptyVector();

	std::vector <std::string> tokenize(const std::string& str, const std::string& delims);

	// martinbroadhurst.com
	std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

	OptionsParams resolveOption(const std::string input);

	template<class T> std::ostream& print(std::ostream& os, T* array, size_t n);
	template<class T> std::ostream& print(std::ostream& os, const std::vector<T>&);
	template<class T> std::ostream& print(std::ostream& os, const std::set<T>&);



	void cleanVector(std::vector <std::string>& v);
}