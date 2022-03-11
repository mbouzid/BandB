#include "milp.h"

const char* milp::s_params = "modeles\\relaxed_v4.mod";

uint16_t milp::run()
{
	IloEnv env;
	IloOplRunConfiguration rc(loadRC(env));

	IloOplModel opl(rc.getOplModel());
	
	//opl.getModel().add(_profit == opl.getElement("profit").asNumVar());



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


	 //IloInt test(cplx.getValue(_profit));

	/*std::ostringstream oss;
	oss << "sol_" << _numThread << ".sol";
	std::string name(oss.str());
	cplx.writeSolution(name.c_str()); */

	IloInt obj(IloRound(cplx.getObjValue()));


	cplx.end();
	env.end();

	
	return obj;
	//return test;
}

void milp::createTmpDat()
{	
	
	std::ofstream f(tmpDatName());


	f << "candidates={";
	if (not _orders.empty())
	{
		f << *_orders.begin();
		for (auto i(std::next(_orders.begin())); i != _orders.end(); ++i)
		{
			f << "," << (*i);
		}
	}
	f << "};" << std::endl;

	f << "tinit=" << _t << ";" << std::endl;
	f << "Tmax=" << _T << ";" << std::endl;

	std::ifstream ff(_datfile);
	std::string line;
	while (std::getline(ff, line))
	{
		f << line << std::endl;
	}
	ff.close();


	f.close();

}

IloOplRunConfiguration milp::loadRC(IloEnv& env)
{
	IloOplErrorHandler errHdlr(env, std::cerr);
	IloOplSettings settings(env, errHdlr);
	IloOplModelSource src(env, milp::s_params);

	IloOplModelDefinition def(src, settings);


	createTmpDat();

	IloOplDataSource dat(env, tmpDatName().c_str());


	_profit = IloIntVar(env);

	IloOplDataElements elts(def, dat);
	

	IloOplRunConfiguration rc(def, elts);
	
	rc.getOplModel().generate();


	return rc;
}