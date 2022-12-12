#ifndef SKC_XML_HELPER_H
#define SKC_XML_HELPER_H

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "./../SKC_Level.h"
#include "./../SKC_Config.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"

namespace skc {

	void save_metadata_xml(const std::string p_folder, const std::string p_filename,
		const std::map<std::size_t, std::vector<std::pair<std::size_t, position>>>& p_meta_tiles,
		const std::vector<std::vector<bool>>& p_spawn_schedules,
		const std::vector<std::vector<byte>>& p_enemy_sets,
		const skc::SKC_Config& p_config);
	void save_level_xml(const skc::Level& p_level, const std::string p_folder, const std::string p_filename);

	std::string get_position_string(const std::pair<int, int>& p_position);
	int wall_type_to_int(skc::Wall p_wall_type);
}

#endif
