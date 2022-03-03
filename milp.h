#pragma once
#include <ilopl/iloopl.h>
#include <vector>


namespace milp
{
	static const char* s_params = "relaxed.mod";
};



uint16_t run(const char* datfile, int numThread, const std::vector<uint16_t> & orders, uint16_t t, uint16_t T);
void createTmpDat(const char* datfile, const char* tmpdatfile, const std::vector<uint16_t>& orders, uint16_t tinit, uint16_t T);
IloOplRunConfiguration loadRC(IloEnv& env, const char* datfile, int numThread, const std::vector<uint16_t>& orders, uint16_t t, uint16_t T);