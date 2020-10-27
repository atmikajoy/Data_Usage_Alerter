#ifndef ZONE_H_INCLUDED
#define ZONE_H_INCLUDED

#include "../util/object_with_unique_id.h"
#include "usage.h"
#include<iostream>
#include <vector>

namespace atmik
{
	namespace alerter
	{
		struct zone : atmik::util::object_with_unique_id<zone>
		{
			using base = atmik::util::object_with_unique_id<zone>;

			const std::string& name() const { return name_; }

			static zone& create(unsigned long long id, const std::string& name )
			{
				return base::create(id, name );
			}

			void add_mcc_mcn(int mcc, int mcn);

			static zone& look_up_mcc_mcn(int mcc, int mcn) ;

			static 	void add_usage(int mcc, int mcn, unsigned long long amt )
			{
				look_up_mcc_mcn(mcc, mcn).usage.add_usage(amt);
			}

			std::vector< std::pair<int, int> > dump_mcc_mcn() const;

			usage_info usage;
			std::string name_;
			unsigned long long temp_amt;

			// do not use this constructor directly
			// instead, use the static function create to create groups
			zone(unsigned long long id, const std::string& name );

			static bool save(const std::string& sim_file);
			static bool load(const std::string& sim_file);
		};

		std::ostream& operator<< (std::ostream& stm, const zone& gp);
	}
}

#endif // ZONE_H_INCLUDED
