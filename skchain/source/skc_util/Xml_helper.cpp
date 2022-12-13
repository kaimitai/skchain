#include "Xml_helper.h"
#include <filesystem>
#include <stdexcept>
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_xml.h"
#include "./../common/klib/klib_util.h"

void skc::save_metadata_xml(const std::string p_folder, const std::string p_filename,
	const std::map<std::size_t, std::vector<std::pair<std::size_t, position>>>& p_meta_tiles,
	const std::vector<std::vector<bool>>& p_spawn_schedules,
	const std::vector<std::vector<byte>>& p_enemy_sets,
	const skc::SKC_Config& p_config) {

	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(c::XML_META_COMMENTS);

	auto n_metadata = doc.append_child(c::XML_TAG_META);
	n_metadata.append_attribute(c::XML_ATTR_APP_VERSION);
	n_metadata.attribute(c::XML_ATTR_APP_VERSION).set_value(c::APP_VERSION);

	auto n_lvl_meta = n_metadata.append_child(c::XML_TAG_LEVEL_META_ITEMS);
	for (const auto& kv : p_meta_tiles)
		for (const auto& l_md_tile : kv.second) {
			std::size_t l_md_index{ l_md_tile.first };
			if (p_config.get_meta_tile_movable(l_md_index)) {
				auto n_lvl_meta_item = n_lvl_meta.append_child(c::XML_TAG_LEVEL_META_ITEM);
				n_lvl_meta_item.append_attribute(c::XML_ATTR_NO);
				n_lvl_meta_item.attribute(c::XML_ATTR_NO).set_value(l_md_index);
				n_lvl_meta_item.append_attribute(c::XML_ATTR_POSITION);
				n_lvl_meta_item.attribute(c::XML_ATTR_POSITION).set_value(get_position_string(l_md_tile.second).c_str());
			}
		}

	auto n_schedules{ n_metadata.append_child(c::XML_TAG_SCHEDULES) };
	for (std::size_t i{ 0 }; i < p_spawn_schedules.size(); ++i) {
		auto n_schedule = n_schedules.append_child(c::XML_ATTR_SCHEDULE);
		n_schedule.append_attribute(c::XML_ATTR_NO);
		n_schedule.attribute(c::XML_ATTR_NO).set_value(i);
		n_schedule.append_attribute(c::XML_ATTR_SCHEDULE);
		n_schedule.attribute(c::XML_ATTR_SCHEDULE).set_value(klib::util::string_join(p_spawn_schedules[i], ',').c_str());
	}

	auto n_enemy_sets{ n_metadata.append_child(c::XML_TAG_ENEMY_SETS) };
	for (std::size_t i{ 0 }; i < p_enemy_sets.size(); ++i) {
		auto n_enemy_set = n_enemy_sets.append_child(c::XML_ATTR_ENEMY_SET);
		n_enemy_set.append_attribute(c::XML_ATTR_NO);
		n_enemy_set.attribute(c::XML_ATTR_NO).set_value(i);
		n_enemy_set.append_attribute(c::XML_ATTR_ENEMY_SET);
		n_enemy_set.attribute(c::XML_ATTR_ENEMY_SET).set_value(klib::util::string_join(p_enemy_sets[i], ',').c_str());
	}

	std::filesystem::create_directory(p_folder);
	std::string l_file_path = p_folder + '/' + p_filename;
	if (!doc.save_file(l_file_path.c_str()))
		throw std::runtime_error("Could not save " + l_file_path);
}

void skc::save_level_xml(const skc::Level& p_level, const std::string p_folder, const std::string p_filename) {
	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(c::XML_LEVEL_COMMENTS);

	auto n_metadata = doc.append_child(c::XML_TAG_META);
	n_metadata.append_attribute(c::XML_ATTR_APP_VERSION);
	n_metadata.attribute(c::XML_ATTR_APP_VERSION).set_value(c::APP_VERSION);

	auto n_level = n_metadata.append_child(c::XML_TAG_LEVEL);
	n_level.append_attribute(c::XML_ATTR_START_POSITION);
	n_level.attribute(c::XML_ATTR_START_POSITION).set_value(get_position_string(p_level.get_player_start_pos()).c_str());
	n_level.append_attribute(c::XML_ATTR_DOOR_POSITION);
	n_level.attribute(c::XML_ATTR_DOOR_POSITION).set_value(get_position_string(p_level.get_door_pos()).c_str());
	n_level.append_attribute(c::XML_ATTR_KEY_POSITION);
	n_level.attribute(c::XML_ATTR_KEY_POSITION).set_value(get_position_string(p_level.get_key_pos()).c_str());
	n_level.append_attribute(c::XML_ATTR_KEY_STATUS);
	n_level.attribute(c::XML_ATTR_KEY_STATUS).set_value(p_level.get_key_status());

	n_level.append_attribute(c::XML_ATTR_SPAWN_ENEMY_LIFETIME);
	n_level.attribute(c::XML_ATTR_SPAWN_ENEMY_LIFETIME).set_value(p_level.get_spawn_enemy_lifetime());

	n_level.append_attribute(c::XML_ATTR_TIME_DECREASE_RATE);
	n_level.attribute(c::XML_ATTR_TIME_DECREASE_RATE).set_value(p_level.get_time_decrease_rate());

	if (p_level.has_constellation()) {
		n_level.append_attribute(c::XML_ATTR_CONSTELLATION_NO);
		n_level.attribute(c::XML_ATTR_CONSTELLATION_NO).set_value(p_level.get_constellation_no());
		n_level.append_attribute(c::XML_ATTR_CONSTELLATION_POSITION);
		n_level.attribute(c::XML_ATTR_CONSTELLATION_POSITION).set_value(get_position_string(p_level.get_constellation_pos()).c_str());
	}

	n_level.append_attribute(c::XML_ATTR_TILESET);
	n_level.attribute(c::XML_ATTR_TILESET).set_value(p_level.get_tileset_no());

	auto n_blocks = n_level.append_child(c::XML_TAG_BLOCKS);
	for (int j{ 0 }; j < 12; ++j) {
		auto n_row = n_blocks.append_child(c::XML_TAG_BLOCK_ROW);
		n_row.append_attribute(c::XML_ATTR_NO);
		n_row.attribute(c::XML_ATTR_NO).set_value(j);
		std::vector<int> l_row_data;
		for (int i{ 0 }; i < 16; ++i)
			l_row_data.push_back(wall_type_to_int(p_level.get_wall_type(i, j)));
		n_row.append_attribute(c::XML_ATTR_VALUE);
		n_row.attribute(c::XML_ATTR_VALUE).set_value(klib::util::string_join<int>(l_row_data, ',').c_str());
	}

	const auto& l_items{ p_level.get_items() };

	auto n_items = n_level.append_child(c::XML_TAG_ITEMS);
	int l_counter{ 0 };
	for (const auto& element : l_items) {
		auto n_item = n_items.append_child(c::XML_TAG_ITEM);
		n_item.append_attribute(c::XML_ATTR_NO);
		n_item.attribute(c::XML_ATTR_NO).set_value(l_counter++);
		n_item.append_attribute(c::XML_ATTR_ELEMENT_NO);
		n_item.attribute(c::XML_ATTR_ELEMENT_NO).set_value(element.get_element_no());
		n_item.append_attribute(c::XML_ATTR_POSITION);
		n_item.attribute(c::XML_ATTR_POSITION).set_value(get_position_string(element.get_position()).c_str());
	}

	const auto& l_enemies{ p_level.get_enemies() };

	auto n_enemies = n_level.append_child(c::XML_TAG_ENEMIES);
	l_counter = 0;
	for (const auto& element : l_enemies) {
		if (element.get_element_type() == skc::Element_type::Enemy) {
			auto n_enemy = n_enemies.append_child(c::XML_TAG_ENEMY);
			n_enemy.append_attribute(c::XML_ATTR_NO);
			n_enemy.attribute(c::XML_ATTR_NO).set_value(l_counter++);
			n_enemy.append_attribute(c::XML_ATTR_ELEMENT_NO);
			n_enemy.attribute(c::XML_ATTR_ELEMENT_NO).set_value(element.get_element_no());
			n_enemy.append_attribute(c::XML_ATTR_POSITION);
			n_enemy.attribute(c::XML_ATTR_POSITION).set_value(get_position_string(element.get_position()).c_str());
		}
	}

	auto n_mirrors = n_level.append_child(c::XML_TAG_MIRRORS);
	for (std::size_t i{ 0 }; i < 2; ++i) {
		auto n_mirror = n_mirrors.append_child(c::XML_TAG_MIRROR);
		n_mirror.append_attribute(c::XML_ATTR_NO);
		n_mirror.attribute(c::XML_ATTR_NO).set_value(i);
		n_mirror.append_attribute(c::XML_ATTR_POSITION);
		n_mirror.attribute(c::XML_ATTR_POSITION).set_value(get_position_string(p_level.get_spawn_position(i)).c_str());
		n_mirror.append_attribute(c::XML_ATTR_SCHEDULE);
		n_mirror.attribute(c::XML_ATTR_SCHEDULE).set_value(p_level.get_spawn_schedule(i));
		n_mirror.append_attribute(c::XML_ATTR_ENEMY_SET);
		n_mirror.attribute(c::XML_ATTR_ENEMY_SET).set_value(p_level.get_spawn_enemies(i));
	}

	std::filesystem::create_directory(p_folder);
	std::string l_file_path = p_folder + '/' + p_filename;
	if (!doc.save_file(l_file_path.c_str()))
		throw std::runtime_error("Could not save " + l_file_path);
}

std::string skc::get_position_string(const std::pair<int, int>& p_position) {
	std::string result{ std::to_string(p_position.first) };
	result.push_back(',');
	result += std::to_string(p_position.second);
	return result;
}

int skc::wall_type_to_int(skc::Wall p_wall_type) {
	if (p_wall_type == skc::Wall::None)
		return 0;
	else if (p_wall_type == skc::Wall::Brown)
		return 1;
	else if (p_wall_type == skc::Wall::White)
		return 2;
	else
		return 3;
}
