#include "enterprise.h"
#include <iomanip>
#include<fstream>
std::ostream& atmik::alerter::operator<< (std::ostream& stm, const enterprise& ep)
{
	return stm << ep.id() << ' ' << ep.usage << ' ' << std::quoted(ep.name());
}

bool atmik::alerter::enterprise::save(const std::string& eprise_file)
{
	if (std::ofstream file{ eprise_file })
	{
		for (const auto& [k, e] : objects()) file << e << '\n';
		return !objects().empty();
	}

	else return false;
}

bool atmik::alerter::enterprise::load(const std::string& group_file)
{
	std::size_t neprises = 0;

	if (std::ifstream file{ group_file })
	{
		objects().clear();

		unsigned long long eprise_id;
		usage_info  ui;
		std::string dummy, eprise_name;
		while (file >> eprise_id  >> ui &&
			std::getline(file, dummy, '"') && std::getline(file, eprise_name, '"'))
		{
			if (enterprise::exists(eprise_id)) enterprise::erase(eprise_id);
			enterprise& e = enterprise::create(eprise_id, eprise_name);
			e.usage = ui ;
			++neprises;
		}
	}

	return neprises > 0;
}