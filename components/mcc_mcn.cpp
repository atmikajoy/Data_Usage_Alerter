#include "mcc_mcn.h"
#include "../util/rng.h"
#include <fstream>
#include <string>
#include <sstream>

namespace atmik
{
	namespace alerter
	{
		namespace
		{
			mcc_mcn_map_type create_mcc_mcn_map()
			{
				static const std::string data_file = "C:\\cdr\\data\\mcc_mcn.txt";
				if (std::ifstream file{ data_file })
				{
					mcc_mcn_map_type map;
					std::string line;
					int cc;
					int pc;
					while (std::getline(file, line))
					{
						std::stringstream stm(line);
						stm >> cc;
						auto& vec = map[cc];
						while (stm >> pc) vec.push_back(pc);
					}
					return map;
				}

				throw std::runtime_error("bad mcc_mcn file");
			}



		}

		std::vector<int> mcc_vec()
		{
			std::vector<int> mcc_vec;
			for (const auto& pair : mcc_mcn_map()) mcc_vec.push_back(pair.first);
			return mcc_vec;
		}

		const mcc_mcn_map_type& mcc_mcn_map()
		{
			static const mcc_mcn_map_type map = create_mcc_mcn_map();
			return map;
		}

		int random_mcc()
		{
			static const std::vector<int> vec = mcc_vec();
			static const std::size_t N = vec.size();

			return vec[atmik::util::random_size_t(0, N - 1)];
		}

		int random_mcn(int mcc)
		{
			auto iter = mcc_mcn_map().find(mcc);
			if (iter == mcc_mcn_map().end()) throw std::out_of_range("bad mcc");

			const auto& vec = iter->second;
			if (vec.empty()) throw std::out_of_range("empty mcn vector");
			return vec[atmik::util::random_size_t(0, vec.size() - 1)];
		}
	}
}



