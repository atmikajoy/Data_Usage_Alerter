#ifndef CDR_WRITE_H_INCLUDED
#define CDR_WRITE_H_INCLUDED
#include<string>
#include<vector>
#include<unordered_set>
namespace atmik
{
	namespace data_gen
	{
		namespace write_cdr
		{
			struct iccid_mcc_mcn_data
			{
				std::string iccid;
				std::string imsi;
				int mcc;
				int mcn;
				
				friend bool operator== ( const iccid_mcc_mcn_data& a, const iccid_mcc_mcn_data& b ) 
				{ return a.iccid == b.iccid; }
				friend bool operator< (const iccid_mcc_mcn_data& a, const iccid_mcc_mcn_data& b)
				{
					return a.iccid < b.iccid;
				}
			};

			iccid_mcc_mcn_data& random_sim(const std::vector<iccid_mcc_mcn_data>& selected_sims);

			std::vector<iccid_mcc_mcn_data> select_sims(int pct_of_sims);

			std::vector<std::string> iccid_vec();

			void write_to_file_cdr(int min_percent_sims, int max_percent_sims);
		
		}
	}
}


#endif // !CDR_WRITE_H_INCLUDED
