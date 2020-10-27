#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED

#include "../components/cdr.h"
#include<unordered_map>
#include "../components/sim.h"

namespace atmik
{
	namespace alerter
	{
		bool process(const cdr& c);
		bool process(std::istream& cdr_stm);
		bool process( const std::string& cdr_file);

		unsigned long long num_cdrs_processed();
		void reset_num_cdrs_processed();

		
	}
}

#endif //PROCESS_H_INCLUDED