#include "sim.h"
#include <stdexcept>
#include "group.h"
#include <fstream>

atmik::alerter::sim::sim(const std::string& iccid, const std::string& imei, group& gp)
	: base(iccid), grp(gp), imei(imei) {}

std::set<atmik::alerter::sim*> atmik::alerter::sim::changed_sims;

std::ostream& atmik::alerter::operator<< (std::ostream& stm, const sim& s)
{
	return stm << s.id() << ' ' << s.imei << ' ' << s.grp.id() 
		       << ' ' << s.usage ;
}

bool atmik::alerter::sim::save(const std::string& sim_file)
{
	std::cout << "sim::save  file: " << sim_file << '\n';

	if(std::ofstream file{ sim_file })
	{
		auto& objs = objects();
		for (const auto& [k, s] : objects()) file << s << '\n';
		return true;
	}

	else return false;
}

bool atmik::alerter::sim::load(const std::string& sim_file)
{
	if (std::ifstream file{ sim_file })
	{
		objects().clear();

		std::string iccid, imsi;
		unsigned long long grp_id;
		usage_info ui;
		std::size_t nsims = 0;
		while (file >> iccid >> imsi >> grp_id >> ui )
		{
			if (sim::exists(iccid)) sim::erase(iccid);
			sim& s = sim::create(iccid, imsi, group::look_up(grp_id));
			s.usage = ui ;
			++nsims;
		}

		return nsims > 0;
	}

	else return false;

}

