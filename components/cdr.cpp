#include "cdr.h"
#include<sstream>
#include <limits>
#include<regex>


namespace atmik
{
    namespace alerter
    {
        namespace
        {
            static constexpr char comma = ',';
            static constexpr auto nchars = std::numeric_limits<std::streamsize>::max();
            
            inline void skip_fld(std::istream& stm)
            {
                stm.ignore(nchars, comma);
            }

            inline void skip_flds(std::istream& stm, std::size_t n)
            {
                for (std::size_t i = 0; i < n; ++i) skip_fld(stm);
            }

            const std::string txt_cap = "(\\w+)";
            const std::string num_cap = "(\\d+)";
            const std::string ignore = "[^,]*";
            const std::string comma_sep = ",";
            const std::string time_cap = "([^,]+)"; // 2020-04-01 10:30:00

            const std::regex cdr_re(num_cap + comma_sep // cdr number, capture 1
                + time_cap + comma_sep // time stamp, capture 2
                + txt_cap + comma_sep // iccid, capture 3
                + ignore + comma_sep // type
                + ignore + comma_sep // usage type
                + ignore + comma_sep // traffic type
                + num_cap + comma_sep // mcc, capture 4
                + num_cap + comma_sep // mnc, capture 5
                + num_cap + comma_sep // total bytes, capture 6
                + ignore + comma_sep // 
                + ignore + comma_sep // 
                + ignore + comma_sep // 
                + ignore + comma_sep // 
                + ignore + comma_sep // 
                + ignore + comma_sep // 
                + num_cap + comma_sep // upload nytes, capture 7
                + num_cap);// download nytes, capture 8

            // const std::regex txt_regex("[^a-zA-Z0-9\x20]+,");
            std::smatch match;
        }
        unsigned long long cdr::last_cdr_number_processed = 0 ;

        unsigned long long cdr::cdr_num(const std::string& line)
        {
            try { return std::stoull(line); }
            catch (const std::exception&) { return 0; } // bad line
        }

        cdr::cdr(const std::string& line)
        {
            /*
            {
                if (std::regex_match(line, match, cdr_re))
                {
                    cdr_number = std::stoull( match[1] );
                    time = match[2];
                    iccid = match[3];
                    mcc = std::stoi(match[4]);
                    mnc = std::stoi(match[5]);
                    total_bytes = std::stoull(match[6]);
                    unsigned long long upload_bytes = std::stoull(match[7]);
                    unsigned long long download_bytes = std::stoull(match[8]);

                    if (total_bytes == 0) total_bytes = upload_bytes + download_bytes;
                    return;
                }
            }
            */

            std::istringstream stm(line);

            std::string v;
            char sep;


                
            last_cdr_number_processed = cdr_number;

            
            unsigned long long upload_bytes = 0;
            unsigned long long download_bytes = 0;
            
            stm >> cdr_number >> sep;
            std::getline(stm, time, comma); // create_time - timestamp 2
            std::getline(stm, iccid, comma); // iccid 3
            
            skip_flds(stm, 3);
            
            /*
            std::getline(stm, v, comma); // type 4
            std::getline(stm, v, comma); // usage type  5
            std::getline(stm, v, comma); // traffic type  6
            */
            

            stm >> mcc >> sep >> mnc >> sep >> total_bytes >> sep;

            skip_flds(stm, 6);
            
            /*
            std::getline(stm, v, comma); // start time 10
            std::getline(stm, v, comma); // duration 11
            std::getline(stm, v, comma); // carrier ac 12
            std::getline(stm, v, comma); // imsi 13
            std::getline(stm, v, comma); // imei 14
            std::getline(stm, v, comma); // msisdn 15
            */
            

            stm >> upload_bytes >> sep >> download_bytes;

            if (total_bytes == 0) total_bytes = upload_bytes + download_bytes;
        }

        std::ostream& operator<< (std::ostream& stm, const cdr& c)
        {
            return stm << c.cdr_number << ' '
                << c.time << ' '
                << c.iccid << ' '
                << c.mcc << ' '
                << c.mnc << ' '
                << c.total_bytes << '\n';
        }
    }
}
