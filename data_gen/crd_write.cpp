#include "cdr_write.h"
#include"random_iccid.h"
#include"../components/mcc_mcn.h"
#include"../components/sim.h"
#include<algorithm>
#include"../util/rng.h"
#include<fstream>
#include<cassert>
#include"../util/time_utils.h"
#include"../util/object_with_unique_id.h"
#include"../components/enterprise.h"
#include"../components/group.h"
#include"../util/config.h"


namespace atmik
{
	namespace data_gen
	{
		namespace write_cdr
		{
			std::vector<std::string> iccid_vec()
			{
				std::vector<std::string> iccids;
				for (auto& [iccid, s] : atmik::alerter::sim::objects())
				{
					iccids.push_back(iccid);
				}
				return iccids;
			}
			
			std::vector<iccid_mcc_mcn_data> select_sims(int pct_of_sims) 
			{
				static auto iccids = iccid_vec();
				const auto N = iccids.size() * pct_of_sims / 100;

				std::shuffle(iccids.begin(), iccids.end(), atmik::util::rng);
				std::vector<iccid_mcc_mcn_data> result;
				for (std::size_t i = 0; i < N; ++i)
				{
					auto& iccid = iccids[i];
					int mcc = alerter::random_mcc();
					int mcn = alerter::random_mcn(mcc);
					auto& imsi = atmik::alerter::sim::object_with_unique_id::look_up(iccid).imei;
					result.push_back(iccid_mcc_mcn_data{ iccid, imsi, mcc, mcn });
				}
				return result;
			}

			iccid_mcc_mcn_data& random_sim(std::vector<iccid_mcc_mcn_data>& selected_sims) 
			{
				return selected_sims[atmik::util::random_size_t(0, selected_sims.size() - 1)];
			}

			void write_to_file_cdr(int min_percent_sims, int max_percent_sims)
			{
				assert( !alerter::enterprise::objects().empty());
				assert(!alerter::group::objects().empty());
				assert(!alerter::sim::objects().empty());

				auto sims = select_sims(atmik::util::random_int(min_percent_sims, max_percent_sims));

				static const std::string cdr_file =
					atmik::config::get<std::string>("alerter.data_files.root_location", "C:/cdr/data/") +
					atmik::config::get<std::string>("alerter.data_files.cdr", "cdr.txt");

			
				std::ofstream ostm(cdr_file);

				assert(ostm.is_open());
				static int cdrno = 1000000;
				std::tm tm{};
				tm.tm_year = 120;
				tm.tm_mon = 3;
				tm.tm_mday = 1;
				tm.tm_hour = 10;
				tm.tm_min = 30;
				int i = 0;
				int days = atmik::config::get<int>("alerter.data_gen.cdr_write.days", 8);
				const int nsecs = 60 * 60 * 24 * days;
				std::cout << "days: " << days << " (" << nsecs << " seconds)\n";
				const int min_cdrs_per_sec = atmik::config::get<int>("alerter.data_gen.cdr_write.min_cdrs_per_sec", 5);
				const int max_cdrs_per_sec = atmik::config::get<int>("alerter.data_gen.cdr_write.max_cdrs_per_sec", 25);
				const int min_bytes = atmik::config::get<int>("alerter.data_gen.cdr_write.min_bytes",5000);
				const int max_bytes = atmik::config::get<int>("alerter.data_gen.cdr_write.max_bytes", 25000);
				unsigned long long cnt = 0;

				for (int s = 0; s < nsecs; ++s)
				{
					const auto ts = atmik::util::time_stamp(tm, s);
					const int ncdrs = atmik::util::random_size_t(min_cdrs_per_sec, max_cdrs_per_sec);
					for (int i = 0; i < ncdrs; ++i)
					{
						if (s % 300 == 299)
						{
							auto esims = select_sims(atmik::util::random_int(0, 2));

							if (sims.size() > esims.size())
								sims.resize(sims.size() - esims.size());
							else sims.clear();
							sims.insert(sims.end(), esims.begin(), esims.end());
							assert(!sims.empty());

							// remove duplicate sims
							std::sort(sims.begin(), sims.end());
							auto last = std::unique(sims.begin(), sims.end());
							sims.erase(last, sims.end());
						}

						const auto& [iccid, imsi, mcc, mcn] = random_sim(sims);
						int bytes = int(atmik::util::random_int(min_bytes, max_bytes));
						int total_bytes = 0;
						int upload_bytes = 0;
						int download_bytes = 0;
						if (atmik::util::random_int(0, 1)) total_bytes = bytes * 2;
						else
						{
							download_bytes = int( bytes * util::random_int(60,80) / 100.0 ) ;
							upload_bytes = bytes - download_bytes;
						}

						ostm << ++cdrno << ',' << ts << ',' << iccid << ','
							<< "type,usage_type,traffic_type," << mcc << ','
							<< mcn << ',' << total_bytes << ",start_time,duration,carrierAC,"
							<< imsi
							<< ",IMEI,MSISDN,"
							<< download_bytes << ',' << upload_bytes << '\n';

						if (cnt % 1'000'000 == 999'999)
						{
							std::cout << cnt + 1
								<< " cdrs (for " << s/(60*60.0) << " hours) written\n";
						}
						++cnt;
					}
				}
				std::cout << cdrno - 1000000 << " cdrs were written, last cdr was #" << cdrno << '\n';
			}
		}
	}
}