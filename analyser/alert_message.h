#ifndef ALERT_MESSAGE_H_INCLUDED
#define ALERT_MESSAGE_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include "../components/usage.h"
#include "../components/sim.h"
#include "../components/enterprise.h"
#include "../components/group.h"
#include"../components/zone.h"
#include <cmath>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <vector>
#include <tuple>
#include <string_view>
#include <thread>
#include <atomic>
#include <chrono>
// #include"../util/sock_util.h"

namespace atmik
{
	namespace alerter
	{
		struct alert_msg
		{
			int day = 1 ;
			int month = 1 ;

			enum source_t : char { SIM = 's', GROUP = 'g', ENTERPRISE = 'e', ZONE = 'z' };
			enum alert_t : char { DAILY = 'd', MONTHLY = 'm' };
			using id_cstr = std::string; //  char[22];
			source_t source;
			id_cstr sender_id {};
			alert_t type;
			unsigned long long curr_usage;
			unsigned long long limit;

			alert_msg( int day, int month, source_t s, const std::string& id, alert_t t,
				unsigned long long cu, unsigned long long l)
				: day(day), month(month), source(s), sender_id(id), 
				  type(t), curr_usage(cu), limit(l)
			{
			}

			unsigned int percentage() const
			{
				return (unsigned int)(std::round((curr_usage * 100.0) / limit));
			}

			static std::vector<alert_msg> create_alerts( int d, int m );

			template < typename ITEM >
			static void add_alerts( int d, int m, std::vector<alert_msg>& alerts )
			{
				for (auto& [k, it] : ITEM::objects())
				{
					if ( !it.usage.daily.alert_sent && it.usage.daily.alert_reached())
					{
						alerts.push_back(create_daily_alert( d, m, it));
						it.usage.daily.alert_sent = true;
					}

					if ( !it.usage.monthly.alert_sent && it.usage.monthly.alert_reached())
					{
						alerts.push_back(create_monthly_alert(d, m, it));
						it.usage.monthly.alert_sent = true;
					}
				}
			}



			static constexpr source_t source_type( const sim&) { return SIM; }
			static constexpr source_t source_type( const group&) { return GROUP; }
			static constexpr source_t source_type( const enterprise&) { return ENTERPRISE; }
			static constexpr source_t source_type( const zone&) { return ZONE; }
			static std::string id_of(const sim& s) { return s.id(); }

			template < typename ITEM >
			static std::string id_of(const ITEM& it) { return std::to_string(it.id()); }

			template < typename ITEM >
			static alert_msg create_daily_alert( int d, int m, const ITEM& it)
			{
				return { d, m, source_type(it), id_of(it), DAILY,
						 it.usage.daily.amount, it.usage.daily.limit };
			}

			template < typename ITEM >
			static alert_msg create_monthly_alert( int d, int m, const ITEM& it)
			{
				return { d, m, source_type(it), id_of(it), MONTHLY,
						 it.usage.monthly.amount, it.usage.monthly.limit };
			}
		};

		inline std::string source_name( alert_msg::source_t s)
		{
			switch (s)
			{
			    case alert_msg::SIM: return "sim";
				case alert_msg::GROUP: return "group";
				case alert_msg::ENTERPRISE: return "enterprise";
				default: return "zone";
			}
		}

		std::ostream& operator<< (std::ostream& stm, const alert_msg& am);

		inline bool operator< (const alert_msg& a, const alert_msg& b)
		{
			using tuple = std::tuple< alert_msg::source_t, alert_msg::alert_t, std::string_view >;
			return tuple{ a.source, a.type, a.sender_id } < tuple{ b.source, b.type, b.sender_id };
		}


	}
}
#endif // ALERT_MESSAGE_H_INCLUDED