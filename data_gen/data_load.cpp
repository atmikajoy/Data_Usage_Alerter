#include"data_load.h"
#include"cdr_write.h"
#include "../util/rng.h"
#include "../components/enterprise.h"
#include "../components/group.h"
#include "../components/sim.h"
#include "../components/zone.h"
#include<fstream>
#include<iostream>
#include<sstream>

namespace atmik
{
	namespace data_gen
	{
	
			
		data_load::data_load()
		{
			alerter::enterprise::load("C:\\cdr\\log\\temp_eprise_file.txt");
			alerter::group::load("C:\\cdr\\log\\temp_grp_file.txt");
			alerter::sim::load("C:\\cdr\\log\\temp_sim_file.txt");
			alerter::zone::load("C:\\cdr\\log\\temp_zone_file.txt");



		}
	}
}