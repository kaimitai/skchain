#include "Xml_helper.h"
#include <filesystem>
#include <stdexcept>
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_xml.h"
#include "./../SKC_Config.h"
#include "./../common/klib/klib_util.h"

void skc::xml::save_xml_file(const pugi::xml_document& p_doc,
	const std::string p_folder, const std::string p_filename) {

	std::filesystem::create_directory(p_folder);
	std::string l_file_path = SKC_Config::path_combine(p_folder, p_filename);
	if (!p_doc.save_file(l_file_path.c_str()))
		throw std::runtime_error("Could not save " + l_file_path);
}

pugi::xml_document skc::xml::load_xml_file(const std::string p_folder,
	const std::string p_filename) {

	pugi::xml_document doc;
	std::string l_filepath{ SKC_Config::path_combine(p_folder, p_filename) };
	if (!doc.load_file(l_filepath.c_str()))
		throw std::runtime_error("Could not load " + l_filepath);

	return doc;
}

void skc::xml::save_metadata_xml(const std::string p_folder, const std::string p_filename,
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
				n_lvl_meta_item.attribute(c::XML_ATTR_POSITION).set_value(position_to_string(l_md_tile.second).c_str());
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

	save_xml_file(doc, p_folder, p_filename);
}

void skc::xml::save_level_xml(const skc::Level& p_level, const std::string p_folder, const std::string p_filename) {
	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(c::XML_LEVEL_COMMENTS);

	auto n_metadata = doc.append_child(c::XML_TAG_META);
	n_metadata.append_attribute(c::XML_ATTR_APP_VERSION);
	n_metadata.attribute(c::XML_ATTR_APP_VERSION).set_value(c::APP_VERSION);

	auto n_level = n_metadata.append_child(c::XML_TAG_LEVEL);
	n_level.append_attribute(c::XML_ATTR_START_POSITION);
	n_level.attribute(c::XML_ATTR_START_POSITION).set_value(position_to_string(p_level.get_player_start_pos()).c_str());
	n_level.append_attribute(c::XML_ATTR_DOOR_POSITION);
	n_level.attribute(c::XML_ATTR_DOOR_POSITION).set_value(position_to_string(p_level.get_door_pos()).c_str());
	n_level.append_attribute(c::XML_ATTR_KEY_POSITION);
	n_level.attribute(c::XML_ATTR_KEY_POSITION).set_value(position_to_string(p_level.get_key_pos()).c_str());
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
		n_level.attribute(c::XML_ATTR_CONSTELLATION_POSITION).set_value(position_to_string(p_level.get_constellation_pos()).c_str());
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
		n_item.attribute(c::XML_ATTR_POSITION).set_value(position_to_string(element.get_position()).c_str());
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
			n_enemy.attribute(c::XML_ATTR_POSITION).set_value(position_to_string(element.get_position()).c_str());
		}
	}

	auto n_mirrors = n_level.append_child(c::XML_TAG_MIRRORS);
	for (std::size_t i{ 0 }; i < 2; ++i) {
		auto n_mirror = n_mirrors.append_child(c::XML_TAG_MIRROR);
		n_mirror.append_attribute(c::XML_ATTR_NO);
		n_mirror.attribute(c::XML_ATTR_NO).set_value(i);
		n_mirror.append_attribute(c::XML_ATTR_POSITION);
		n_mirror.attribute(c::XML_ATTR_POSITION).set_value(position_to_string(p_level.get_spawn_position(i)).c_str());
		n_mirror.append_attribute(c::XML_ATTR_SCHEDULE);
		n_mirror.attribute(c::XML_ATTR_SCHEDULE).set_value(p_level.get_spawn_schedule(i));
		n_mirror.append_attribute(c::XML_ATTR_ENEMY_SET);
		n_mirror.attribute(c::XML_ATTR_ENEMY_SET).set_value(p_level.get_spawn_enemies(i));
	}

	save_xml_file(doc, p_folder, p_filename);
}

std::string skc::xml::position_to_string(const std::pair<int, int>& p_position) {
	std::string result{ std::to_string(p_position.first) };
	result.push_back(',');
	result += std::to_string(p_position.second);
	return result;
}

int skc::xml::wall_type_to_int(skc::Wall p_wall_type) {
	if (p_wall_type == skc::Wall::None)
		return 0;
	else if (p_wall_type == skc::Wall::Brown)
		return 1;
	else if (p_wall_type == skc::Wall::White)
		return 2;
	else
		return 3;
}

skc::Level skc::xml::load_level_xml(const std::string p_folder, const std::string p_filename) {
	pugi::xml_document doc = load_xml_file(p_folder, p_filename);
	skc::Level result;

	auto n_meta = doc.child(c::XML_TAG_META);
	auto n_level = n_meta.child(c::XML_TAG_LEVEL);

	result.set_player_start_pos(string_to_position(
		n_level.attribute(c::XML_ATTR_START_POSITION).as_string()));
	result.set_door_pos(string_to_position(
		n_level.attribute(c::XML_ATTR_DOOR_POSITION).as_string()));
	result.set_key_pos(string_to_position(
		n_level.attribute(c::XML_ATTR_KEY_POSITION).as_string()));
	result.set_key_status(n_level.attribute(c::XML_ATTR_KEY_STATUS).as_uint());
	result.set_spawn_enemy_lifetime(n_level.attribute(c::XML_ATTR_SPAWN_ENEMY_LIFETIME).as_uint());
	result.set_time_decrease_rate(n_level.attribute(c::XML_ATTR_TIME_DECREASE_RATE).as_uint());
	result.set_tileset_no(n_level.attribute(c::XML_ATTR_TILESET).as_uint());
	if (!n_level.attribute(c::XML_ATTR_CONSTELLATION_NO).empty())
		result.set_constellation(n_level.attribute(c::XML_ATTR_CONSTELLATION_NO).as_uint(),
			string_to_position(
				n_level.attribute(c::XML_ATTR_CONSTELLATION_POSITION).as_string()));

	auto n_blocks = n_level.child(c::XML_TAG_BLOCKS);
	std::vector<std::vector<skc::Wall>> l_lvl_blocks;
	for (auto n_block_row = n_blocks.child(c::XML_TAG_BLOCK_ROW); n_block_row;
		n_block_row = n_block_row.next_sibling(c::XML_TAG_BLOCK_ROW)) {
		std::vector<skc::Wall> l_block_row;
		auto l_block_str{ klib::util::string_split<int>(n_block_row.attribute(c::XML_ATTR_VALUE).as_string(), ',') };
		for (int l_block : l_block_str)
			l_block_row.push_back(int_to_wall_type(l_block));
		l_lvl_blocks.push_back(l_block_row);
	}
	result.set_blocks(l_lvl_blocks);

	auto n_items = n_level.child(c::XML_TAG_ITEMS);
	for (auto n_item = n_items.child(c::XML_TAG_ITEM); n_item;
		n_item = n_item.next_sibling(c::XML_TAG_ITEM)) {
		result.add_item(n_item.attribute(c::XML_ATTR_ELEMENT_NO).as_uint(),
			string_to_position(n_item.attribute(c::XML_ATTR_POSITION).as_string())
		);
	}

	auto n_enemies = n_level.child(c::XML_TAG_ENEMIES);
	for (auto n_enemy = n_enemies.child(c::XML_TAG_ENEMY); n_enemy;
		n_enemy = n_enemy.next_sibling(c::XML_TAG_ENEMY)) {
		result.add_enemy(n_enemy.attribute(c::XML_ATTR_ELEMENT_NO).as_uint(),
			string_to_position(n_enemy.attribute(c::XML_ATTR_POSITION).as_string())
		);
	}

	auto n_mirrors = n_level.child(c::XML_TAG_MIRRORS);
	for (auto n_mirror = n_mirrors.child(c::XML_TAG_MIRROR); n_mirror;
		n_mirror = n_mirror.next_sibling(c::XML_TAG_MIRROR)) {
		std::size_t l_mirror_no = n_mirror.attribute(c::XML_ATTR_NO).as_uint();
		result.set_spawn_position(l_mirror_no, string_to_position(n_mirror.attribute(c::XML_ATTR_POSITION).as_string()));
		result.set_spawn_schedule(l_mirror_no, n_mirror.attribute(c::XML_ATTR_SCHEDULE).as_uint());
		result.set_spawn_enemies(l_mirror_no, n_mirror.attribute(c::XML_ATTR_ENEMY_SET).as_uint());
	}

	return result;
}

position skc::xml::string_to_position(const std::string& p_position_str) {
	auto l_coords{ klib::util::string_split<int>(p_position_str, ',') };
	return position(l_coords.at(0), l_coords.at(1));
}

skc::Wall skc::xml::int_to_wall_type(int p_wall_type_no) {
	if (p_wall_type_no == 0)
		return skc::Wall::None;
	else if (p_wall_type_no == 1)
		return skc::Wall::Brown;
	else if (p_wall_type_no == 2)
		return skc::Wall::White;
	else
		return skc::Wall::Brown_white;
}

skc::Game_metadata skc::xml::load_metadata_xml(const std::string p_folder, const std::string p_filename) {
	pugi::xml_document doc = load_xml_file(p_folder, p_filename);
	skc::Game_metadata result;

	auto n_meta = doc.child(c::XML_TAG_META);

	auto n_meta_tiles = n_meta.child(c::XML_TAG_LEVEL_META_ITEMS);
	for (auto n_meta_tile = n_meta_tiles.child(c::XML_TAG_LEVEL_META_ITEM);
		n_meta_tile; n_meta_tile = n_meta_tile.next_sibling(c::XML_TAG_LEVEL_META_ITEM)) {
		result.m_meta_tiles.insert(std::make_pair(
			n_meta_tile.attribute(c::XML_ATTR_NO).as_uint(),
			string_to_position(n_meta_tile.attribute(c::XML_ATTR_POSITION).as_string())
		));
	}

	auto n_schedules = n_meta.child(c::XML_TAG_SCHEDULES);
	for (auto n_schedule = n_schedules.child(c::XML_ATTR_SCHEDULE); n_schedule;
		n_schedule = n_schedule.next_sibling(c::XML_ATTR_SCHEDULE)) {
		auto l_int_sched{ klib::util::string_split<int>(n_schedule.attribute(c::XML_ATTR_SCHEDULE).as_string(), ',') };
		std::vector<bool> l_bool_sched;
		for (int l_n : l_int_sched)
			l_bool_sched.push_back(l_n != 0);
		result.m_drop_schedules.push_back(l_bool_sched);
	}

	auto n_enemy_sets = n_meta.child(c::XML_TAG_ENEMY_SETS);
	for (auto n_enemy_set = n_enemy_sets.child(c::XML_ATTR_ENEMY_SET); n_enemy_set;
		n_enemy_set = n_enemy_set.next_sibling(c::XML_ATTR_ENEMY_SET)) {
		result.m_drop_enemies.push_back(
			klib::util::string_split<byte>(n_enemy_set.attribute(c::XML_ATTR_ENEMY_SET).as_string(), ',')
		);
	}

	return result;
}

std::string skc::xml::get_region_code(const pugi::xml_node p_node, const std::vector<byte> p_rom_bytes) {

	const auto is_region_rule_adhered = [](const std::string& p_rule, const std::vector<byte>& p_rom_bytes) -> bool {
		auto l_rules = klib::util::string_split_strings(p_rule, ',');

		for (const auto& l_rule : l_rules) {
			auto l_kv_pair{ klib::util::string_split<std::size_t>(l_rule, ':') };

			if (p_rom_bytes.at(l_kv_pair.at(0)) !=
				static_cast<byte>(l_kv_pair.at(1)))
				return false;
		}

		return true;
	};

	for (auto n_region = p_node.child(c::XML_ATTR_REGION);
		n_region; n_region = n_region.next_sibling(c::XML_ATTR_REGION)) {

		if (is_region_rule_adhered(n_region.attribute(c::XML_ATTR_RULE).as_string(),
			p_rom_bytes))
			return n_region.attribute(c::XML_ATTR_NAME).as_string();
	}

	// no region rule found, use hard-coded rules
	constexpr std::size_t OFFSET_REGION_DIFF{ 0x0bf2 };
	constexpr byte REGION_EU_VALUE{ 0xff };
	constexpr byte REGION_JP_VALUE{ 0xea };
	constexpr byte REGION_US_VALUE{ 0x00 };

	byte l_chk{ p_rom_bytes.at(OFFSET_REGION_DIFF) };

	if (l_chk == REGION_EU_VALUE)
		return c::REGION_EU;
	else if (l_chk == REGION_JP_VALUE)
		return c::REGION_JP;
	else
		return c::REGION_US;
}

bool skc::xml::node_has_region_attribute(const pugi::xml_node p_node) {
	std::string l_code = p_node.attribute(c::XML_ATTR_REGION).as_string();
	return !l_code.empty();
}

bool skc::xml::node_has_region_code(const pugi::xml_node p_node, const std::string& p_region_code) {
	std::string l_codes_str = p_node.attribute(c::XML_ATTR_REGION).as_string();
	auto l_codes = klib::util::string_split_strings(l_codes_str, ',');
	for (const auto& l_code : l_codes)
		if (l_code == p_region_code)
			return true;

	return false;
}

std::string skc::xml::node_region_best_match(const pugi::xml_node p_node,
	const std::string& p_region_code,
	const std::string& p_xml_tag, const std::string p_value_attr) {
	std::string result;

	for (auto n_child = p_node.child(p_xml_tag.c_str());
		n_child; n_child = n_child.next_sibling(p_xml_tag.c_str())) {
		if ((result.empty() && !node_has_region_code(n_child, p_region_code)) ||
			node_has_region_code(n_child, p_region_code))
			result = n_child.attribute(p_value_attr.c_str()).as_string();
	}

	return result;
}
