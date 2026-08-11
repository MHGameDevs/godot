/**************************************************************************/
/*  was_palette_transform_set.cpp                                         */
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

#include "was_palette_transform_set.h"

#include "core/object/class_db.h"

void WasPaletteTransformSet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_palette_transform_start", "index", "start"), &WasPaletteTransformSet::set_palette_transform_start);
	ClassDB::bind_method(D_METHOD("get_palette_transform_start", "index"), &WasPaletteTransformSet::get_palette_transform_start);

    ClassDB::bind_method(D_METHOD("set_palette_transform_finish", "index", "finish"), &WasPaletteTransformSet::set_palette_transform_finish);
	ClassDB::bind_method(D_METHOD("get_palette_transform_finish", "index"), &WasPaletteTransformSet::get_palette_transform_finish);

    ClassDB::bind_method(D_METHOD("set_palette_transform_rr", "index", "rr"), &WasPaletteTransformSet::set_palette_transform_rr);
	ClassDB::bind_method(D_METHOD("get_palette_transform_rr", "index"), &WasPaletteTransformSet::get_palette_transform_rr);
	ClassDB::bind_method(D_METHOD("set_palette_transform_rg", "index", "rg"), &WasPaletteTransformSet::set_palette_transform_rg);
	ClassDB::bind_method(D_METHOD("get_palette_transform_rg", "index"), &WasPaletteTransformSet::get_palette_transform_rg);
	ClassDB::bind_method(D_METHOD("set_palette_transform_rb", "index", "rb"), &WasPaletteTransformSet::set_palette_transform_rb);
	ClassDB::bind_method(D_METHOD("get_palette_transform_rb", "index"), &WasPaletteTransformSet::get_palette_transform_rb);

    ClassDB::bind_method(D_METHOD("set_palette_transform_gr", "index", "gr"), &WasPaletteTransformSet::set_palette_transform_gr);
	ClassDB::bind_method(D_METHOD("get_palette_transform_gr", "index"), &WasPaletteTransformSet::get_palette_transform_gr);
	ClassDB::bind_method(D_METHOD("set_palette_transform_gg", "index", "gg"), &WasPaletteTransformSet::set_palette_transform_gg);
	ClassDB::bind_method(D_METHOD("get_palette_transform_gg", "index"), &WasPaletteTransformSet::get_palette_transform_gg);
	ClassDB::bind_method(D_METHOD("set_palette_transform_gb", "index", "gb"), &WasPaletteTransformSet::set_palette_transform_gb);
	ClassDB::bind_method(D_METHOD("get_palette_transform_gb", "index"), &WasPaletteTransformSet::get_palette_transform_gb);

    ClassDB::bind_method(D_METHOD("set_palette_transform_br", "index", "br"), &WasPaletteTransformSet::set_palette_transform_br);
	ClassDB::bind_method(D_METHOD("get_palette_transform_br", "index"), &WasPaletteTransformSet::get_palette_transform_br);
	ClassDB::bind_method(D_METHOD("set_palette_transform_bg", "index", "bg"), &WasPaletteTransformSet::set_palette_transform_bg);
	ClassDB::bind_method(D_METHOD("get_palette_transform_bg", "index"), &WasPaletteTransformSet::get_palette_transform_bg);
	ClassDB::bind_method(D_METHOD("set_palette_transform_bb", "index", "bb"), &WasPaletteTransformSet::set_palette_transform_bb);
	ClassDB::bind_method(D_METHOD("get_palette_transform_bb", "index"), &WasPaletteTransformSet::get_palette_transform_bb);

	ClassDB::bind_method(D_METHOD("move_palette_transform", "from_idx", "to_idx"), &WasPaletteTransformSet::move_palette_transform);

	ClassDB::bind_method(D_METHOD("set_palette_transform_count", "count"), &WasPaletteTransformSet::set_palette_transform_count);
	ClassDB::bind_method(D_METHOD("get_palette_transform_count"), &WasPaletteTransformSet::get_palette_transform_count);
	ClassDB::bind_method(D_METHOD("remove_palette_transform", "idx"), &WasPaletteTransformSet::remove_palette_transform);

    ADD_ARRAY_COUNT("PaletteTransform", "palette_transform_count", "set_palette_transform_count", "get_palette_transform_count", "palette_transform_");

	PaletteTransform defaults;
	base_property_helper.set_prefix("palette_transform_");
	base_property_helper.set_array_length_getter(&WasPaletteTransformSet::get_palette_transform_count);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "start", PROPERTY_HINT_RANGE, "0,255,1"), defaults.start, &WasPaletteTransformSet::set_palette_transform_start, &WasPaletteTransformSet::get_palette_transform_start);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "finish", PROPERTY_HINT_RANGE, "0,255,1"), defaults.finish, &WasPaletteTransformSet::set_palette_transform_finish, &WasPaletteTransformSet::get_palette_transform_finish);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "rr", PROPERTY_HINT_RANGE, "0,512,1"), defaults.rr, &WasPaletteTransformSet::set_palette_transform_rr, &WasPaletteTransformSet::get_palette_transform_rr);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "rg", PROPERTY_HINT_RANGE, "0,512,1"), defaults.rg, &WasPaletteTransformSet::set_palette_transform_rg, &WasPaletteTransformSet::get_palette_transform_rg);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "rb", PROPERTY_HINT_RANGE, "0,512,1"), defaults.rb, &WasPaletteTransformSet::set_palette_transform_rb, &WasPaletteTransformSet::get_palette_transform_rb);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "gr", PROPERTY_HINT_RANGE, "0,512,1"), defaults.gr, &WasPaletteTransformSet::set_palette_transform_gr, &WasPaletteTransformSet::get_palette_transform_gr);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "gg", PROPERTY_HINT_RANGE, "0,512,1"), defaults.gg, &WasPaletteTransformSet::set_palette_transform_gg, &WasPaletteTransformSet::get_palette_transform_gg);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "gb", PROPERTY_HINT_RANGE, "0,512,1"), defaults.gb, &WasPaletteTransformSet::set_palette_transform_gb, &WasPaletteTransformSet::get_palette_transform_gb);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "br", PROPERTY_HINT_RANGE, "0,512,1"), defaults.br, &WasPaletteTransformSet::set_palette_transform_br, &WasPaletteTransformSet::get_palette_transform_br);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "bg", PROPERTY_HINT_RANGE, "0,512,1"), defaults.bg, &WasPaletteTransformSet::set_palette_transform_bg, &WasPaletteTransformSet::get_palette_transform_bg);
	base_property_helper.register_property(PropertyInfo(Variant::FLOAT, "bb", PROPERTY_HINT_RANGE, "0,512,1"), defaults.bb, &WasPaletteTransformSet::set_palette_transform_bb, &WasPaletteTransformSet::get_palette_transform_bb);
	PropertyListHelper::register_base_helper(get_class_static(), &base_property_helper);
}

void WasPaletteTransformSet::set_palette_transform_start(int p_index, uint8_t p_start) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].start == p_start) {
		return;
	}

	palette_transforms.write[p_index].start = p_start;
	emit_changed();
}

uint8_t WasPaletteTransformSet::get_palette_transform_start(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].start;
}

void WasPaletteTransformSet::set_palette_transform_finish(int p_index, uint8_t p_finish) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].finish == p_finish) {
		return;
	}

	palette_transforms.write[p_index].finish = p_finish;
	emit_changed();
}

uint8_t WasPaletteTransformSet::get_palette_transform_finish(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].finish;
}

void WasPaletteTransformSet::set_palette_transform_rr(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].rr == v) {
		return;
	}

	palette_transforms.write[p_index].rr = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_rr(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].rr;
}

void WasPaletteTransformSet::set_palette_transform_rg(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].rg == v) {
		return;
	}

	palette_transforms.write[p_index].rg = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_rg(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].rg;
}

void WasPaletteTransformSet::set_palette_transform_rb(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].rb == v) {
		return;
	}

	palette_transforms.write[p_index].rb = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_rb(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].rb;
}

void WasPaletteTransformSet::set_palette_transform_gr(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].gr == v) {
		return;
	}

	palette_transforms.write[p_index].gr = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_gr(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].gr;
}

void WasPaletteTransformSet::set_palette_transform_gg(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].gg == v) {
		return;
	}

	palette_transforms.write[p_index].gg = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_gg(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].gg;
}

void WasPaletteTransformSet::set_palette_transform_gb(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].gb == v) {
		return;
	}

	palette_transforms.write[p_index].gb = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_gb(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].gb;
}

void WasPaletteTransformSet::set_palette_transform_br(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].br == v) {
		return;
	}

	palette_transforms.write[p_index].br = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_br(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].br;
}

void WasPaletteTransformSet::set_palette_transform_bg(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].bg == v) {
		return;
	}

	palette_transforms.write[p_index].bg = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_bg(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].bg;
}

void WasPaletteTransformSet::set_palette_transform_bb(int p_index, uint16_t v) {
	if (p_index < 0) {
		p_index += get_palette_transform_count();
	}
	ERR_FAIL_INDEX(p_index, palette_transforms.size());

	if (palette_transforms[p_index].bb == v) {
		return;
	}

	palette_transforms.write[p_index].bb = v;
	emit_changed();
}

uint16_t WasPaletteTransformSet::get_palette_transform_bb(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, palette_transforms.size(), 0);
	return palette_transforms[p_index].bb;
}

void WasPaletteTransformSet::move_palette_transform(int p_from_idx, int p_to_idx) {
    ERR_FAIL_INDEX(p_from_idx, palette_transforms.size());
	ERR_FAIL_INDEX(p_to_idx, palette_transforms.size());

    if (p_from_idx == p_to_idx) {
        return;
    }

	PaletteTransform item = palette_transforms[p_from_idx];
	palette_transforms.remove_at(p_from_idx);
	palette_transforms.insert(p_to_idx, item);
}

void WasPaletteTransformSet::set_palette_transform_count(int p_count) {
    ERR_FAIL_COND(p_count < 0);

	int size = palette_transforms.size();

    if (size == p_count) {
        return;
    }

    palette_transforms.resize(p_count);

	notify_property_list_changed();
	emit_changed();
}

void WasPaletteTransformSet::remove_palette_transform(int p_index) {
    ERR_FAIL_INDEX(p_index, palette_transforms.size());
    palette_transforms.remove_at(p_index);
	emit_changed();
	notify_property_list_changed();
}

WasPaletteTransformSet::WasPaletteTransformSet() {
	property_helper.setup_for_instance(base_property_helper, this);
}

WasPaletteTransformSet::~WasPaletteTransformSet() {
}
