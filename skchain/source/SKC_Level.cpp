#include "SKC_Level.h"
#include "skc_constants/Constants_level.h"
#include "./common/klib/klib_file.h"
#include <set>
#include <stdexcept>

using byte = unsigned char;

skc::Level_element::Level_element(skc::Element_type p_type, position p_position, byte p_element_no) :
	m_type{ p_type }, m_position{ p_position }, m_element_no{ p_element_no }
{ }

position skc::Level_element::get_position(void) const {
	return m_position;
}

skc::Element_type skc::Level_element::get_element_type(void) const {
	return m_type;
}

byte skc::Level_element::get_element_no(void) const {
	return m_element_no;
}

byte skc::Level_element::get_item_no(void) const {
	if (m_element_no >= 0xc0)
		return m_element_no;
	else
		return m_element_no % 0x40;
}

void skc::Level::add_blocks(byte p_b1, byte p_b2, byte p_w1, byte p_w2) {
	std::vector<Wall> l_row;

	for (int b{ 0 }; b < 2; ++b) {
		byte l_b = (b == 0 ? p_b1 : p_b2);
		byte l_w = (b == 0 ? p_w1 : p_w2);

		for (int i{ 7 }; i >= 0; --i) {
			bool l_is_b = klib::file::get_bit(l_b, i);
			bool l_is_w = klib::file::get_bit(l_w, i);

			if (l_is_b && l_is_w)
				l_row.push_back(skc::Wall::Brown_white);
			else if (l_is_b)
				l_row.push_back(skc::Wall::Brown);
			else if (l_is_w)
				l_row.push_back(skc::Wall::White);
			else
				l_row.push_back(skc::Wall::None);
		}
	}

	m_tiles.push_back(l_row);
}

skc::Level::Level(void) :
	m_key_status{ c::DEFAULT_KEY_STATUS },
	m_spawn_rate{ c::DEFAULT_SPAWN_RATE },
	m_spawn01{ c::DEFAULT_SPAWN_VALUE },
	m_spawn02{ c::DEFAULT_SPAWN_VALUE },
	m_tileset_no{ 0 }
{ }

void skc::Level::load_block_data(const std::vector<byte>& p_bytes, std::size_t p_offset) {
	for (std::size_t i{ 0 }; i < 24; i += 2) {
		add_blocks(p_bytes.at(p_offset + i), p_bytes.at(p_offset + i + 1),
			p_bytes.at(p_offset + 24 + i), p_bytes.at(p_offset + 24 + i + 1));
	}
}

void skc::Level::load_enemy_data(const std::vector<byte>& p_bytes, std::size_t p_offset) {
	m_spawn_rate = p_bytes.at(p_offset);

	for (std::size_t i{ 1 }; ; i += 2) {
		byte l_enemy_no = p_bytes.at(p_offset + i);
		if (l_enemy_no == 0)
			break;
		else
			m_enemies.push_back(Level_element(skc::Element_type::Enemy,
				get_position_from_byte(p_bytes.at(p_offset + i + 1)),
				l_enemy_no));
	}
}

void skc::Level::load_item_data(const std::vector<byte>& p_bytes, std::size_t p_offset) {
	m_item_header = std::vector<byte>(begin(p_bytes) + p_offset,
		begin(p_bytes) + p_offset + c::ITEM_OFFSET_KEY_STATUS);

	m_key_status = p_bytes.at(p_offset + c::ITEM_OFFSET_KEY_STATUS);
	m_fixed_door_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_DOOR_POS));
	m_fixed_key_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_KEY_POS));
	m_fixed_start_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_START_POS));
	m_spawn01 = p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN01);
	m_spawn02 = p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN02);

	for (std::size_t i{ p_offset + c::ITEM_OFFSET_ITEM_DATA }; ; i += 2) {
		byte l_next_elm{ p_bytes.at(i) };
		// found a 0-terminator, end stream read
		if (is_item_delimiter(l_next_elm)) {
			m_item_eof = l_next_elm;
			break;
		}
		// if we hit a constellation, store it as an optional in the level metadata (as 0 or 1 of these can be present)
		// this will also act as end-of-stream
		if (is_item_constellation(l_next_elm)) {
			m_constellation = Level_element(skc::Element_type::Item,
				get_position_from_byte(p_bytes.at(i + 1)),
				l_next_elm);
			break;
		}
		// found a regular item, store it in the list
		else if (l_next_elm / 16 != 0xc) {
			m_items.push_back(Level_element(skc::Element_type::Item,
				get_position_from_byte(p_bytes.at(i + 1)),
				l_next_elm));
		}
		// found a repeating item, store them all in the list
		else {
			std::size_t l_repeat_count{ static_cast<std::size_t>(l_next_elm % 16) + 1 };
			l_next_elm = p_bytes.at(i + 1);
			for (std::size_t j{ 0 }; j < l_repeat_count; ++j)
				m_items.push_back(Level_element(skc::Element_type::Item,
					get_position_from_byte(p_bytes.at(i + 2 + j)),
					l_next_elm));
			i += l_repeat_count;
		}
	}
}

skc::Wall skc::Level::get_wall_type(int p_x, int p_y) const {
	return m_tiles.at(p_y).at(p_x);
}

std::pair<int, int> skc::Level::get_player_start_pos(void) const {
	return m_fixed_start_pos;
}

position skc::Level::get_door_pos(void) const {
	return m_fixed_door_pos;
}

position skc::Level::get_key_pos(void) const {
	return m_fixed_key_pos;
}

byte skc::Level::get_key_status(void) const {
	return m_key_status;
}

bool skc::Level::has_constellation(void) const {
	return m_constellation != std::nullopt;
}

bool skc::Level::has_item_eof(void) const {
	return m_item_eof != std::nullopt;
}

byte skc::Level::get_constellation_no(void) const {
	return m_constellation.value().get_element_no();
}

byte skc::Level::get_item_eof(void) const {
	return m_item_eof.value();
}

position skc::Level::get_constellation_pos(void) const {
	return m_constellation.value().get_position();
}

byte skc::Level::get_spawn_rate(void) const {
	return m_spawn_rate;
}

byte skc::Level::get_spawn01(void) const {
	return m_spawn01;
}

byte skc::Level::get_spawn02(void) const {
	return m_spawn02;
}

byte skc::Level::get_tileset_no(void) const {
	return m_tileset_no;
}

const std::vector<skc::Level_element>& skc::Level::get_items(void) const {
	return m_items;
}

const std::vector<skc::Level_element>& skc::Level::get_enemies(void) const {
	return m_enemies;
}

const std::vector<byte>& skc::Level::get_item_header(void) const {
	return m_item_header;
}

// setters
void skc::Level::set_player_start_pos(const std::pair<int, int>& p_pos) {
	m_fixed_start_pos = p_pos;
}

void skc::Level::set_key_pos(const std::pair<int, int>& p_pos) {
	m_fixed_key_pos = p_pos;
}

void skc::Level::set_door_pos(const std::pair<int, int>& p_pos) {
	m_fixed_door_pos = p_pos;
}

void skc::Level::set_constellation(byte p_constellation_no, const std::pair<int, int>& p_pos) {
	m_constellation = Level_element(Element_type::Item, p_pos, p_constellation_no);
}

void skc::Level::set_block(skc::Wall p_wall_type, const std::pair<int, int>& p_pos) {
	m_tiles.at(p_pos.second).at(p_pos.first) = p_wall_type;
}

void skc::Level::set_tileset_no(byte p_tileset_no) {
	m_tileset_no = p_tileset_no;
}

void skc::Level::add_item(byte p_item_no, const position& p_pos) {
	m_items.push_back(skc::Level_element(skc::Element_type::Item, p_pos, p_item_no));
}

void skc::Level::add_enemy(byte p_item_no, const position& p_pos) {
	m_enemies.push_back(skc::Level_element(skc::Element_type::Enemy, p_pos, p_item_no));
}

void skc::Level::delete_item(int p_index) {
	m_items.erase(begin(m_items) + p_index);
}

void skc::Level::delete_enemy(int p_index) {
	m_enemies.erase(begin(m_enemies) + p_index);
}

// static functions
bool skc::Level::is_item_constellation(byte p_item_no) {
	return p_item_no >= c::ITEM_CONSTELLATION_ARIES &&
		p_item_no <= c::ITEM_CONSTELLATION_SAGITTARIUS;
}

bool skc::Level::is_item_in_block(byte p_item_no) {
	return klib::file::get_bit(p_item_no, 7);
}

bool skc::Level::is_item_hidden(byte p_item_no) {
	return klib::file::get_bit(p_item_no, 6);
}

std::vector<std::size_t> skc::Level::get_item_indexes(byte p_item_no, std::set<std::size_t>& p_ignored_indexes) const {
	std::vector<std::size_t> result;

	for (std::size_t i{ 0 }; result.size() <= 16 && i < m_items.size(); ++i)
		if (p_ignored_indexes.find(i) == end(p_ignored_indexes) &&
			m_items[i].get_element_no() == p_item_no) {
			result.push_back(i);
			p_ignored_indexes.insert(i);
		}

	return result;
}

std::pair<int, int> skc::Level::get_position_from_byte(byte b) {
	int l_x{ b % 0x10 };
	int l_y{ b / 0x10 };
	return std::make_pair(l_x, l_y - 1);
}

byte skc::Level::get_byte_from_position(const std::pair<int, int>& p_position) {
	return static_cast<byte>(p_position.second + 1) * 16 + static_cast<byte>(p_position.first);
}

std::vector<byte> skc::Level::get_block_bytes(void) const {
	std::vector<byte> result;

	const auto get_block_bit = [](const Wall p_target, int p_pos) -> int {
		return 0;
	};

	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; i += 8) {
			byte l_byte{ 0 };
			for (int b{ 0 }; b < 8; ++b) {
				auto l_wall = m_tiles.at(j).at(i + b);
				byte l_bit = (l_wall == Wall::Brown || l_wall == Wall::Brown_white);
				l_byte *= 2;
				l_byte += l_bit;
			}
			result.push_back(l_byte);
		}

	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; i += 8) {
			byte l_byte{ 0 };
			for (int b{ 0 }; b < 8; ++b) {
				auto l_wall = m_tiles.at(j).at(i + b);
				byte l_bit = (l_wall == Wall::White || l_wall == Wall::Brown_white);
				l_byte *= 2;
				l_byte += l_bit;
			}
			result.push_back(l_byte);
		}

	return result;
}

std::vector<byte> skc::Level::get_item_bytes(void) const {
	std::vector<byte> result(m_item_header);

	// level metadata
	result.push_back(m_key_status);
	result.push_back(get_byte_from_position(m_fixed_door_pos));
	result.push_back(get_byte_from_position(m_fixed_key_pos));
	result.push_back(get_byte_from_position(m_fixed_start_pos));

	// enemy spawn point values (spawn rate set in the enemy data)
	result.push_back(m_spawn01);
	result.push_back(m_spawn02);

	// add all item data, and apply "0xC"-compression whenever more than one item of the same type exists
	std::set<std::size_t> l_handled_offsets;
	for (std::size_t i{ 0 }; i < m_items.size(); ++i)
		if (l_handled_offsets.find(i) == end(l_handled_offsets)) {
			byte l_item_no = m_items[i].get_element_no();
			auto l_indexes = get_item_indexes(l_item_no, l_handled_offsets);

			// when two items are the same, it does not matter if we compress or not
			// the original game seems to prefer compression when item count > 1, so we will go with the same cutoff
			if (l_indexes.size() > 1) {
				byte l_repeat_count = 0xc0 + static_cast<byte>(l_indexes.size() - 1);
				result.push_back(l_repeat_count);
				result.push_back(l_item_no);
				for (std::size_t li{ 0 }; li < l_indexes.size(); ++li)
					result.push_back(get_byte_from_position(m_items[l_indexes[li]].get_position()));
			}
			else {
				for (std::size_t li{ 0 }; li < l_indexes.size(); ++li) {
					result.push_back(l_item_no);
					result.push_back(get_byte_from_position(m_items[l_indexes[li]].get_position()));
				}
			}
		}

	// add constellation if it exists, this will act as an end-of-stream indicator
	// if no constellation exists, 0x00 will terminate the stream
	if (has_constellation()) {
		result.push_back(m_constellation.value().get_element_no());
		result.push_back(get_byte_from_position(m_constellation.value().get_position()));
	}
	else
		result.push_back(has_item_eof() ? get_item_eof() : 0x00);

	return result;
}

std::vector<byte> skc::Level::get_enemy_bytes(void) const {
	std::vector<byte> result;
	// spwan rate is the first byte of the enemy stream
	result.push_back(m_spawn_rate);

	// add data (element number and position) for each enemy
	for (const auto& element : m_enemies) {
		result.push_back(element.get_element_no());
		result.push_back(get_byte_from_position(element.get_position()));
	}

	// append end-of-stream symbol
	result.push_back(0x00);
	return result;
}

bool skc::Level::is_item_delimiter(byte p_value) {
	return (p_value == 0) || (p_value >= 0xd0 && p_value < 0xf0);
}

bool skc::Level::is_key_hidden(void) const {
	return is_item_hidden(m_key_status);
}

bool skc::Level::is_key_in_block(void) const {
	return is_item_in_block(m_key_status);
}

bool skc::Level::is_key_removed(void) const {
	return m_fixed_key_pos.second < 0;
}

bool skc::Level::is_door_removed(void) const {
	return m_fixed_door_pos.second < 0;
}

int skc::Level::get_item_index(const position& p_pos) const {
	for (std::size_t i{ 0 }; i < m_items.size(); ++i)
		if (m_items[i].get_position() == p_pos)
			return static_cast<int>(i);

	return -1;
}

int skc::Level::get_enemy_index(const position& p_pos) const {
	for (std::size_t i{ 0 }; i < m_enemies.size(); ++i)
		if (m_enemies[i].get_position() == p_pos)
			return static_cast<int>(i);

	return -1;
}

bool skc::Level::has_item_at_position(const std::pair<int, int>& p_pos) const {
	return get_item_index(p_pos) != -1;
}

bool skc::Level::has_enemy_at_position(const std::pair<int, int>& p_pos) const {
	return get_enemy_index(p_pos) != -1;
}
