#include "group.h"
#include "enterprise.h"
#include <iomanip>
#include <fstream>

atmik::alerter::group::group(unsigned long long id, const std::string& name, enterprise& e)
	: base(id), name_(name), eprise(e) 
{
}

std::ostream& atmik::alerter::operator<< (std::ostream& stm, const atmik::alerter::group& gp)
{
	return stm << gp.id() << ' ' << gp.eprise.id()
		<< ' ' << gp.usage << ' ' << std::quoted(gp.name()) ;
}

bool atmik::alerter::group::save(const std::string& group_file)
{
	if (std::ofstream file{ group_file })
	{
		for (const auto& [k,g] : objects() ) file << g << '\n';
		return !objects().empty() ;
	}

	else return false;
}

bool atmik::alerter::group::load(const std::string& group_file)
{
	std::size_t ngroups = 0;

	if (std::ifstream file{ group_file })
	{
		objects().clear();

		unsigned long long grp_id, enterprise_id ;
		usage_info ui;
		std::string dummy, grp_name;
		while ( file >> grp_id >> enterprise_id >> ui &&
			    std::getline(file, dummy, '"' ) && std::getline(file, grp_name, '"') )
		{
			if (group::exists(grp_id)) group::erase(grp_id);
			group& g = group::create( grp_id, grp_name, enterprise::look_up(enterprise_id));
			g.usage = ui ;
			++ngroups;
		}
	}

	return ngroups > 0;
}
