#include <SDL.h>
#include "common/klib/NES_Palette.h"
#include "common/klib/NES_Gfx_tile.h"
#include "./common/klib/klib_file.h"
#include "./SKC_Level.h"
#include <iostream>
#include <vector>

using byte = unsigned char;

int main(int argc, char** argv) {
	constexpr unsigned int LEVEL_W{ 16 }, LEVEL_H{ 12 }, LEVEL_COUNT{ 53 };

	constexpr unsigned int OFFSET_ENEMIES{ 0x5d67 };
	constexpr unsigned int OFFSET_WALLS{ 0x603c };
	constexpr unsigned int OFFSET_GFX{ 0x8010 };

	constexpr unsigned int SIZE_LEVEL_WALL_LAYER{ (LEVEL_W * LEVEL_H) / 8 };
	constexpr unsigned int SIZE_LEVEL_WALLS{ 2 * SIZE_LEVEL_WALL_LAYER };
	constexpr unsigned int SIZE_TOTAL_WALLS{ LEVEL_COUNT * SIZE_LEVEL_WALLS };

	auto l_bytes = klib::file::read_file_as_bytes("Solomon's Key (U) [!].nes");

	std::vector<skc::Level> levels;

	for (std::size_t i{ OFFSET_WALLS }; i < OFFSET_WALLS + SIZE_TOTAL_WALLS; i += SIZE_LEVEL_WALLS) {
		std::vector<byte> l_brown_layer{
			std::vector<byte>(begin(l_bytes) + i,
			begin(l_bytes) + i + SIZE_LEVEL_WALL_LAYER) };

		std::vector<byte> l_white_layer{
	std::vector<byte>(begin(l_bytes) + i + SIZE_LEVEL_WALL_LAYER,
	begin(l_bytes) + i + 2 * SIZE_LEVEL_WALL_LAYER) };

		levels.push_back(skc::Level(l_brown_layer, l_white_layer));

	}

	for (std::size_t i{ 0 }; i < levels.size(); ++i)
		std::cout << i << "\n" << levels[i].get_blocks();

	klib::NES_Palette x({ 1, 1, 1 });

	std::vector<byte> img01v = std::vector<byte>(begin(l_bytes) + 0x8010, begin(l_bytes) + 0x8010 + 16);
	klib::NES_Gfx_tile img01(img01v);

	/*
	int lvl_no{ 1 };
	unsigned int l_idx{ OFFSET_ENEMIES };

	while (lvl_no <= 53) {
		std::cout << "Level " << lvl_no << "\n";

		while (true) {
			int a = l_bytes.at(l_idx);
			int b = l_bytes.at(l_idx + 1);
			l_idx += 2;

			if (a == 0) {
				++lvl_no;
				std::cout << "Leftover: " << b << "\n";
				break;
			}
			else {
				std::cout << "Enemy " << a << " @ " << (b % 16) << "," << (b / 16) << "\n";
			}
		}
	}
	*/

	return 0;
}
