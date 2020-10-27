
#define _CRT_SECURE_NO_WARNINGS
#include "process.h"
#include <fstream>
#include "../components/sim.h"
#include "../components/zone.h"
#include <unordered_set>
#include <set>
#include "../util/time_utils.h"
#include "alert_message.h"
#include "snap_shot.h"
#include "alert_recvr.h"
#include "..\util\config.h"
#include <ctime>

namespace atmik
{
	namespace alerter
	{
		namespace
		{
			unsigned long long ncdrs_processed = 0;
			

			void update_usage()
			{
				for (sim* s : sim::changed_sims) s->update_usage();
				//sim::changed_sims.clear();
			}
            
			const unsigned long long cdrs_per_sec = 
				atmik::config::get <unsigned long long>( "alerter.cdr.number_per_second", 200 );

			const unsigned int update_interval_minutes = 
				atmik::config::get<unsigned int>( "alerter.update.interval_minutes" ,5);
			
			const unsigned long long update_interval_ncdrs = 
				update_interval_minutes * 60ULL * cdrs_per_sec ;
			
			const unsigned long long update_interval_ncdrs_m1 =
				update_interval_ncdrs - 1;

			// this is an estimate based on actuak measurement
			const unsigned long long estimated_cdrs_processed_per_sec = 150'000;
			
			const unsigned long long snapshot_interval_minutes =
				atmik::config::get <unsigned long long>( "alerter.snapshot.interval_minutes", 2 ) ;
			
			const unsigned long long snapshot_interval_ncdrs =
				snapshot_interval_minutes * 60 * estimated_cdrs_processed_per_sec;
			
			const unsigned long long snapshot_interval_ncdrs_m1 =
				snapshot_interval_ncdrs - 1;
		}

		unsigned long long num_cdrs_processed() { return ncdrs_processed; }

		void reset_num_cdrs_processed() { ncdrs_processed = 0;  }

		namespace
		{
			int curr_day = 0;
			int curr_month = 0;

			void make_zero(atmik::alerter::usage_stats& us)
			{
				us.amount = 0;
				us.alert_pending = us.alert_sent = false;
			}

			template < typename ITEM >
			void do_start_new_day()
			{
				for (auto& [k, it] : ITEM::objects()) make_zero(it.usage.daily);
			}

			template < typename ITEM >
			void do_start_new_month()
			{
				for (auto& [k, it] : ITEM::objects())
				{
					make_zero(it.usage.daily);
					make_zero(it.usage.monthly);
				}
			}
			void start_new_day()
			{
				do_start_new_day<sim>();
				do_start_new_day<group>();
				do_start_new_day<enterprise>();
				do_start_new_day<zone>();
			}

			void start_new_month()
			{
				do_start_new_month<sim>();
				do_start_new_month<group>();
				do_start_new_month<enterprise>();
				do_start_new_month<zone>();

			}

		}

		bool process(const cdr& c)
		{
            #ifndef NDEBUG
			if (sim::exists(c.iccid))
#endif // NDEBUG 

			{
				sim& this_sim = sim::look_up(c.iccid);
				this_sim.temp_amt += c.total_bytes;

				sim::changed_sims.insert(std::addressof(this_sim));
				zone::add_usage(c.mcc, c.mnc, c.total_bytes);

				cdr::last_cdr_number_processed = c.cdr_number;

				const int day = atmik::util::day(c.time);
				if (day != curr_day)
				{
					const int month = atmik::util::month(c.time);
					if (month != curr_month) 
					{
						start_new_month();
						curr_month = month;
					}
					else start_new_day();

					curr_day = day;
				}

				++ncdrs_processed;

				if (ncdrs_processed % snapshot_interval_ncdrs == snapshot_interval_ncdrs_m1)
				{
					update_usage();
					sim::changed_sims.clear();
					atmik::alerter::save_snapshot();
				}

				else if (ncdrs_processed % update_interval_ncdrs == update_interval_ncdrs_m1)
				{
					update_usage();

					auto alerts = atmik::alerter::alert_msg::create_alerts( curr_day, curr_month );
					atmik::alerter::alert_recvr::send_alerts(alerts);

					sim::changed_sims.clear();
				}

			    return true;
			}

			// else (#ifndef NDEBUG)
			{
				std::cerr << "*** error *** cdr #" << c.cdr_number
					<< "   invalid iccid " << c.iccid << '\n';
				const auto nsims = sim::objects().size();
				auto& sims = sim::objects();
				__asm { int 3 };
				return false;
			}
			
		}

		namespace
		{
			// skip cdrs and return the first cdr line to be processed
			// return an empty string if eof is reached on input
			std::string get_first_cdr_line(std::istream& stm)
			{
				std::string line;
				unsigned long long cnt = 0;

				if (cdr::last_cdr_number_processed)
				{
					std::cout << "skipping cdrs up to "
						<< cdr::last_cdr_number_processed
						<< "  (these were processed before the last snapshot)\n";

					unsigned long long n_skipped = 0;
					const std::clock_t start = std::clock();

					while (std::getline(stm, line))
					{
						const auto cdr_number = cdr::cdr_num(line);
						if (cdr_number <= cdr::last_cdr_number_processed)
						{
							if (++n_skipped % 1'000'000 == 0)
							{
								std::cout << "skipped " << n_skipped << " cdrs, "
									<< " up to cdr #" << cdr_number << '\n';
							}

							continue;
						}

						else break; // next cdr is to be processed
					}

					const std::clock_t end = std::clock();
					const auto secs = ((end - start) * 1000.0 / CLOCKS_PER_SEC) / 1000.0;

					if (n_skipped > 0)
					{
						std::cout << "in all, skipped " << n_skipped << " cdrs in "
							<< secs << " seconds (" << n_skipped / secs << " cdrs skipped per sec)\n";
					}

					if (stm.good())
					{
						std::cout << "first cdr to be processed is #"
							<< cdr::cdr_num(line) << '\n';
						return line;
					}

					else // eof on input
					{
						// no cdrs to process, just return
						std::cout << "all cdrs in thwe imput were skipped; "
							<< "there are no cdrs to process\n";
						return {};
					}
				}

				else // no cdrs were skipped, cdr::last_cdr_number_processed == 0
				{
					// so we need to read the first line in input
					if (std::getline(stm, line)) return line;
					else return {};
				}
			}
		}


		bool process(std::istream& cdr_stm)
		{
			std::string line = get_first_cdr_line(cdr_stm);
			if (line.empty()) return false;

			unsigned long long cnt = 0;

			do // note: line was already read successfully
			{
				process(cdr(line));
				++cnt;
				
                #ifndef NDEBUG
				static constexpr unsigned long long N = 10'000;
				if (cnt % N == (N-1) ) std::cout << '.' << std::flush;
				#endif // NDEBUG
			} while (std::getline(cdr_stm, line));

			// finally, before we quit, update usage, send alerts if any 
            // and save a final snapshot
			update_usage();
			auto alerts = atmik::alerter::alert_msg::create_alerts(curr_day,curr_month);
			atmik::alerter::alert_recvr::send_alerts(alerts);
			sim::changed_sims.clear();
			atmik::alerter::save_snapshot();
			return cnt > 0 ;
		}

		bool process(const std::string& cdr_file_name)
		{
			if (std::ifstream file{ cdr_file_name }) return process(file);
			else return false;
		}
	}
}
