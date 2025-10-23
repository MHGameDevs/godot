/**************************************************************************/
/*  was_animated_sprite_2d.cpp                                            */
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

#include "was_animated_sprite_2d.h"

#include "scene/resources/atlas_texture.h"

static const PackedStringArray ANIMATION_NAMES = {
	"down_right", "down_left", "up_left", "up_right",
	"down", "left", "up", "right"
};

void WasAnimatedSprite2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_was_texture", "texture"), &WasAnimatedSprite2D::set_was_texture);
	ClassDB::bind_method(D_METHOD("get_was_texture"), &WasAnimatedSprite2D::get_was_texture);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "was_texture", PROPERTY_HINT_RESOURCE_TYPE, "WasTexture"), "set_was_texture", "get_was_texture");
}

void WasAnimatedSprite2D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "sprite_frames") {
		p_property.usage = PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY;
	}
}

void WasAnimatedSprite2D::set_was_texture(const Ref<WasTexture> &p_was_texture) {
	if (was_texture == p_was_texture) {
		return;
	}

	if (was_texture.is_valid()) {
		was_texture->disconnect_changed(callable_mp(this, &WasAnimatedSprite2D::_was_texture_changed));
	}

	was_texture = p_was_texture;

	if (was_texture.is_valid()) {
		was_texture->connect_changed(callable_mp(this, &WasAnimatedSprite2D::_was_texture_changed));
	}

	_was_texture_changed();
}

void WasAnimatedSprite2D::_was_texture_changed() {
	if (was_texture.is_valid()) {
		Ref<SpriteFrames> sprite_frames;
		sprite_frames.instantiate();

		uint32_t vframes = was_texture->get_vframes();
		uint32_t hframes = was_texture->get_hframes();
		Size2 size = was_texture->get_frame_size();

		for (uint32_t y = 0; y < vframes; y++) {
			String anim_name;

			if (vframes == 1) {
				anim_name = SceneStringName(default_);
			} else {
				if (vframes == 4 || vframes == 8) {
					anim_name = ANIMATION_NAMES[y];
				} else {
					anim_name = vformat("animation_%d", y);
				}
				sprite_frames->add_animation(anim_name);
			}

			sprite_frames->set_animation_speed(anim_name, 10);
			sprite_frames->set_animation_loop(anim_name, true);

			for (uint32_t x = 0; x < hframes; x++) {
				Rect2 frame_region(Vector2(x, y) * size, size);

				Ref<AtlasTexture> atlas;
				atlas.instantiate();
				atlas->set_atlas(was_texture);
				atlas->set_region(frame_region);
				atlas->set_filter_clip(true);

				sprite_frames->add_frame(anim_name, atlas);
			}
		}

		set_sprite_frames(sprite_frames);
		set_centered(false);
		set_offset(was_texture->get_offset());

		if (vframes == 1) {
			play(SceneStringName(default_));
		}
	} else {
		set_sprite_frames(Ref<SpriteFrames>());
		set_centered(true);
		set_offset(Vector2());
	}
}
