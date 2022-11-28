#include "SKC_Level.h"
#include "skc_constants/Constants_level.h"
#include "./common/klib/klib_file.h"

using byte = unsigned char;

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

skc::Level::Level(const std::vector<byte>& p_browns,
	const std::vector<byte>& p_whites) {

	for (std::size_t i{ 0 }; i < p_browns.size(); i += 2)
		add_blocks(p_browns[i], p_browns.at(i + 1),
			p_whites.at(i), p_whites.at(i + 1));
}

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

skc::Wall skc::Level::get_wall_type(int p_x, int p_y) const {
	return m_tiles.at(p_y).at(p_x);
}

std::pair<byte, byte> skc::Level::get_player_start_pos(void) const {
	return m_fixed_start_pos;
}

// setters
void skc::Level::set_player_start_pos(byte p_x, byte p_y) {
	m_fixed_start_pos = std::make_pair(p_x, p_y);
}

// static functions
bool skc::Level::is_item_constellation(byte p_item_no) {
	return p_item_no >= c::ITEM_CONSTELLATION_ARIES &&
		p_item_no <= c::ITEM_CONSTELLATION_SAGITTARIUS;
}
