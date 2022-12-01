#ifndef SKC_CONFIG_H
#define SKC_CONFIG_H

#include <vector>
using byte = unsigned char;

namespace skc {

	class SKC_Config {
		std::size_t m_offset_gfx, m_offset_blocks, m_offset_enemy_table, m_offset_item_table;
		unsigned int m_level_count, m_gfx_tile_count;
		int m_rom_ram_diff;
		void load_config_xml(void);
		std::vector<byte> m_rom_data;
		std::vector<std::size_t> m_level_tilesets;

	public:
		SKC_Config(const std::vector<byte>& p_rom_data);
		const std::vector<byte>& get_rom_data(void) const;

		std::size_t get_offset_gfx(void) const;
		std::size_t get_offset_block_data(void) const;
		std::size_t get_offset_enemy_table_lo(void) const;
		std::size_t get_offset_enemy_table_hi(void) const;
		std::size_t get_offset_item_table_lo(void) const;
		std::size_t get_offset_item_table_hi(void) const;
		unsigned int get_level_count(void) const;
		unsigned int get_nes_tile_count(void) const;
		std::size_t get_rom_address_from_ram(std::size_t p_ram_address) const;
		std::size_t get_level_tileset(std::size_t p_level_no) const;
	};

}

#endif
