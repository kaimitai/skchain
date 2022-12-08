#ifndef SKC_CONSTANTS_XML_H
#define SKC_CONSTANTS_XML_H

namespace skc {
	namespace c {

		constexpr char XML_TAG_META[]{ "skchain" };

		constexpr char XML_TAG_ROM_METADATA[]{ "rom_metadata" };
		constexpr char XML_TAG_GFX_METADATA[]{ "gfx_metadata" };
		constexpr char XML_TAG_PALETTES[]{ "palettes" };
		constexpr char XML_TAG_PALETTE[]{ "palette" };
		constexpr char XML_TAG_MD_DEFINITIONS[]{ "metadata_definitions" };
		constexpr char XML_TAG_ENEMY_DEFINITIONS[]{ "enemy_definitions" };
		constexpr char XML_TAG_ITEM_DEFINITIONS[]{ "item_definitions" };
		constexpr char XML_TAG_ITEM_BITMASKS[]{ "item_bitmasks" };
		constexpr char XML_TAG_ITEM_BITMASK[]{ "item_bitmask" };
		constexpr char XML_TAG_LEVEL_META_ITEMS[]{ "level_meta_items" };
		constexpr char XML_TAG_LEVEL_META_ITEM[]{ "level_meta_item" };
		constexpr char XML_TAG_METADATA[]{ "metadata" };
		constexpr char XML_TAG_ENEMY[]{ "enemy" };
		constexpr char XML_TAG_TILE_PICKERS[]{ "tile_pickers" };
		constexpr char XML_TAG_TILE_PICKER[]{ "tile_picker" };
		constexpr char XML_TAG_SECTION[]{ "section" };
		constexpr char XML_TAG_TILE_DEFINITIONS[]{ "tile_definitions" };
		constexpr char XML_TAG_TILESETS[]{ "tilesets" };
		constexpr char XML_TAG_TILESET[]{ "tileset" };
		constexpr char XML_TAG_TILE[]{ "tile" };

		constexpr char XML_ATTR_APP_VERSION[]{ "app_version" };
		constexpr char XML_TAG_OFFSET_GFX[]{ "offset_gfx" };
		constexpr char XML_ATTR_ITEM_NO[]{ "item_no" };
		constexpr char XML_ATTR_LEVEL_NO[]{ "level_no" };
		constexpr char XML_TAG_OFFSET_BLOCKS[]{ "offset_block_data" };
		constexpr char XML_TAG_OFFSET_ENEMIES[]{ "offset_enemy_table" };
		constexpr char XML_TAG_OFFSET_ITEMS[]{ "offset_item_table" };
		constexpr char XML_TAG_OFFSET_MIRROR_RATE_TABLE[]{ "offset_mirror_rate_table" };
		constexpr char XML_TAG_OFFSET_MIRROR_ENEMY_TABLE[]{ "offset_mirror_enemy_table" };
		constexpr char XML_TAG_ROM_RAM_DIFF[]{ "rom_ram_diff" };
		constexpr char XML_TAG_LEVEL_COUNT[]{ "level_count" };
		constexpr char XML_TAG_MIRROR_RATE_COUNT[]{ "mirror_rate_count" };
		constexpr char XML_TAG_MIRROR_ENEMY_COUNT[]{ "mirror_enemy_count" };
		constexpr char XML_TAG_NES_TILE_COUNT[]{ "nes_tile_count" };
		constexpr char XML_TAG_LEVEL_PALETTES[]{ "level_palettes" };

		constexpr char XML_ATTR_NO[]{ "no" };
		constexpr char XML_ATTR_DESCRIPTION[]{ "description" };
		constexpr char XML_ATTR_ANIMATION[]{ "animation" };
		constexpr char XML_ATTR_W[]{ "w" };
		constexpr char XML_ATTR_TRANSPARENT[]{ "transparent" };
		constexpr char XML_ATTR_OFFSET[]{ "offset" };
		constexpr char XML_ATTR_PALETTE_NO[]{ "palette_no" };
		constexpr char XML_ATTR_NES_TILES[]{ "nes_tiles" };
		constexpr char XML_ATTR_PALETTE_OFFSET[]{ "palette_offset" };
		constexpr char XML_ATTR_TILE_OFFSET[]{ "tile_offset" };

		// level/metadata xml files
		constexpr char XML_META_COMMENTS[]{ " Solomon's Key level metadata file created with Solomon's Keychain (https://github.com/kaimitai/skchain) " };
		constexpr char XML_LEVEL_COMMENTS[]{ " Solomon's Key level file created with Solomon's Keychain (https://github.com/kaimitai/skchain) " };

		constexpr char XML_TAG_LEVEL[]{ "level" };

		constexpr char XML_TAG_BLOCKS[]{ "blocks" };
		constexpr char XML_TAG_BLOCK_ROW[]{ "block_row" };
		constexpr char XML_TAG_ITEMS[]{ "items" };
		constexpr char XML_TAG_ITEM[]{ "item" };
		constexpr char XML_TAG_ENEMIES[]{ "enemies" };

		constexpr char XML_ATTR_SPAWN01_SCHEDULE[]{ "spawn01_schedule" };
		constexpr char XML_ATTR_SPAWN02_SCHEDULE[]{ "spawn02_schedule" };
		constexpr char XML_ATTR_SPAWN01_ENEMIES[]{ "spawn01_enemies" };
		constexpr char XML_ATTR_SPAWN02_ENEMIES[]{ "spawn02_enemies" };
		constexpr char XML_ATTR_KEY_STATUS[]{ "key_status" };
		constexpr char XML_ATTR_KEY_POSITION[]{ "key_position" };
		constexpr char XML_ATTR_DOOR_POSITION[]{ "door_position" };
		constexpr char XML_ATTR_START_POSITION[]{ "start_position" };
		constexpr char XML_ATTR_SPAWN_RATE[]{ "spawn_rate" };
		constexpr char XML_ATTR_SPAWN01[]{ "spawn01" };
		constexpr char XML_ATTR_SPAWN02[]{ "spawn02" };
		constexpr char XML_ATTR_CONSTELLATION_NO[]{ "constellation_no" };
		constexpr char XML_ATTR_CONSTELLATION_POSITION[]{ "constellation_position" };
		constexpr char XML_ATTR_TILESET[]{ "tileset" };

		constexpr char XML_ATTR_ELEMENT_NO[]{ "element_no" };
		constexpr char XML_ATTR_POSITION[]{ "position" };

		constexpr char XML_ATTR_VALUE[]{ "value" };
	}
}

#endif
