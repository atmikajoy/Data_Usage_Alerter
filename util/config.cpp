#define  _CRT_SECURE_NO_WARNINGS
#include "config.h"
#include <cstdlib>
#include <boost/property_tree/xml_parser.hpp>
#include <locale>

namespace atmik
{
	namespace config
	{
		std::string file_name()
		{
			static const std::string default_file_name = "C:/cdr/config/config.xml";
			static const std::string env_variable_name = "ALERTER_CONFIG_FILE";

			const char* env_var = std::getenv(env_variable_name.c_str());
			if (env_var) return env_var;
			else return default_file_name;
		}

		namespace
		{
			// wrapper class to manage 
			// 1. automatic initialisation of the ptree from an xml file
			// 2. automatic updation of the xml file on destruction (at end of program)
			struct ptree_wrapper_
			{
				ptree_wrapper_()
				{
					read_xml(file_name(), pt, boost::property_tree::xml_parser::trim_whitespace);
				}

				~ptree_wrapper_()
				{
					boost::property_tree::write_xml(file_name(), pt, std::locale::classic(),
						boost::property_tree::xml_parser::xml_writer_make_settings<std::string>(' ', 4));
				}

				boost::property_tree::ptree pt;
			};
		}

		boost::property_tree::ptree& ptree()
		{
			static ptree_wrapper_ wrapper;
			return wrapper.pt;
		}
	}
}
