#pragma once
#include <ilopl/iloopl.h>
#include <vector>


class milp
{
	static const char* s_params ;

	private:
		
		int _numThread;
		const char* _datfile;
		uint16_t _t;
		uint16_t _T;
		std::vector<uint16_t> _orders;
		IloIntVar _profit;

	protected:
		
		IloOplRunConfiguration loadRC(IloEnv& env);
		
		std::string tmpDatName() const
		{
			std::ostringstream oss;
			oss << "tmp_" << _numThread << ".dat";
			std::string name(oss.str());
			return name;
		}
		void createTmpDat();
		
	public:

		milp
		(
			int numThread,
			const char * datfile,
			uint16_t t,
			uint16_t T,
			const std::vector<uint16_t> & orders
		):
			_numThread(numThread),
			_datfile(datfile),
			_t(t),
			_T(T),
			_orders(orders)
		{}

		uint16_t run();


};

