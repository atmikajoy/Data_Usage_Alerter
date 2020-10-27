#ifndef CDR_H_INCLUDED
#define CDR_H_INCLUDED
#include<string>
#include <iostream>

namespace atmik
{
	namespace alerter
	{
		struct cdr
		{
		    cdr( const std::string& line);

			unsigned long long cdr_number = 0;
			std::string iccid;
			unsigned int mcc = 0;
			unsigned int mnc = 0;
			unsigned long long total_bytes = 0;
			std::string time;

			static unsigned long long last_cdr_number_processed ;

			static unsigned long long cdr_num(const std::string& line);
		};

		std::ostream& operator<< (std::ostream& stm, const cdr& c);
	}
}
#endif // !CDR_H_INCLUDED
