#include "milp.h"


uint16_t run(const char * datfile, int numThread, const std::vector<uint16_t>& orders, uint16_t t, uint16_t T)
{
	IloEnv env;
	IloOplRunConfiguration rc(loadRC(env, datfile,numThread, orders,t,T));

	IloOplModel opl(rc.getOplModel());



	IloCplex cplx(opl.getCplex());
	cplx.setParam(IloCplex::RootAlg, IloCplex::Primal);
	cplx.setOut(env.getNullStream());

	
	uint16_t profit(0);
	if (cplx.solve())
	{
		profit =  IloRound(cplx.getObjValue()) ;
	}
	else
	{
		std::cerr << "problem with CPLEX" << std::endl;
	}

	cplx.end();
	env.end();

	return profit;
}

void createTmpDat(const char* datfile,const char* tmpdatfile, const std::vector<uint16_t>& orders,  uint16_t tinit, uint16_t T)
{	
	
	std::ofstream f(tmpdatfile);


	f << "candidates={";
	if (not orders.empty())
	{
		f << *orders.begin();
		for (auto i(std::next(orders.begin())); i != orders.end(); ++i)
		{
			f << "," << (*i);
		}
	}
	f << "};" << std::endl;

	f << "tinit=" << tinit << ";" << std::endl;
	f << "Tmax=" << T << ";" << std::endl;

	std::ifstream ff(datfile);
	std::string line;
	while (std::getline(ff, line))
	{
		f << line << std::endl;
	}
	ff.close();


	f.close();

}

IloOplRunConfiguration loadRC(IloEnv& env, const char* datfile, int numThread, const std::vector<uint16_t>& orders, uint16_t t, uint16_t T)
{
	IloOplErrorHandler errHdlr(env, std::cerr);
	IloOplSettings settings(env, errHdlr);
	IloOplModelSource src(env, milp::s_params);

	IloOplModelDefinition def(src, settings);


	std::ostringstream oss;
	oss << "tmp_" << (uint16_t)numThread << ".dat";

	std::string name(oss.str());
	createTmpDat(datfile,name.c_str(), orders, t, T);

	IloOplDataSource dat(env, name.c_str());


	IloOplDataElements elts(def, dat);
	
	IloOplRunConfiguration rc(def, elts);
	rc.getOplModel().generate();


	return rc;
}