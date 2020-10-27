#include "alert_message.h"
#include "../components/enterprise.h"
#include "../components/group.h"
#include "../components/sim.h"
#include"../components/zone.h"
#include"../components/mcc_mcn.h"
#include <iomanip>
#include<iostream>
// #include "../data_gen/data_load.h"
#include"process.h"
#include "../components/cdr.h"
#include <ctime>
#include <fstream>
#include "../util/rng.h"
#include <sstream>
#include"../util/sock_util.h"
#include "alert_sender.h"
#include "snap_shot.h"
#include <filesystem>
#include "alert_recvr.h"
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#undef UNICODE
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "alert_message.h"
#include <string>
#include "../util/config.h"

int main()
{
	using namespace atmik;

	const std::string last_snapshot_dir = alerter::last_snapshot_dir();
	bool try_load_snapshot = false;

	if (!last_snapshot_dir.empty())
	{
		std::cout << "load snapshot from dir " << last_snapshot_dir << " << ? (y/n): ";
		char yn;
		if (std::cin >> yn && yn == 'y' || yn == 'Y') try_load_snapshot = true;
	}
	
	if ( try_load_snapshot && atmik::alerter::load_last_snapshot())
	{
		// enterprise etc. were loaded from the last snapshot
		std::cout << "loaded last snapshot from directory "
			<< last_snapshot_dir << '\n'
			<< "last cdr number == " << alerter::cdr::last_cdr_number_processed
			<< '\n';
	}

	else
	{
		// attempted load of last dnapshot failed
		std::cout << "snapshot not loaded / attempted load of last dnapshot "
			      << "failed\nloading data from alerter.data_files\n";

		// load original data from master data files
		static const std::string root_dir =
			atmik::config::get<std::string>("alerter.data_files.root_location", "C:/cdr/data/");

		alerter::enterprise::load(root_dir +
			atmik::config::get<std::string>("alerter.data_files.enterprise", "enterprise_file.txt"));

		alerter::group::load(root_dir +
			atmik::config::get<std::string>("alerter.data_files.group", "group_file.txt"));

		alerter::sim::load(root_dir +
			atmik::config::get<std::string>("alerter.data_files.sim", "sim_file.txt"));

		alerter::zone::load(root_dir +
			atmik::config::get<std::string>("alerter.data_files.zone", "zone_file.txt"));
	}


	std::cout << alerter::sim::objects().size() << " sims\n"
		<< alerter::group::objects().size() << " groups\n"
		<< alerter::enterprise::objects().size() << " enterprises\n"
		<< alerter::zone::objects().size() << " zones\n";

	int main1();
	for (int i = 0; i < 1; ++i) main1();

	/*
	const auto start_snap = std::clock();
	atmik::alerter::save_snapshot();
	const auto end_snap = std::clock();
	std::cout << "snapshot took "
		<< (end_snap - start_snap) * 1000.0 / CLOCKS_PER_SEC << " millisecs\n";
    */
}

int main1()
{
	std::cout << "\n----------------\n" << std::flush;


	{
		/*
		const std::string line = "1000001,2020-04-01 10:30:00,899101000000100031,type,usage_type,"
			                     "traffic_type,33,428,11204,start_time,duration,carrierAC,900000000000016,"
			                     "IMEI,MSISDN,0,0";
		atmik::alerter::cdr test(line);
		return 0;
		*/
	}
	using namespace atmik;

	alerter::reset_num_cdrs_processed();

	// std::cout << "type enter to process cdrs\n" && std::cin.get();

	static const std::string root_dir =
		atmik::config::get<std::string>("alerter.data_files.root_location", "C:/cdr/data/");

	const std::string cdr_file_name = root_dir +
		atmik::config::get<std::string>("alerter.data_files.cdr", "cdr.txt");

	const auto start = std::clock();
	alerter::process(cdr_file_name);
	unsigned long long num_cdrs_read = 0;

	/*
	if (std::ifstream file{ "C:\\cdr\\log\\cdr.txt" })
	{
		std::string line;
		while (std::getline(file, line))
		{
			alerter::process( )
			++num_cdrs_read;
		}
	}
	*/


	const auto end = std::clock();

	const double secs = (end - start) * 1000.0 / CLOCKS_PER_SEC / 1000.0;
	const double cdrs_per_second = alerter::num_cdrs_processed() / secs;


	std::cout << "last cdr number: " << alerter::cdr::last_cdr_number_processed << '\n'
		<< alerter::num_cdrs_processed() << " cdrs were processed\n";
    
	if(alerter::num_cdrs_processed())
	    std::cout << secs << " seconds, " << std::fixed << cdrs_per_second << " cdrs per second\n";
	

	std::cout << "waiting for 10 seconds to end program..." ;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	std::cout << '\n';

	return 0; 

	///////////////////////////SOKET STUFF


	
	/*
	enterprise& e = enterprise::create(123456, "dummy");
	group& g = group::create(99, "anither dummy", e );
	sim& s = sim::create("abcdefghijklmnopqrstuvwxyz", "efgh", g);
	s.usage.daily.amount = 1500;
	s.usage.daily.limit = 6000;

	auto am = alert_msg::create_daily_alert(s);
	std::cout << sizeof(am) << ' ' << am.percentage() << "%\n"
		<< std::quoted(am.sender_id) << '\n';*/
}