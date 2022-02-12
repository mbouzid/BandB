#include "utils.h"
#include <fstream>
#include <algorithm>
#include <numeric>


std::vector<std::string> utils::tokenize(const std::string& str, const std::string& delims)
{

	std::vector<std::string> out;
	size_t beg, pos = 0;
	while ((beg = str.find_first_not_of(delims, pos)) != std::string::npos)
	{
		pos = str.find_first_of(delims, beg + 1);
		out.push_back(str.substr(beg, pos - beg));
	}

	return out;
}

std::string& utils::ltrim(std::string& str, const std::string& chars)
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

std::string& utils::rtrim(std::string& str, const std::string& chars)
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

std::string& utils::trim(std::string& str, const std::string& chars)
{
	return ltrim(rtrim(str, chars), chars);
}


utils::OptionsParams utils::resolveOption(const std::string input)
{
	if (input == "n")
		return OptionsParams::OrderNb;
	if (input == "p")
		return OptionsParams::ProcessingTime;
	if (input == "d")
		return OptionsParams::DueDate;
	if (input == "w")
		return OptionsParams::Weight;
	if (input == "e")
		return OptionsParams::Energy;
	if (input == "E")
		return OptionsParams::EnergyPeriods;
	else
		return OptionsParams::Invalid;
}


void utils::cleanVector(std::vector<std::string>& v)
{

	auto is_whitespace = [](const std::string& str)
	{
		return std::all_of(
			begin(str), end(str), [](unsigned char c) { return std::isspace(c); });
	};

	for (std::vector<std::string>::iterator it = v.begin(); it != v.end() && is_whitespace(*it); )
	{
		it = v.erase(it);
	}
}

template<class T>
std::ostream& utils::print(std::ostream& os, T* array, size_t n)
{
	os << "{";
	if (n >= 1)
	{
		os << array[0];
		for (size_t i(1); i < n; ++i)
		{
			os << "," << array[i];
		}
	}
	os << "}";
	return os;
}

template<class T>
std::ostream& utils::print(std::ostream& os, const std::vector<T>& v)
{
	for (const T& t : v)
	{
		os << t << " ";
	}
	return os;
	
}

template<class T>
std::ostream& utils::print(std::ostream& os, const std::set<T>& s)
{
	for (const T& t : s)
	{
		os << t << " ";
	}
	return os;

}


std::set<uint16_t> utils::emptySet()
{
	return std::set<uint16_t>();
}

std::vector<uint16_t> utils::emptyVector()
{
	return std::vector<uint16_t>();
}


template std::ostream& utils::print<uint16_t>(std::ostream& os, const std::vector<uint16_t>& v);
template std::ostream& utils::print<uint16_t>(std::ostream& os, const std::set<uint16_t>& v);