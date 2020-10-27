#ifndef RNG_H_INCULDED
#define RNG_H_INCLUDED

#include <random>
#include <type_traits>

namespace atmik
{
	namespace util
	{
		extern std::mt19937 rng;

		unsigned long long random_int(unsigned long long min, unsigned long long max);

		template < typename T > T random_int(T a, T b)
		{
			static_assert(std::is_integral_v<T>);
			unsigned long long aa = a;
			unsigned long long bb = b;
			return T(random_int(aa, bb));
		}

		std::size_t random_size_t(std::size_t min, std::size_t max);
	}
}

#endif // MASTER_SIM_H_INCLUDED

