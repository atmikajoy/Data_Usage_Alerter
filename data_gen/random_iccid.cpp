#include "random_iccid.h"
#include "../util/rng.h"
#include <cassert>
#include <algorithm>
#include "../components/enterprise.h"
#include "../components/group.h"
#include "../components/sim.h"
#include"../util/config.h"

namespace atmik
{
	namespace data_gen
	{
		namespace random
		{
			namespace
			{
				std::string to_string(std::size_t iccid_n, std::size_t min_length )
				{
					auto str = std::to_string(iccid_n);
					while (str.size() < min_length) str = '0' + str;
					return str;
				}

				std::vector<unsigned long long> group_list()
				{
					std::vector<unsigned long long> grpids;

					for (const auto& [k, g] : atmik::alerter::group::objects())
						grpids.push_back(k);

					assert(!grpids.empty());

					return grpids;
				}

				atmik::alerter::group& random_group()
				{
					static const auto ids = group_list();
					static const auto n = ids.size();

					const auto rand_id = ids[atmik::util::random_size_t(0, n-1)];
					return atmik::alerter::group::look_up(rand_id);
				}

			}

			std::vector<std::string> iccids(std::size_t n,
				const std::string& iccid_prefix, std::size_t iccid_n_start)
			{
				std::vector<std::string> result;

				static constexpr std::size_t ICCID_LEN = 18;
				assert(iccid_prefix.size() < ICCID_LEN);
				const std::size_t len = ICCID_LEN - iccid_prefix.size();

				auto s = iccid_n_start;
				while (result.size() < n)
					result.push_back(iccid_prefix + to_string(s++, len));

				std::shuffle(result.begin(), result.end(), atmik::util::rng);

				return result;

			}

			namespace
			{

				unsigned int random_alert_pct()
				{
					return util::random_int(1, 5) == 1 ?
						util::random_int(70, 120) : 0;
				}

				constexpr unsigned long long bytes_per_mb = 1024 * 1024ULL;

				unsigned long long random_limit(unsigned int min_mb, unsigned int max_mb)
				{
					return util::random_int(min_mb * bytes_per_mb, max_mb * bytes_per_mb);
				}

				template < typename ITEM >
				void set_random_limits( ITEM& it, unsigned int min_mb, unsigned int max_mb)
				{
					int daily_alert_pct = random_alert_pct();
					unsigned long long daily_limit = random_limit(min_mb, max_mb);

					if (daily_alert_pct != 0)
					{
						it.usage.daily.set_limits(daily_limit, daily_alert_pct);
					}
						

					int monthly_alert_pct = random_alert_pct();
					if (monthly_alert_pct != 0)
					{
						it.usage.monthly.set_limits(
							daily_limit * util::random_int(15, 35), daily_alert_pct);
					}
				}
			}


			void create_enterprises(std::size_t n, unsigned long long id )
			{
				const std::string name_prefix = "enterprise #";
				
				for (std::size_t i = 0; i < n; ++i)
				{
					auto& e = atmik::alerter::enterprise::create(id, name_prefix + std::to_string(id));
					unsigned int min_eprise =
						atmik::config::get<unsigned int>("alerter.data_gen.enterprise.min", 50);
					unsigned int max_eprise =
						atmik::config::get<unsigned int>("alerter.data_gen.enterprise.max", 800);
					set_random_limits(e, min_eprise, max_eprise);
					++id;
				}
			}

			void create_groups(unsigned long long id,
				std::size_t max_groups_per_enterprise)
			{
				const std::string name_prefix = "group #";

				for (auto& [k,e] : atmik::alerter::enterprise::objects())
				{
					const auto n = atmik::util::random_int(1, max_groups_per_enterprise);
					for (std::size_t i = 0; i < n; ++i)
					{
						auto& g = atmik::alerter::group::create(id, name_prefix + std::to_string(id), e);
						unsigned int min_grp =
							atmik::config::get<unsigned int>("alerter.data_gen.group.min", 35);
						unsigned int max_grp =
							atmik::config::get<unsigned int>("alerter.data_gen.group.max", 100);

						set_random_limits(g, min_grp, max_grp);
						++id;
					}
				}
			}

			
			void create_random_sims(const std::vector<std::string>& iccids)
			{
				// dummy imsei numbers for testing
				static const unsigned long long IMSI_START = 900'000'000'000'000;

				auto imsi = IMSI_START;
				for (const auto& iccid : iccids)
				{
					alerter::sim& s = atmik::alerter::sim::create(iccid, std::to_string(imsi++), random_group());
					unsigned int min_sim =
						atmik::config::get<unsigned int>("alerter.data_gen.sim.min", 1);
					unsigned int max_sim =
						atmik::config::get<unsigned int>("alerter.data_gen.sim.max", 2);

					set_random_limits(s,min_sim,max_sim);
				}

				const auto nsins = atmik::alerter::sim::objects().size();
			}
			
		}
	}
}