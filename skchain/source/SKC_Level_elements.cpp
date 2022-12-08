#include "SKC_Level.h"

skc::Demon_mirror::Demon_mirror(const position& p_position, byte p_schedule_no, byte p_monster_set_no) :
	m_position{ p_position }, m_schedule_no{ p_schedule_no }, m_monster_set_no{ p_monster_set_no }
{}

position skc::Level::get_spawn_position(std::size_t p_index) const {
	return m_demon_mirrors.at(p_index).m_position;
}

byte skc::Level::get_spawn_schedule(std::size_t p_index) const {
	return m_demon_mirrors.at(p_index).m_schedule_no;
}

byte skc::Level::get_spawn_enemies(std::size_t p_index) const {
	return m_demon_mirrors.at(p_index).m_monster_set_no;
}

void skc::Level::set_spawn_position(std::size_t p_index, const position& p_pos) {
	m_demon_mirrors.at(p_index).m_position = p_pos;
}

void skc::Level::set_spawn_schedule(std::size_t p_index, byte p_value) {
	m_demon_mirrors.at(p_index).m_schedule_no = p_value;
}

void skc::Level::set_spawn_enemies(std::size_t p_index, byte p_value) {
	m_demon_mirrors.at(p_index).m_monster_set_no = p_value;
}
