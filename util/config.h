#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace atmik
{
	namespace config
	{
		std::string file_name();
		boost::property_tree::ptree& ptree();


		// returns the value of the specified key
		// adds the key and the default value to the tree if key is not found 
		template < typename T >
		T get(const std::string& xml_key, const T& default_value = T{})
		{
			try { return ptree().get<T>(xml_key); }
			catch (const std::exception&)
			{
				ptree().put(xml_key, default_value);
				return default_value;
			}
		}
	}
}

#endif // CONFIG_H_INCLUDED