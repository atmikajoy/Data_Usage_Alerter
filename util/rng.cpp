#include "rng.h"

namespace atmik
{
	namespace util
	{
		std::mt19937 rng{ std::random_device{}() };

		unsigned long long random_int(unsigned long long min, unsigned long long max)
		{
			static std::uniform_int_distribution<unsigned long long> distrib;
			using param_type = decltype(distrib)::param_type;

			distrib.param(param_type(min, max));
			return distrib(rng);
		}

		std::size_t random_size_t(std::size_t min, std::size_t max)
		{
			static std::uniform_int_distribution<std::size_t> distrib;
			using param_type = decltype(distrib)::param_type;

			distrib.param(param_type(min, max));
			return distrib(rng);
		}

	}
}
