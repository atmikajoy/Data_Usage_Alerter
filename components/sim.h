#ifndef SIM_H_INCLUDED
#define SIM_H_INCLUDED

#include "../util/object_with_unique_id.h"
#include "usage.h"
#include <string>
#include <iostream>
#include "group.h"
#include <set>

namespace atmik
{
	namespace alerter
	{
		struct group;

		struct sim : atmik::util::object_with_unique_id<sim,std::string>
		{
			using base = atmik::util::object_with_unique_id<sim, std::string>;

			static sim& create( const std::string& iccid, const std::string& imei, group& gp)
			{
				return base::create(iccid,imei,gp);
			}

			usage_info usage;
			group& grp;
			const std::string imei;
			unsigned long long temp_amt = 0 ;

			void update_usage()
			{
				add_usage(temp_amt);
				temp_amt = 0;
			}

			void add_usage(unsigned long long amt)
			{
				usage.add_usage(amt);
				grp.add_usage(amt);
			}

			// do not use this constructor directly
			// instead, use the static function create to create sims
			sim(const std::string& iccid, const std::string& imei, group& gp );

			static bool save(const std::string& sim_file);
			static bool load(const std::string& sim_file);

			static std::set<sim*> changed_sims;
		};

		std::ostream& operator<< (std::ostream& stm, const sim& s);

		
	}
}

#endif // SIM_H_INCLUDED
