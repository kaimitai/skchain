#include "SKC_Main_window.h"
#include "./../skc_util/Xml_helper.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/klib/IPS_Patch.h"

void skc::SKC_Main_window::save_nes_file(SKC_Config& p_config, bool p_overwrite) const {
	try {
		std::string l_output_path{ p_config.get_nes_output_file_path(p_overwrite) };

		klib::file::write_bytes_to_file(generate_patch_bytes(p_config), l_output_path);
		p_config.add_message("Saved " + l_output_path, c::MSG_CODE_SUCCESS);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what(), c::MSG_CODE_ERROR);
	}
}

void skc::SKC_Main_window::save_xml_files(SKC_Config& p_config) const {
	try {
		skc::xml::save_metadata_xml(p_config.get_xml_path(),
			p_config.get_meta_xml_filename(),
			m_meta_tiles,
			m_drop_schedules,
			m_drop_enemies,
			p_config);
		p_config.add_message("Saved metadata xml " + p_config.get_meta_xml_filename(), c::MSG_CODE_SUCCESS);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what(), c::MSG_CODE_ERROR);
	}

	int l_xml_out{ 0 };
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		try {
			skc::xml::save_level_xml(m_levels.at(i),
				p_config.get_xml_path(),
				p_config.get_level_xml_filename(i));
			++l_xml_out;
		}
		catch (const std::exception&) {}
	}

	p_config.add_message(std::to_string(l_xml_out) + " level xml files saved to " + p_config.get_xml_path(),
		l_xml_out == p_config.get_level_count() ? c::MSG_CODE_SUCCESS : c::MSG_CODE_WARNING);
}

void skc::SKC_Main_window::save_ips_file(SKC_Config& p_config) const {
	try {
		klib::file::write_bytes_to_file(klib::ips::generate_patch(p_config.get_rom_data(), generate_patch_bytes(p_config)),
			p_config.get_ips_output_file_path());
		p_config.add_message("Saved " + p_config.get_ips_output_file_path(), c::MSG_CODE_SUCCESS);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what(), c::MSG_CODE_ERROR);
	}
}

void skc::SKC_Main_window::load_xml_files(SKC_Config& p_config) {
	try {
		auto l_meta_xml = skc::xml::load_metadata_xml(p_config.get_xml_path(),
			p_config.get_meta_xml_filename());
		m_drop_enemies = l_meta_xml.m_drop_enemies;
		m_drop_schedules = l_meta_xml.m_drop_schedules;

		for (const auto& kv : l_meta_xml.m_meta_tiles) {
			if (p_config.get_meta_tile_movable(kv.first)) {

				// find the matching tile instance
				for (auto& mkv : m_meta_tiles)
					for (auto& mkvt : mkv.second)
						if (mkvt.first == kv.first)
							mkvt.second = kv.second;
			}
		}
		p_config.add_message("Metadata xml file loaded", c::MSG_CODE_SUCCESS);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what(), c::MSG_CODE_ERROR);
	}

	int l_import_count{ 0 };
	for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
		try {
			auto l_level = skc::xml::load_level_xml(p_config.get_xml_path(),
				p_config.get_level_xml_filename(i));
			m_levels.at(i) = l_level;
			reset_selections(i);
			++l_import_count;
		}
		catch (const std::exception&) {}
	}
	p_config.add_message(std::to_string(l_import_count) + " level xml files loaded from " + p_config.get_xml_path(),
		l_import_count == p_config.get_level_count() ? c::MSG_CODE_SUCCESS : c::MSG_CODE_WARNING);
}
