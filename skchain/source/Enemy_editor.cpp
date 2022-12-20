#include "Enemy_editor.h"

#include <string>
#include <stdexcept>

void skc::Enemy_editor::generate_maps(void) {
	for (std::size_t i{ 0 }; i < m_direction_bundles.size(); ++i)
		for (byte l_enemy_no : m_direction_bundles[i])
			m_db_map.insert(std::make_pair(l_enemy_no, i));

	for (std::size_t i{ 0 }; i < m_speed_bundles.size(); ++i)
		for (byte l_enemy_no : m_speed_bundles[i])
			m_sb_map.insert(std::make_pair(l_enemy_no, i));
}

void skc::Enemy_editor::add_speed_bundle(const std::vector<byte>& p_enemy_list) {
	m_speed_bundles.push_back(p_enemy_list);
}

void skc::Enemy_editor::add_direction_bundle(const std::vector<byte>& p_enemy_list) {
	m_direction_bundles.push_back(p_enemy_list);
}

bool skc::Enemy_editor::has_direction(byte p_enemy_no) const {
	return m_db_map.find(p_enemy_no) != end(m_db_map);
}

bool skc::Enemy_editor::has_speed(byte p_enemy_no) const {
	return m_sb_map.find(p_enemy_no) != end(m_sb_map);
}

std::size_t skc::Enemy_editor::get_direction_size(byte p_enemy_no) const {
	return m_direction_bundles[m_db_map.at(p_enemy_no)].size();
}

std::size_t skc::Enemy_editor::get_speed_size(byte p_enemy_no) const {
	return m_speed_bundles[m_sb_map.at(p_enemy_no)].size();
}

byte skc::Enemy_editor::get_enemy_no_by_speed(byte p_enemy_no, std::size_t p_speed) const {
	return get_enemy_no(p_speed, m_speed_bundles[m_sb_map.at(p_enemy_no)]);
}

byte skc::Enemy_editor::get_enemy_no_by_direction(byte p_enemy_no, std::size_t p_direction) const {
	return get_enemy_no(p_direction, m_direction_bundles[m_db_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_direction(byte p_enemy_no) const {
	return get_enemy_property(p_enemy_no, m_direction_bundles[m_db_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_speed(byte p_enemy_no) const {
	return get_enemy_property(p_enemy_no, m_speed_bundles[m_sb_map.at(p_enemy_no)]);
}

std::size_t skc::Enemy_editor::get_enemy_property(byte p_enemy_no,
	const std::vector<byte>& p_bundle) const {

	for (std::size_t i{ 0 }; i < p_bundle.size(); ++i)
		if (p_bundle[i] == p_enemy_no)
			return i;

	throw std::runtime_error(c::TXT_ERR_MISCONFIG);

}

byte skc::Enemy_editor::get_enemy_no(std::size_t p_property_no,
	const std::vector<byte>& p_bundle) const {
	return p_bundle[p_property_no];

	throw std::runtime_error(c::TXT_ERR_MISCONFIG);
}
