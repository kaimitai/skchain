#include "Xml_helper.h"
#include <filesystem>
#include <stdexcept>
#include "./../skc_constants/Constants_application.h"
#include "./../skc_constants/Constants_xml.h"
#include "./../common/klib/klib_util.h"

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

	n_level.append_attribute(c::XML_ATTR_SPAWN_RATE);
	n_level.attribute(c::XML_ATTR_SPAWN_RATE).set_value(p_level.get_spawn_rate());
	n_level.append_attribute(c::XML_ATTR_SPAWN01);
	n_level.attribute(c::XML_ATTR_SPAWN01).set_value(p_level.get_spawn01());
	n_level.append_attribute(c::XML_ATTR_SPAWN02);
	n_level.attribute(c::XML_ATTR_SPAWN02).set_value(p_level.get_spawn02());

	if (p_level.has_constellation()) {
		n_level.append_attribute(c::XML_ATTR_CONSTELLATION_NO);
		n_level.attribute(c::XML_ATTR_CONSTELLATION_NO).set_value(p_level.get_constellation_no());
		n_level.append_attribute(c::XML_ATTR_CONSTELLATION_POSITION);
		n_level.attribute(c::XML_ATTR_CONSTELLATION_POSITION).set_value(get_position_string(p_level.get_constellation_pos()).c_str());
	}

	n_level.append_attribute(c::XML_ATTR_ITEM_HEADER);
	n_level.attribute(c::XML_ATTR_ITEM_HEADER).set_value(klib::util::string_join<byte>(p_level.get_item_header(), ',').c_str());

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

	const auto& l_elements{ p_level.get_elements() };

	auto n_items = n_level.append_child(c::XML_TAG_ITEMS);
	int l_counter{ 0 };
	for (const auto& element : l_elements) {
		if (element.get_element_type() == skc::Element_type::Item) {
			auto n_item = n_items.append_child(c::XML_TAG_ITEM);
			n_item.append_attribute(c::XML_ATTR_NO);
			n_item.attribute(c::XML_ATTR_NO).set_value(l_counter++);
			n_item.append_attribute(c::XML_ATTR_ELEMENT_NO);
			n_item.attribute(c::XML_ATTR_ELEMENT_NO).set_value(element.get_element_no());
			n_item.append_attribute(c::XML_ATTR_POSITION);
			n_item.attribute(c::XML_ATTR_POSITION).set_value(get_position_string(element.get_position()).c_str());
		}
	}

	auto n_enemies = n_level.append_child(c::XML_TAG_ENEMIES);
	l_counter = 0;
	for (const auto& element : l_elements) {
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
