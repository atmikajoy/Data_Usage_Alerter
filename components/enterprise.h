#ifndef ENTERPRISE_H_INCLUDED
#define ENTERPRISE_H_INCLUDED

#include "../util/object_with_unique_id.h"
#include "usage.h"
#include<iostream>

namespace atmik
{
	namespace alerter
	{
		struct enterprise : atmik::util::object_with_unique_id<enterprise>
		{
			using base = atmik::util::object_with_unique_id<enterprise>;

			const std::string& name() const { return name_; }

			static enterprise& create(unsigned long long id, const std::string& name )
			{
				return base::create(id, name );
			}

			usage_info usage;
			std::string name_;

			void add_usage(unsigned long long amt)
			{
				usage.add_usage(amt);
			}

			// do not use this constructor directly
			// instead, use the static function create to create enterprises
			enterprise(unsigned long long id, const std::string& name ) 
				: base(id), name_(name) {}

			static bool save(const std::string& group_file);
			static bool load(const std::string& group_file);
		};



		std::ostream& operator<< (std::ostream& stm, const enterprise& gp);
	}
}

#endif // ENTERPRISE_H_INCLUDED
