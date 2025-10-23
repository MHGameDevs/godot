/**************************************************************************/
/*  was_texture.cpp                                                       */
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

#include "was_texture.h"

#include "../io/was_stream.h"
#include "scene/resources/bit_map.h"

void WasTexture::reload_from_file() {
	String path = ResourceLoader::path_remap(get_path());
	if (!path.is_resource_file()) {
		return;
	}

	path = ResourceLoader::path_remap(path); //remap for translation
	path = ResourceLoader::import_remap(path); //remap for import
	if (!path.is_resource_file()) {
		return;
	}

	Ref<WasTexture> tex = ResourceLoader::load(path, "", ResourceFormatImporter::CACHE_MODE_IGNORE);
	if (tex.is_null()) {
		return;
	}

	path_to_file = tex->path_to_file;
	hframes = tex->hframes;
	vframes = tex->vframes;
	offset = tex->offset;
	palette_transform_set = tex->palette_transform_set;
	set_image(tex->get_image());
	image_stored = true;
}

void WasTexture::_bind_methods() {
	ClassDB::bind_static_method("WasTexture", D_METHOD("load_from_file", "path"), &WasTexture::load_from_file);

	ClassDB::bind_method(D_METHOD("load", "path"), &WasTexture::load);
	ClassDB::bind_method(D_METHOD("get_load_path"), &WasTexture::get_load_path);

	ClassDB::bind_method(D_METHOD("set_palette_transform_set", "palette_transform_set"), &WasTexture::set_palette_transform_set);
	ClassDB::bind_method(D_METHOD("get_palette_transform_set"), &WasTexture::get_palette_transform_set);

	ClassDB::bind_method(D_METHOD("get_frame_size"), &WasTexture::get_frame_size);
	ClassDB::bind_method(D_METHOD("get_offset"), &WasTexture::get_offset);
	ClassDB::bind_method(D_METHOD("get_hframes"), &WasTexture::get_hframes);
	ClassDB::bind_method(D_METHOD("get_vframes"), &WasTexture::get_vframes);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "load_path", PROPERTY_HINT_FILE, "*.was"), "load", "get_load_path");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "frame_size", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_frame_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_offset");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hframes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_hframes");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vframes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_vframes");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "palette_transform_set", PROPERTY_HINT_RESOURCE_TYPE, "WasPaletteTransformSet"), "set_palette_transform_set", "get_palette_transform_set");
}

void WasTexture::_get_property_list(List<PropertyInfo> *p_list) const {
	for (List<PropertyInfo>::Element *E = p_list->front(); E;) {
		List<PropertyInfo>::Element *next = E->next();
		if (E->get().name == "image") {
			p_list->erase(E);
		}
		E = next;
	}
}

Ref<WasTexture> WasTexture::load_from_file(const String &p_path) {
	Ref<WasTexture> tex;
	tex.instantiate();
	tex->load(p_path);
	return tex;
}

void WasTexture::load(const String &p_path) {
	if (path_to_file == p_path) {
		return;
	}

	Ref<WasStream> stream = WasStream::load_from_file(p_path);
	ERR_FAIL_COND_MSG(stream.is_null(), vformat("Unable to open file: %s.", p_path));

	if (palette_transform_set.is_valid()) {
		stream->change_palette(palette_transform_set);
	}

	Ref<WasImage> image = stream->get_image();

	set_image(image->get_image());

	path_to_file = p_path;
	hframes = stream->get_hframes();
	vframes = stream->get_vframes();
	offset = image->get_offset();

	image_stored = true;
}

String WasTexture::get_load_path() const {
	return path_to_file;
}

void WasTexture::set_palette_transform_set(const Ref<WasPaletteTransformSet> &p_set) {
	if (palette_transform_set == p_set) {
		return;
	}

	if (palette_transform_set.is_valid()) {
		palette_transform_set->disconnect_changed(callable_mp(this, &WasTexture::_update_palette_transform_set));
	}

	palette_transform_set = p_set;

	if (palette_transform_set.is_valid()) {
		palette_transform_set->connect_changed(callable_mp(this, &WasTexture::_update_palette_transform_set));
	}

	_update_palette_transform_set();
}

void WasTexture::_update_palette_transform_set() {
	if (image_stored) {
		Ref<WasStream> stream = WasStream::load_from_file(path_to_file);
		ERR_FAIL_COND_MSG(stream.is_null(), vformat("Unable to open file: %s.", path_to_file));

		if (palette_transform_set.is_valid()) {
			stream->change_palette(palette_transform_set);
		}

		Ref<WasImage> image = stream->get_image();
		update(image->get_image());
	} else {
		notify_property_list_changed();
		emit_changed();
	}
}

Ref<Resource> ResourceFormatLoaderWasTexture::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<WasTexture> tex = WasTexture::load_from_file(p_path);
	if (tex.is_valid()) {
		return tex;
	}
	return Ref<Resource>();
}

void ResourceFormatLoaderWasTexture::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("was");
}

bool ResourceFormatLoaderWasTexture::handles_type(const String &p_type) const {
	return p_type == "WasTexture";
}

String ResourceFormatLoaderWasTexture::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "was") {
		return "WasTexture";
	}
	return "";
}
