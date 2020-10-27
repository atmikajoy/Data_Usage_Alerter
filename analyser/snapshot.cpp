#include "snap_shot.h"
#include <filesystem>
#include <fstream>
#include "../components/cdr.h"
#include "../components/sim.h"
#include"../components/group.h"
#include"../components/enterprise.h"
#include "../components/zone.h"


namespace atmik
{
	namespace alerter
	{
		namespace
		{
			static const char snapshot_root[] = "C:/cdr/snapshots";
			static const char dir_extension[] = ".snapshot";

			static const char last_cdr_num_file_name[] = "last_cdr_number.txt";
			static const char sim_snapshot_name[] = "sims.txt";
			static const char group_snapshot_name[] = "group.txt";
			static const char enterprise_snapshot_name[] = "enterprise.txt";
			static const char zone_snapshot_name[] = "zone.txt";

			// snapshot directories in snapshot_root are named
			// nnnn.snapshot where nnnn is a sequence number

			// returns 0 if no snapshot directoroes are found
			unsigned long last_snapshot_dir_n()
			{
				using namespace std::filesystem;

				path dir_path(snapshot_root);
				unsigned long last_n = 0;

				for (auto& de : directory_iterator(dir_path))
				{
					auto p = de.path();
					if (is_directory(p))
					{
						try
						{
							unsigned long n = std::stoul(p.filename().stem());

							if (n > last_n)
							{
								last_n = n;
							}
						}

						catch (const std::exception&) {}
					}
				}

				return last_n;
			}

			std::string nnnn_string( unsigned long n, std::size_t sz = 4 )
			{
				std::string str = std::to_string(n);
				while ( str.size() < sz ) str = '0' + str;
				return str;
			}
		}

		std::string last_snapshot_dir()
		{
			using namespace std::filesystem;

			path dir_path(snapshot_root);
			unsigned long last_n = last_snapshot_dir_n() ;
			path last_dir_path;

			for (auto& de : directory_iterator(dir_path))
			{
				auto p = de.path();
				if (is_directory(p))
				{
					try
					{
						unsigned long n = std::stoul(p.filename().stem());

						if (n == last_n) return p.string();
					}

					catch (const std::exception&) {}
				}
			}

			return last_dir_path.string() ;
		}

		std::string next_snapshot_dir()
		{
			using namespace std::filesystem;

			path dir_path(snapshot_root);
			unsigned long next_n = last_snapshot_dir_n() + 1 ;

			const path next_dir_path = dir_path / (nnnn_string(next_n) + dir_extension);
			create_directory(next_dir_path);
			return next_dir_path.string() ;
		}

		void save_snapshot()
		{
			std::string dir = next_snapshot_dir();
			
			// save the cdr number
			std::ofstream(dir + '/' + last_cdr_num_file_name)
				<< cdr::last_cdr_number_processed << '\n';

			// save enterprises
			// save groups
			// save sims
			sim::save(dir + '/' + sim_snapshot_name);
			group::save(dir + '/' + group_snapshot_name);
			enterprise::save(dir + '/' + enterprise_snapshot_name);
			zone::save(dir + '/' + zone_snapshot_name);
			// save zones
		}

		namespace
		{
			void clear_all()
			{
				enterprise::objects().clear();
				group::objects().clear();
				sim::objects().clear();
				zone::objects().clear();
			}
		}
		bool load_last_snapshot()
		{
			const std::string dir = last_snapshot_dir();
			if (dir.empty() || !std::filesystem::is_directory(dir))
				return false;

			// read the last cdr number
			if (!(std::ifstream(dir + '/' + last_cdr_num_file_name)
				>> cdr::last_cdr_number_processed))
			{
				cdr::last_cdr_number_processed = 0;
				return false;
			}

			clear_all();

			// load enterprises, load groups, load sims, load zones
			// return true on sucess
			if ( enterprise::load(dir + '/' + enterprise_snapshot_name) &&
				 group::load(dir + '/' + group_snapshot_name) &&
                 sim::load(dir + '/' + sim_snapshot_name) &&
				 zone::load(dir + '/' + zone_snapshot_name ) ) return true ;

			// load failed; clear data and return false
			clear_all();
			return false;
		}
	}
}