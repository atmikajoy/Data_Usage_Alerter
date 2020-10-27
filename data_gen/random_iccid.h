#ifndef RANDOM_ICCID_H_INCLUDED
#define RANDOM_ICCID_H_INCLUDED

#include <string>
#include <vector>

namespace atmik
{
	namespace data_gen
	{
		namespace random
		{
			std::vector<std::string> iccids(std::size_t n,
				const std::string& iccid_prefix = "899101000000",
				std::size_t ICCID_N_START = 100'000);

			void create_enterprises(std::size_t n, unsigned long long start_id = 1);

			void create_groups( unsigned long long start_id = 100, 
				                std::size_t max_groups_per_enterprise = 10 );

			void create_random_sims( const std::vector<std::string>& iccids );

		}
	}
}

#endif // RANDOM_ICCID_H_INCLUDED
