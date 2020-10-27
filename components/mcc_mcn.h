#ifndef MCC_MCN_H_INCULDED
#define MCC_MCN_H_INCULDED

#include <unordered_map>
#include <vector>

namespace atmik
{
	namespace alerter
	{
		using mcc_mcn_map_type = std::unordered_map<int, std::vector<int>>;

		const mcc_mcn_map_type& mcc_mcn_map();

		std::vector<int> mcc_vec();

		int random_mcc();
		int random_mcn(int mcc);
	}
}

#endif // MCC_MCN_H_INCULDED
