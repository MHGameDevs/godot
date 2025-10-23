/**************************************************************************/
/*  was_palette_transform_set.h                                           */
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

#include "core/io/resource.h"
#include "scene/property_list_helper.h"

class WasPaletteTransformSet : public Resource {
    GDCLASS(WasPaletteTransformSet, Resource);

	struct PaletteTransform {
		uint8_t start = 0;
		uint8_t finish = 0;

		// Red output = r * rr + g * rg + b * rb
		uint16_t rr, rg, rb;
		// Green output = r * gr + g * gg + b * gb
		uint16_t gr, gg, gb;
		// Blue output = r * br + g * bg + b * bb
		uint16_t br, bg, bb;

		PaletteTransform() :
				start(0), finish(0), rr(0), rg(0), rb(0), gr(0), gg(0), gb(0), br(0), bg(0), bb(0) {}
	};

	static inline PropertyListHelper base_property_helper;
	PropertyListHelper property_helper;

    Vector<PaletteTransform> palette_transforms;

protected:
	bool _set(const StringName &p_name, const Variant &p_value) { return property_helper.property_set_value(p_name, p_value); }
	bool _get(const StringName &p_name, Variant &r_ret) const { return property_helper.property_get_value(p_name, r_ret); }
	void _get_property_list(List<PropertyInfo> *p_list) const { property_helper.get_property_list(p_list); }
	bool _property_can_revert(const StringName &p_name) const { return property_helper.property_can_revert(p_name); }
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const { return property_helper.property_get_revert(p_name, r_property); }
    static void _bind_methods();

public:
    void set_palette_transform_start(int p_index, uint8_t p_start);
    uint8_t get_palette_transform_start(int p_index) const;

    void set_palette_transform_finish(int p_index, uint8_t p_finish);
    uint8_t get_palette_transform_finish(int p_index) const;

    // Red output = r * rr + g * rg + b * rb
    void set_palette_transform_rr(int p_index, uint16_t v);
    uint16_t get_palette_transform_rr(int p_index) const;

    void set_palette_transform_rg(int p_index, uint16_t v);
    uint16_t get_palette_transform_rg(int p_index) const;

    void set_palette_transform_rb(int p_index, uint16_t v);
    uint16_t get_palette_transform_rb(int p_index) const;

    // Green output = r * gr + g * gg + b * gb
    void set_palette_transform_gr(int p_index, uint16_t v);
    uint16_t get_palette_transform_gr(int p_index) const;

    void set_palette_transform_gg(int p_index, uint16_t v);
    uint16_t get_palette_transform_gg(int p_index) const;

    void set_palette_transform_gb(int p_index, uint16_t v);
    uint16_t get_palette_transform_gb(int p_index) const;

    // Blue output = r * br + g * bg + b * bb
    void set_palette_transform_br(int p_index, uint16_t v);
    uint16_t get_palette_transform_br(int p_index) const;   

    void set_palette_transform_bg(int p_index, uint16_t v);
    uint16_t get_palette_transform_bg(int p_index) const;

    void set_palette_transform_bb(int p_index, uint16_t v);
    uint16_t get_palette_transform_bb(int p_index) const;
    
	void move_palette_transform(int p_from_idx, int p_to_idx);

    void set_palette_transform_count(int p_count);
    int get_palette_transform_count() const { return palette_transforms.size(); }
    void remove_palette_transform(int p_index);

    WasPaletteTransformSet();
    ~WasPaletteTransformSet();
};
