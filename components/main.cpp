#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "group.h"
#include "enterprise.h"
#include "sim.h"
#include "../util/time_utils.h"

int main()
{
	{
		using namespace atmik::util;

		const std::time_t now = std::time(nullptr);
		const auto tm = *std::localtime(std::addressof(now));
		const std::string tstamp = time_stamp(tm,24*60*60);

		std::cout << tstamp << '\n';

		std::cout << now + 24 * 60 * 60  << ' ' << to_time_t(tstamp) << '\n';

		// return 0;
	}
	using namespace atmik::alerter;
	auto& e = enterprise::create( 82, "abcd enterprise" );

	auto& g1 = group::create(1234, "test group one", enterprise::look_up(82) );
	std::cout << group::look_up(1234).name() << '\n'
		<< "enterprise is: " << group::look_up(1234).eprise.name() << '\n';

	try
	{
		auto& g2 = group::create(1235, "amother groop", enterprise::look_up(82) );
		std::cout << group::look_up(1235).name() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cout << "*** error: " << e.what() << '\n';
	}

	auto& s = sim::create("iccid1", "imei", group::look_up(1235));
	s.usage.daily.amount = 12345;
	s.usage.monthly.amount = 99999999;

	std::cout << sim::look_up("iccid1").imei
		<< ' ' << sim::look_up("iccid1").grp.name() << '\n';

	std::cout << s << '\n';
	std::cout << "Group : " << g1<<'\n';
	std::cout << "Enterprise: " << e<< '\n';

}