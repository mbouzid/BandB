#include <cstdlib>
#include <chrono>
#include <ctime>
#include "solver/solver.h"
//#include <filesystem>
#include "solver/solution.h"

int main(int argc, char* argv[])
{
    if (argc < 2){
        std::cerr << "An argument is required" << std::endl;
        exit(EXIT_FAILURE);
    }




    omp_set_num_threads(8);
    try{
        pthread_t thid;

		srand(161295);
		const char* datname(argv[1]);

		Instance* dat(Instance::load(datname));

        /*std::filesystem::path f{ datname };
        if (not std::filesystem::exists(f)){
            std::cout << "File " << datname << " does not exist" << std::endl;
            return EXIT_FAILURE;
        }*/


		char delim(';');

        Solver 	solver(dat);

        auto start = std::chrono::system_clock::now();
        t_arg args =  { &solver, false};

        pthread_create(&thid, NULL, (&runWrapper), (void*)(&args));

        while(not args._hasResult && time(0) < std::chrono::system_clock::to_time_t(start) + MAXTIME){
        }

        dat->printCharacteristics(std::cout,delim);
        std::cout << delim << solver.getBestSolution()->getProfit(dat);
        std::cout << delim << solver.getBestSolution()->getTotalImpact(dat);
//        std::cout << delim << *solver.getBestSolution() ;

        if(not args._hasResult){
            std::cout << delim << "nOPT" ;
        }
        else{
            std::cout << delim << "OPT" ;
        }


        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << delim << elapsed_seconds.count() << std::endl;



        delete dat;
	}
	catch (const std::exception& e){
		std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}