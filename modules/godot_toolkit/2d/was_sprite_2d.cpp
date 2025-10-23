/**************************************************************************/
/*  was_sprite_2d.cpp                                                     */
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

#include "was_sprite_2d.h"

void WasSprite2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_was_texture", "texture"), &WasSprite2D::set_was_texture);
	ClassDB::bind_method(D_METHOD("get_was_texture"), &WasSprite2D::get_was_texture);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "was_texture", PROPERTY_HINT_RESOURCE_TYPE, "WasTexture"), "set_was_texture", "get_was_texture");
}

void WasSprite2D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "texture") {
		p_property.usage = PROPERTY_USAGE_NONE;
	}
}

void WasSprite2D::set_was_texture(const Ref<WasTexture> &p_was_texture) {
	if (was_texture == p_was_texture) {
		return;
	}

	if (was_texture.is_valid()) {
		was_texture->disconnect_changed(callable_mp(this, &WasSprite2D::_was_texture_changed));
	}

	was_texture = p_was_texture;

	if (was_texture.is_valid()) {
		was_texture->connect_changed(callable_mp(this, &WasSprite2D::_was_texture_changed));
	}

	_was_texture_changed();
}

void WasSprite2D::_was_texture_changed() {
	if (was_texture.is_valid()) {
		set_texture(was_texture);
		set_centered(false);
		set_offset(was_texture->get_offset());
		set_hframes(was_texture->get_hframes());
		set_vframes(was_texture->get_vframes());
	} else {
		set_texture(Ref<Texture2D>());
		set_centered(true);
		set_offset(Vector2());
		set_hframes(1);
		set_vframes(1);
	}
}
