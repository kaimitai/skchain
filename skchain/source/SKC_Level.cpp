#include "SKC_Level.h"
#include "skc_constants/Constants_level.h"
#include "./common/klib/klib_file.h"

using byte = unsigned char;

skc::Level_element::Level_element(skc::Element_type p_type, position p_position, byte p_element_no) :
	m_type{ p_type }, m_position{ p_position }, m_element_no{ p_element_no }
{ }

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

skc::Level::Level(void) : m_key_status{ 0x01 }, m_spawn_rate{ 0x02 }
{ }

std::string skc::Level::get_blocks(void) const {
	std::string result;

	for (const auto& row : m_tiles) {
		for (const auto wall : row) {
			if (wall == skc::Wall::White)
				result.push_back('#');
			else if (wall == skc::Wall::Brown)
				result.push_back('+');
			else if (wall == skc::Wall::Brown_white)
				result.push_back('X');
			else
				result.push_back('.');
		}

		result.push_back('\n');
	}

	return result;
}

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
			m_elements.push_back(Level_element(skc::Element_type::Enemy,
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

	for (std::size_t i{ p_offset + c::ITEM_OFFSET_ITEM_DATA }; ; i += 2) {
		byte l_next_elm{ p_bytes.at(i) };
		// found a 0-terminator, end stream read
		if (l_next_elm == 0x00)
			break;
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
			m_elements.push_back(Level_element(skc::Element_type::Item,
				get_position_from_byte(p_bytes.at(i + 1)),
				l_next_elm));
		}
		// found a repeating item, store them all in the list
		else {
			std::size_t l_repeat_count{ static_cast<std::size_t>(l_next_elm % 16) + 1 };
			l_next_elm = p_bytes.at(i + 1);
			for (std::size_t j{ 0 }; j < l_repeat_count; ++j)
				m_elements.push_back(Level_element(skc::Element_type::Item,
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

// setters
void skc::Level::set_player_start_pos(int p_x, int p_y) {
	m_fixed_start_pos = std::make_pair(p_x, p_y);
}

// static functions
bool skc::Level::is_item_constellation(byte p_item_no) {
	return p_item_no >= c::ITEM_CONSTELLATION_ARIES &&
		p_item_no <= c::ITEM_CONSTELLATION_SAGITTARIUS;
}

std::pair<int, int> skc::Level::get_position_from_byte(byte b) {
	int l_x{ b % 0x10 };
	int l_y{ b / 0x10 };
	return std::make_pair(l_x, l_y - 1);
}
