#ifndef SKC_XML_HELPER_H
#define SKC_XML_HELPER_H

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "./../SKC_Level.h"
#include "./../SKC_Config.h"
#include "./../SKC_Game_metadata.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"

namespace skc {
	namespace xml {

		void save_xml_file(const pugi::xml_document& p_doc, const std::string p_folder, const std::string p_filename);
		pugi::xml_document load_xml_file(const std::string p_folder, const std::string p_filename);

		void save_metadata_xml(const std::string p_folder, const std::string p_filename,
			const std::map<std::size_t, std::vector<std::pair<std::size_t, position>>>& p_meta_tiles,
			const std::vector<std::vector<bool>>& p_spawn_schedules,
			const std::vector<std::vector<byte>>& p_enemy_sets,
			const skc::SKC_Config& p_config);
		void save_level_xml(const skc::Level& p_level, const std::string p_folder, const std::string p_filename);

		skc::Level load_level_xml(const std::string p_folder, const std::string p_filename);
		skc::Game_metadata load_metadata_xml(const std::string p_folder, const std::string p_filename);

		std::string position_to_string(const std::pair<int, int>& p_position);
		position string_to_position(const std::string& p_position_str);
		int wall_type_to_int(skc::Wall p_wall_type);
		skc::Wall int_to_wall_type(int p_wall_type_no);

		// get region information from xml nodes
		bool node_has_region_attribute(const pugi::xml_node p_node);
		bool node_has_region_code(const pugi::xml_node p_node, const std::string& p_region_code);
		std::string node_region_best_match(const pugi::xml_node p_node,
			const std::string& p_region_code,
			const std::string& p_xml_tag, const std::string p_value_attr);
	}
}

#endif
