#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include "../util/object_with_unique_id.h"
#include "usage.h"
#include<iostream>
#include "enterprise.h"

namespace atmik
{
	namespace alerter
	{
		struct enterprise;

		struct group : atmik::util::object_with_unique_id<group>
		{
			using base = atmik::util::object_with_unique_id<group>;

			const std::string& name() const { return name_;  }

			static group& create(unsigned long long id, const std::string& name, enterprise& e )
			{
				return base::create(id, name, e);
			}

			usage_info usage;
			std::string name_;
			enterprise& eprise;

			// do not use this constructor directly
			// instead, use the static function create to create groups
			group(unsigned long long id, const std::string& name, enterprise& e);

			void add_usage(unsigned long long amt)
			{
				usage.add_usage(amt);
				eprise.add_usage(amt);
			}

			static bool save(const std::string& group_file);
			static bool load(const std::string& group_file);

			std::ostream& make_snapshot(std::ostream& stm) const;
			static void snap_all(std::ostream& stm);
			static void load_snapshot(std::istream& stm);
		};

		std::ostream& operator<< (std::ostream& stm, const group& gp);
	}
}

#endif // GROUP_H_INCLUDED
