/**************************************************************************/
/*  was_texture.h                                                         */
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

#pragma once

#include "was_palette_transform_set.h"

#include "core/io/resource_loader.h"
#include "core/io/resource_importer.h"
#include "scene/resources/image_texture.h"
#include "editor/inspector/editor_resource_preview.h"

class BitMap;

class WasTexture : public ImageTexture {
	GDCLASS(WasTexture, ImageTexture);

    String path_to_file;
	
	int vframes = 1;
	int hframes = 1;
	Vector2 offset;
	bool image_stored = false;

	Ref<WasPaletteTransformSet> palette_transform_set;
protected:
	virtual void reload_from_file() override;

	void _get_property_list(List<PropertyInfo> *p_list) const;

    static void _bind_methods();

public:
	static Ref<WasTexture> load_from_file(const String &p_path);

	void load(const String &p_path);
	String get_load_path() const;

	void set_palette_transform_set(const Ref<WasPaletteTransformSet> &p_set);
	Ref<WasPaletteTransformSet> get_palette_transform_set() const { return palette_transform_set; }

	Size2 get_frame_size() const { return Size2(get_width() / hframes, get_height() / vframes); }
	Vector2 get_offset() const { return offset; }
	int get_vframes() const { return vframes; };
    int get_hframes() const { return hframes; }

private:
	void _update_palette_transform_set();

};

class ResourceFormatLoaderWasTexture : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE) override;
	virtual void get_recognized_extensions(List<String> *p_extensions) const override;
	virtual bool handles_type(const String &p_type) const override;
	virtual String get_resource_type(const String &p_path) const override;
};
