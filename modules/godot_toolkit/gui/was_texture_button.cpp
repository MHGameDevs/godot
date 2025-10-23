/**************************************************************************/
/*  was_texture_button.cpp                                                */
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

#include "was_texture_button.h"

#include "scene/resources/atlas_texture.h"

void WasTextureButton::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_was_texture", "texture"), &WasTextureButton::set_was_texture);
	ClassDB::bind_method(D_METHOD("get_was_texture"), &WasTextureButton::get_was_texture);
	ClassDB::bind_method(D_METHOD("set_use_click_mask", "texture"), &WasTextureButton::set_use_click_mask);
	ClassDB::bind_method(D_METHOD("get_use_click_mask"), &WasTextureButton::get_use_click_mask);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "was_texture", PROPERTY_HINT_RESOURCE_TYPE, "WasTexture"), "set_was_texture", "get_was_texture");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_click_mask"), "set_use_click_mask", "get_use_click_mask");
}

void WasTextureButton::_validate_property(PropertyInfo &p_property) const {
    if (p_property.name == "texture_normal" || p_property.name == "texture_pressed" ||
        p_property.name == "texture_hover" || p_property.name == "texture_disabled" ||
		p_property.name == "texture_focused") {
        p_property.usage = PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY;
    }
}

void WasTextureButton::set_was_texture(const Ref<WasTexture> &p_was_texture) {
	if (was_texture == p_was_texture) {
		return;
	}

	if (was_texture.is_valid()) {
		was_texture->disconnect_changed(callable_mp(this, &WasTextureButton::_was_texture_changed));
	}

	was_texture = p_was_texture;

	if (was_texture.is_valid()) {
		was_texture->connect_changed(callable_mp(this, &WasTextureButton::_was_texture_changed));
	}

	_was_texture_changed();
}

void WasTextureButton::set_use_click_mask(const bool p_use_click_mask) {
	if (use_click_mask == p_use_click_mask) {
		return;
	}

	use_click_mask = p_use_click_mask;

	_was_texture_changed();
}

void WasTextureButton::_was_texture_changed() {
	if (was_texture.is_valid()) {

		uint32_t hframes = was_texture->get_hframes();
		Size2 size = was_texture->get_frame_size();

		for (uint32_t i = 0; i < hframes; i++) {
			Ref<AtlasTexture> atlas_tex;
			atlas_tex.instantiate();
			atlas_tex->set_atlas(was_texture);
			atlas_tex->set_region(Rect2(Vector2(i, 0) * size, size));

			if (i == 0) {
				set_texture_normal(atlas_tex);
				if (use_click_mask) {
					if (atlas_tex->get_image().is_null()) {
						auto path = get_path();
						print_line(path);
					}

					Ref<BitMap> bit_map = Ref<BitMap>();
					bit_map.instantiate();
					bit_map->create_from_image_alpha(atlas_tex->get_image());
					set_click_mask(bit_map);
				} else {
					set_click_mask(Ref<BitMap>());
				}
			} else if (i == 1) {
				set_texture_pressed(atlas_tex);
			} else if (i == 2) {
				set_texture_hover(atlas_tex);
			} else if (i == 3) {
				set_texture_disabled(atlas_tex);
			} else if (i == 4) {
				set_texture_focused(atlas_tex);
			}
		}
	} else {
		set_texture_normal(Ref<Texture2D>());
		set_texture_pressed(Ref<Texture2D>());
		set_texture_hover(Ref<Texture2D>());
		set_texture_disabled(Ref<Texture2D>());
		set_texture_focused(Ref<Texture2D>());
		set_click_mask(Ref<BitMap>());
	}
}
