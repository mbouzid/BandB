#pragma once

#include <vector>
#include <set>
#include <string>
#include <random>
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define PMAX 10
#define TMAX 96//1440


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

	 std::vector<std::string> extractPattern(const std::string & );

	 std::string getBaseName(const std::string& path);

	 namespace datfile
	 {
		 enum e_corrLevel
		 {
			 NO,
			 WEAKLY,
			 STRONGLY,
			 UNKNOWN
		 };
		 typedef enum e_corrLevel e_corrLevel;

		 struct s_characteristics
		 {
			 uint16_t n;
			 uint16_t dU;
			 uint16_t dL;
			 e_corrLevel corrLevel;
			 uint16_t numInst;

		 };
		 typedef struct s_characteristics s_characteristics;

		 s_characteristics * getCharacteristicsFromName(const std::string & filename);
		 e_corrLevel toCorrLevel(const std::string& corrLevel);

		 std::string fromCorrLevel(e_corrLevel corrLevel);

	 }

}
