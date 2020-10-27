#include "zone.h"
#include<iomanip>
#include <map>
#include <fstream>

namespace atmik
{
	namespace alerter
	{

		namespace
		{
			static constexpr unsigned long long multiplier = 1'000'000ULL;

			unsigned long long make_key( int mcc, int mcn )
			{
				return mcc * multiplier + mcn;
			}

			std::pair<int, int> split_key(unsigned long long key)
			{
				return { int(key/multiplier), int(key%multiplier) };
			}

			std::map< unsigned long long, zone* > mcc_mcn_to_zone_map;

		}

		zone::zone(unsigned long long id, const std::string& name)
			: base(id), name_(name) {}

		void zone::add_mcc_mcn(int mcc, int mcn)
		{
			mcc_mcn_to_zone_map[make_key(mcc, mcn)] = this;
		}

		zone& zone::look_up_mcc_mcn(int mcc, int mcn)
		{
			static zone dummy_zone(9999999, "no zone data");
			const auto iter = mcc_mcn_to_zone_map.find(make_key(mcc, mcn));
			return iter != mcc_mcn_to_zone_map.end() ? *iter->second : dummy_zone;
		}

		std::ostream& operator<< (std::ostream& stm, const zone& z)
		{
			return stm << "Zone ID: " << z.id()
				<< " Zone Name: " << std::quoted(z.name());
		}

		std::vector< std::pair<int, int> > zone::dump_mcc_mcn() const
		{
			std::vector < std::pair<int, int> > result;

			for (const auto& [k, pz] : mcc_mcn_to_zone_map)
				if (pz == this) result.push_back(split_key(k));

			return result;
		}

		bool zone::save(const std::string& zone_file)
		{
			if (std::ofstream file{ zone_file })
			{
				// write number of zones
				// and then write contents of each zone
				file << objects().size() << '\n';
				for (auto& [k, z] : objects())
					file << z.id() << ' ' << z.usage  << ' ' << z.name() << '\n';

				// write the contents of the mcc_mcn_to_zone_map
				for (const auto& [k, pz] : mcc_mcn_to_zone_map)
					file << k << ' '  << pz->id() << '\n';
                
				return !objects().empty();
			}
			
			return false;
		}

		bool zone::load(const std::string& zone_file)
		{
			if (std::ifstream file{ zone_file })
			{
				objects().clear();

				// read the number of zones
				std::size_t num_zones;
				file >> num_zones;

				// now read info about num_zones zones
				unsigned long long id;
				usage_info ui;
				std::string name;

				while (num_zones > 0 && file >> id >> ui && std::getline(file, name))
				{
					if (zone::exists(id)) zone::erase(id);
					zone& z = zone::create(id, name);
					z.usage = ui;
					--num_zones;
				}

				mcc_mcn_to_zone_map.clear();

				// read the mcc_mcn and zone id
				unsigned long long mcc_mcn_key;
				unsigned long long zone_id;
				while (file >> mcc_mcn_key >> zone_id)
				{
					auto [mcc, mcn] = split_key(mcc_mcn_key);
					look_up(zone_id).add_mcc_mcn(mcc, mcn);
				}

				return true;
			}

			else return false;
		}

	}
}