#include <cstdlib>
#include "instance.h"

int main(int argc, char* argv[])
{
	try
	{
		srand(161295);
		const char* datname("15orders_dL_20_dU_40_no_0.dat");

		Instance* dat(Instance::load(datname));

		delete dat;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}