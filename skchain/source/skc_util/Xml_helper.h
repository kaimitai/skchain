#ifndef SKC_XML_HELPER_H
#define SKC_XML_HELPER_H

#include <string>
#include <utility>
#include "./../SKC_Level.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"

namespace skc {

	void save_level_xml(const skc::Level& p_level, const std::string p_folder, const std::string p_filename);

	std::string get_position_string(const std::pair<int, int>& p_position);
	int wall_type_to_int(skc::Wall p_wall_type);
}

#endif
