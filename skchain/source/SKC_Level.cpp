#include "SKC_Level.h"
#include "skc_constants/Constants_level.h"
#include "./common/klib/klib_util.h"
#include <set>
#include <stdexcept>

using byte = unsigned char;
using position = std::pair<int, int>;

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
	if (m_element_no >= c::ITEM_COPY_INDICATOR_MIN)
		return m_element_no;
	else
		return m_element_no % 0x40;
}

void skc::Level_element::set_item_hidden(bool p_value) {
	klib::util::set_bit(m_element_no, 6, p_value);
	if (p_value)
		set_item_in_block(false);
}

void skc::Level_element::set_item_in_block(bool p_value) {
	klib::util::set_bit(m_element_no, 7, p_value);
	if (p_value)
		set_item_hidden(false);
}

void skc::Level_element::set_position(const position& p_pos) {
	m_position = p_pos;
}

skc::Level::Level(void) :
	m_key_status{ c::DEFAULT_KEY_STATUS },
	m_spawn_rate{ c::DEFAULT_SPAWN_RATE },
	m_tileset_no{ 0 }
{ }

void skc::Level::load_block_data(const std::vector<byte>& p_bytes, std::size_t p_offset) {
	auto l_bblocks = klib::util::bytes_to_bitmask(p_bytes, c::LEVEL_W, c::LEVEL_H, p_offset);
	auto l_wblocks = klib::util::bytes_to_bitmask(p_bytes, c::LEVEL_W, c::LEVEL_H, p_offset + c::TILE_BITMASK_BYTE_SIZE);

	for (int j{ 0 }; j < c::LEVEL_H; ++j) {
		std::vector<Wall> l_row;
		for (int i{ 0 }; i < c::LEVEL_W; ++i)
			l_row.push_back(walls_to_wall_type(
				l_bblocks.at(j).at(i),
				l_wblocks.at(j).at(i)
			));

		m_tiles.push_back(l_row);
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
	m_key_status = p_bytes.at(p_offset + c::ITEM_OFFSET_KEY_STATUS);
	m_fixed_door_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_DOOR_POS));
	m_fixed_key_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_KEY_POS));
	m_fixed_start_pos = get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_START_POS));

	m_demon_mirrors.push_back(skc::Demon_mirror(get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN02)),
		p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN02_SCHEDULE),
		p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN02_ENEMIES)));
	m_demon_mirrors.push_back(skc::Demon_mirror(get_position_from_byte(p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN01)),
		p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN01_SCHEDULE),
		p_bytes.at(p_offset + c::ITEM_OFFSET_SPAWN01_ENEMIES)));

	for (std::size_t i{ p_offset + c::ITEM_OFFSET_ITEM_DATA }; ; i += 2) {
		byte l_next_elm{ p_bytes.at(i) };
		// found a 0-terminator, end stream read
		if (is_item_delimiter(l_next_elm)) {
			m_tileset_no = (l_next_elm >> 2) & 3;
			break;
		}
		// if we hit a constellation, store it as an optional in the level metadata (as 0 or 1 of these can be present)
		// this will also act as end-of-stream
		if (is_item_constellation(l_next_elm)) {
			m_constellation = Level_element(skc::Element_type::Item,
				get_position_from_byte(p_bytes.at(i + 1)),
				l_next_elm);
			m_tileset_no = (get_constellation_no() >> 2) & 3;
			break;
		}
		// found a regular item, store it in the list
		else if (l_next_elm < c::ITEM_COPY_INDICATOR_MIN) {
			m_items.push_back(Level_element(skc::Element_type::Item,
				get_position_from_byte(p_bytes.at(i + 1)),
				l_next_elm));
		}
		// found a repeating item, store them all in the list
		else {
			std::size_t l_repeat_count{ static_cast<std::size_t>(l_next_elm - c::ITEM_COPY_INDICATOR_MIN) + 1 };
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

byte skc::Level::get_constellation_no(void) const {
	return m_constellation.value().get_element_no();
}

position skc::Level::get_constellation_pos(void) const {
	return m_constellation.value().get_position();
}

byte skc::Level::get_spawn_rate(void) const {
	return m_spawn_rate;
}

byte skc::Level::get_tileset_no(void) const {
	return m_tileset_no;
}

byte skc::Level::get_item_delimiter(void) const {
	return c::ITEM_DELIMITER_MIN + 4 * m_tileset_no;
}

const std::vector<skc::Level_element>& skc::Level::get_items(void) const {
	return m_items;
}

const std::vector<skc::Level_element>& skc::Level::get_enemies(void) const {
	return m_enemies;
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
	if (has_constellation()) {
		byte four{ 4 };
		int l_base_constellation{ (get_constellation_no() - c::ITEM_CONSTELLATION_MIN) % 4 };
		set_constellation(c::ITEM_CONSTELLATION_MIN + 4 * p_tileset_no, get_constellation_pos());
	}
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

void skc::Level::set_item_hidden(int p_index, bool p_value) {
	m_items.at(p_index).set_item_hidden(p_value);
}

void skc::Level::set_item_in_block(int p_index, bool p_value) {
	m_items.at(p_index).set_item_in_block(p_value);
}

void skc::Level::set_item_position(int p_index, const position& l_pos) {
	m_items.at(p_index).set_position(l_pos);
}

void skc::Level::set_enemy_position(int p_index, const position& l_pos) {
	m_enemies.at(p_index).set_position(l_pos);
}

// static functions
bool skc::Level::is_item_constellation(byte p_item_no) {
	return p_item_no >= c::ITEM_CONSTELLATION_MIN &&
		p_item_no <= c::ITEM_CONSTELLATION_MAX;
}

bool skc::Level::is_item_in_block(byte p_item_no) {
	return klib::util::get_bit(p_item_no, 7);
}

bool skc::Level::is_item_hidden(byte p_item_no) {
	return klib::util::get_bit(p_item_no, 6);
}

std::vector<std::size_t> skc::Level::get_item_indexes(byte p_item_no, std::set<std::size_t>& p_ignored_indexes) const {
	std::vector<std::size_t> result;

	for (std::size_t i{ 0 }; result.size() < c::ITEM_COMPRESS_MAX_COUNT
		&& i < m_items.size(); ++i)
		if (p_ignored_indexes.find(i) == end(p_ignored_indexes) &&
			m_items[i].get_element_no() == p_item_no) {
			result.push_back(i);
			p_ignored_indexes.insert(i);
		}

	return result;
}

skc::Wall skc::Level::walls_to_wall_type(bool p_bblock, bool p_wblock) {
	if (p_bblock && p_wblock)
		return skc::Wall::Brown_white;
	else if (p_bblock)
		return skc::Wall::Brown;
	else if (p_wblock)
		return skc::Wall::White;
	else
		return skc::Wall::None;
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

	const std::vector<byte> l_brown_bytes{ klib::util::bitmask_to_bytes(
		klib::util::vec2_to_bitmask(m_tiles, {Wall::Brown, Wall::Brown_white})
	) };
	const std::vector<byte> l_white_bytes{ klib::util::bitmask_to_bytes(
		klib::util::vec2_to_bitmask(m_tiles, {Wall::White, Wall::Brown_white})
	) };

	klib::util::append_vector(result, l_brown_bytes);
	klib::util::append_vector(result, l_white_bytes);

	return result;
}

std::vector<byte> skc::Level::get_item_bytes(const std::vector<byte>& p_ignore_item_elements) const {
	std::vector<byte> result;
	result.push_back(m_demon_mirrors.at(1).m_schedule_no);
	result.push_back(m_demon_mirrors.at(0).m_schedule_no);
	result.push_back(m_demon_mirrors.at(1).m_monster_set_no);
	result.push_back(m_demon_mirrors.at(0).m_monster_set_no);

	std::set<std::size_t> l_handled_offsets;

	// if we ignore any elements, set them as handled before starting
	for (std::size_t i{ 0 }; i < m_items.size(); ++i)
		for (byte b : p_ignore_item_elements)
			if (m_items[i].get_element_no() == b)
				l_handled_offsets.insert(i);

	// level metadata
	result.push_back(m_key_status);
	result.push_back(get_byte_from_position(m_fixed_door_pos));
	result.push_back(get_byte_from_position(m_fixed_key_pos));
	result.push_back(get_byte_from_position(m_fixed_start_pos));

	// enemy spawn point values (spawn rate set in the enemy data)
	result.push_back(get_byte_from_position(m_demon_mirrors.at(0).m_position));
	result.push_back(get_byte_from_position(m_demon_mirrors.at(1).m_position));

	// add all item data, and apply "0xC"-compression whenever more than one item of the same type exists
	for (std::size_t i{ 0 }; i < m_items.size(); ++i)
		if (l_handled_offsets.find(i) == end(l_handled_offsets)) {
			byte l_item_no = m_items[i].get_element_no();
			auto l_indexes = get_item_indexes(l_item_no, l_handled_offsets);

			// when two items are the same, it does not matter if we compress or not
			// the original game seems to prefer compression when item count > 1, so we will go with the same cutoff
			if (l_indexes.size() > 1) {
				byte l_repeat_count = c::ITEM_COPY_INDICATOR_MIN + static_cast<byte>(l_indexes.size() - 1);
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
		result.push_back(get_item_delimiter());

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

std::vector<byte> skc::Level::get_item_bitmask_bytes(byte p_item_element_no) const {
	std::vector<std::vector<bool>> l_item_bitmask(c::LEVEL_H, std::vector<bool>(c::LEVEL_W, false));

	for (const auto& l_item : m_items)
		if (l_item.get_element_no() == p_item_element_no) {
			auto l_pos = l_item.get_position();
			if (l_pos.first >= 0 && l_pos.first < c::LEVEL_W && l_pos.second >= 0 && l_pos.second < c::LEVEL_H)
				l_item_bitmask[l_pos.second][l_pos.first] = true;
		}

	return klib::util::bitmask_to_bytes(l_item_bitmask);
}

bool skc::Level::is_item_delimiter(byte p_value) {
	return (p_value == 0x00) || (p_value >= c::ITEM_DELIMITER_MIN && p_value < c::ITEM_CONSTELLATION_MIN);
}

bool skc::Level::is_key_hidden(void) const {
	return m_key_status >= 0x80;
}

bool skc::Level::is_key_in_block(void) const {
	return m_key_status >= 0x40 && m_key_status < 0x80;
}

bool skc::Level::is_key_removed(void) const {
	return m_fixed_key_pos.second < 0;
}

void skc::Level::set_key_hidden(bool p_value) {
	if (p_value)
		m_key_status = 0x80;
	else
		m_key_status = 0x00;
}

void skc::Level::set_key_in_block(bool p_value) {
	if (p_value)
		m_key_status = 0x40;
	else
		m_key_status = 0x00;
}

void skc::Level::set_key_removed(void) {
	m_fixed_key_pos = std::make_pair(-1, 0);
	set_key_hidden(true);
}

bool skc::Level::is_door_removed(void) const {
	return m_fixed_door_pos.second < 0;
}

void skc::Level::set_door_removed(void) {
	m_fixed_door_pos = std::make_pair(-1, 0);
}

int skc::Level::get_item_count(void) const {
	return static_cast<int>(m_items.size());
}

int skc::Level::get_enemy_count(void) const {
	return static_cast<int>(m_enemies.size());
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
