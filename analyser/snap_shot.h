#ifndef SNAPSHOT_H_INCLUDED
#define SNAPSHOT_H_INCLUDED

#include <string>

namespace atmik
{
	namespace alerter
	{
		std::string last_snapshot_dir();
		std::string next_snapshot_dir();

		void save_snapshot();

		bool load_last_snapshot();
	}
}


#endif // SNAPSHOT_H_INCLUDED
