#include <cstdlib>
#include <chrono>
#include <ctime>
#include "solver.h"
#include <filesystem>
int main(int argc, char* argv[])
{
	try
	{
		srand(161295);
		const char* datname(argv[1]);

		Instance* dat(Instance::load(datname));

        std::filesystem::path f{ datname };
        if (not std::filesystem::exists(f))
        {
            std::cout << "File " << datname << " does not exist" << std::endl;
            exit(EXIT_FAILURE);
        }


		char delim(';');
		Solver 	solver(dat);



		auto start = std::chrono::system_clock::now();
		solver.run();
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);

		std::cout << delim <<  elapsed_seconds.count() << std::endl;



		delete dat;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}