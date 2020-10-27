#include <iostream>
#include "random_iccid.h"
#include "../components/enterprise.h"
#include "../components/group.h"
#include "../components/sim.h"
#include"../components/zone.h"
#include"../components/mcc_mcn.h"
#include"cdr_write.h"
#include <fstream>
#include"data_load.h"
#include"../util/rng.h"
#include<sstream>
#include "../util/config.h"

std::string root_directory()
{
	static const std::string rd = 
		atmik::config::get<std::string>( "alerter.data_files.root_location", "C:/cdr/data/" );
	
	return rd;
}

void create_random_zones(unsigned int nzones = 5)
{
	unsigned long long zone_id = 0;
	while (zone_id < nzones)
	{
		++zone_id;
		atmik::alerter::zone::create(zone_id, "zone #" + std::to_string(zone_id));
	}

	static const std::string data_file_fname = 
		atmik::config::get<std::string>("alerter.data_files.mcc_mcn","mcc_mcn.txt");

	static const std::string data_file = root_directory() + data_file_fname;

	if (std::ifstream file{ data_file })
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream stm(line);
			int mcc;
			if (stm >> mcc)
			{
				int mnc;
				while (stm >> mnc)
				{
					auto& z = atmik::alerter::zone::look_up(atmik::util::random_int(1, nzones));
					z.add_mcc_mcn(mcc, mnc);
				}
			}

			for (auto& [k, z] : atmik::alerter::zone::objects())
			{
				z.usage.daily.set_limits(1000000000, 70);
				z.usage.monthly.set_limits(1000000000, 70);
			}
		}
		//atmik::alerter::zone::save("C:\\cdr\\log\\temp_zone_file.txt");
	}
}
		
int main()
{
	using namespace atmik;

	std::cout << "\n---------------------ENTERPRISES-------------------\n";
	data_gen::random::create_enterprises(5);
	{
		static const std::string eprise_file_fname =
			atmik::config::get<std::string>("alerter.data_files.enterprise", "enterprise_file.txt");

		static const std::string eprise_file = root_directory() + eprise_file_fname;

		alerter::enterprise::save(eprise_file);
		std::cout << "\n\nsaved enterprise data\n--------------\n"
			<< std::ifstream(eprise_file).rdbuf()
			<< "\n---------------------------\n";
	}

	std::cout << "\n---------------------GROUPS-----------------------\n";
	data_gen::random::create_groups();
	{
		static const std::string group_file_fname =
			atmik::config::get<std::string>("alerter.data_files.group", "group_file.txt");

		static const std::string grp_file = root_directory() + group_file_fname;

		alerter::group::save(grp_file);
		std::cout << "\n\nsaved group data\n--------------\n"
			<< std::ifstream(grp_file).rdbuf()
			<< "\n---------------------------\n";
	}

	std::cout << "\n --------------------SIMS--------------------\n";
	data_gen::random::create_random_sims(data_gen::random::iccids(100'000));
	{
		static const std::string sim_file_fname =
			atmik::config::get<std::string>("alerter.data_files.sim", "sim_file.txt");

		static const std::string sim_file = root_directory() + sim_file_fname;

		alerter::sim::save(sim_file);
		std::cout << "\n\nsaved sim data for " 
			      << alerter::sim::objects().size() << " sims\n";
	}

	std::cout << "\n ------------------ZONES-----------------------\n";
	create_random_zones();
	{
		static const std::string zone_file_fname =
			atmik::config::get<std::string>("alerter.data_files.zone", "zone_file.txt");

		static const std::string zone_file = root_directory() + zone_file_fname;

		alerter::zone::save(zone_file);
		std::cout << "\n\nsaved zone data\n--------------\n"
			<< std::ifstream(zone_file).rdbuf()
			<< "\n---------------------------\n";
	}

	std::cout << "\n------------------------Writing cdrs-----------------\n";
	atmik::data_gen::write_cdr::write_to_file_cdr(5, 8);

}