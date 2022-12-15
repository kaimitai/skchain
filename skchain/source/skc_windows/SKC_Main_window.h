#ifndef SKC_MAIN_WINDOW_H
#define SKC_MAIN_WINDOW_H

#include <SDL.h>
#include <utility>
#include <vector>
#include "./../common/klib/User_input.h"
#include "./../SKC_Config.h"
#include "./../SKC_Level.h"
#include "./../SKC_Gfx.h"

using byte = unsigned char;
using position = std::pair<int, int>;

namespace skc {

	class SKC_Main_window {
		SKC_Gfx m_gfx;
		std::vector<skc::Level> m_levels;
		std::size_t m_current_level;
		SDL_Texture* m_texture;

		std::size_t m_selected_type; // 0 - metadata, 1 - items, 2 - enemies
		std::vector<byte> m_selected_picker_tile;	 // sub-selection
		// selected board item - one entry per level, with one entry per type
		std::vector<std::vector<int>> m_board_selection;
		std::map<std::size_t, std::vector<std::pair<std::size_t, position>>> m_meta_tiles;
		std::vector<std::vector<bool>> m_drop_schedules;
		std::vector<std::vector<byte>> m_drop_enemies;

		bool is_selected_index_valid(void) const;
		int get_selected_index_count(void) const;
		void increase_selected_index(void);
		void decrease_selected_index(void);
		int get_selected_index(void) const;
		void set_selected_index(int p_index);
		void delete_selected_index(void);
		void reset_selections(std::size_t p_level_no);

		int get_tile_w(int p_screen_h) const;
		void draw_tile(SDL_Renderer* p_rnd, SDL_Texture* p_texture, int p_x, int p_y, bool p_transp = false) const;
		void generate_texture(SDL_Renderer* p_rnd, const SKC_Config& p_config);

		// ui stuff
		void draw_ui(const SKC_Config& p_config);
		void draw_ui_level_window(const SKC_Config& p_config);
		void draw_ui_selected_tile_window(const SKC_Config& p_config);
		void draw_tile_picker(const SKC_Config& p_config, std::size_t p_element_types);
		void draw_ui_selected_enemy(const SKC_Config& p_config);
		void draw_ui_selected_mirror(std::size_t p_mirror_no, const SKC_Config& p_config);
		void draw_ui_selected_metadata(const SKC_Config& p_config);

		bool is_valid_constellation(byte p_constellation) const;

		const skc::Level& get_level(void) const;
		skc::Level& get_level(void);

		byte get_tile_selection(void) const;
		void set_tile_selection(byte p_value);

		std::pair<int, int> pixel_to_tile_pos(int p_screen_h, int p_x, int p_y) const;
		void right_click(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config);
		void left_click(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config);
		void shift_click(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config);

		void right_click_md(const std::pair<int, int>& tile_pos, const skc::SKC_Config& p_config);
		void right_click_item(const std::pair<int, int>& tile_pos);
		void right_click_enemy(const std::pair<int, int>& tile_pos);

		bool left_click_item(const std::pair<int, int>& tile_pos);
		bool left_click_enemy(const std::pair<int, int>& tile_pos);
		bool left_click_metadata(const std::pair<int, int>& tile_pos);

		void shift_click_metadata(const std::pair<int, int>& tile_pos, const SKC_Config& p_config);

		void set_meta_tile_position(std::size_t p_index, const position& p_pos);

		// util
		position get_metadata_tile_position(byte p_board_index_no) const;
		void set_metadata_tile_position(byte p_board_index_no, const position& p_pos, const SKC_Config& p_config);

		// file
		std::vector<byte> generate_patch_bytes(const SKC_Config& p_config) const;
		void save_nes_file(const std::string& p_file_path, const SKC_Config& p_config) const;

	public:
		SKC_Main_window(SDL_Renderer* p_rnd, const SKC_Config& p_config);
		void set_application_icon(SDL_Window* p_window) const;
		void move(int p_delta_ms, const klib::User_input& p_input, skc::SKC_Config& p_config, int p_screen_h);
		void draw(SDL_Renderer* p_rnd, const SKC_Config& p_config, int p_w, int p_h);
	};

}

#endif
