/**************************************************************************/
/*  resource_importer_was.cpp                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT TOOLKIT MODULE                       */
/*                        https://github.com/devefx                       */
/**************************************************************************/
/* Copyright (c) 2025 Yoke (yoke.yue@outlook.com)                         */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "resource_importer_was.h"
#include "was_texture.h"

#include "core/io/resource_saver.h"

String ResourceImporterWas::get_importer_name() const {
	return "godot.wastexture";
}

String ResourceImporterWas::get_visible_name() const {
	return "Was Texture";
}

void ResourceImporterWas::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("was");
}

String ResourceImporterWas::get_save_extension() const {
	return "tres";
}

String ResourceImporterWas::get_resource_type() const {
	return "WasTexture";
}

int ResourceImporterWas::get_preset_count() const {
	return 1;
}

String ResourceImporterWas::get_preset_name(int p_idx) const {
	return "Default";
}

void ResourceImporterWas::get_import_options(const String &p_path, List<ImportOption> *r_options, int p_preset) const {
	r_options->push_back(ImportOption(PropertyInfo(Variant::OBJECT, "palette_transform_set", PROPERTY_HINT_RESOURCE_TYPE, "WasPaletteTransformSet"), Variant()));
}

bool ResourceImporterWas::get_option_visibility(const String &p_path, const String &p_option, const HashMap<StringName, Variant> &p_options) const {
	return true;
}

Error ResourceImporterWas::import(ResourceUID::ID p_source_id, const String &p_source_file, const String &p_save_path, const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Ref<WasTexture> tex = WasTexture::load_from_file(p_source_file);
	if (tex.is_null()) {
		return FAILED;
	}

	Ref<WasPaletteTransformSet> pts;
	if (p_options.has("palette_transform_set")) {
		pts = p_options["palette_transform_set"];
		if (!pts.is_null()) {
			tex->set_palette_transform_set(pts);
		}
	}

	String save_path = p_save_path + "." + get_save_extension();
	Error err = ResourceSaver::save(tex, save_path);
	if (err != OK) {
		return err;
	}
	return OK;
}
