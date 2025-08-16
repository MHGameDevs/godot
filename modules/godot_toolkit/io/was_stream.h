/**************************************************************************/
/*  was_stream.h                                                          */
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

#include "memory_reader.h"

#include "core/object/ref_counted.h"
#include "core/io/image.h"

class PaletteTransform;

class WasStream : public RefCounted {
    GDCLASS(WasStream, RefCounted);

    Ref<MemoryReader> reader;
    uint16_t bl_size;
    uint16_t vframes;
	uint16_t hframes;
	uint16_t width;
	uint16_t height;
	uint16_t offset_x;
	uint16_t offset_y;
    uint16_t palette[256];
	const uint32_t *frame_offsets;
protected:
    static void _bind_methods();

public:

    static Ref<WasStream> load_from_file(const String &p_path);

    void change_palette(Ref<PaletteTransform> p_palette_transform);
    void reset_palette();
    Ref<Image> get_image() const;

    uint32_t get_width() const;
	uint32_t get_height() const;
	uint32_t get_hframes() const;
	uint32_t get_vframes() const;
    Vector2 get_offset() const;

	WasStream() {}
	~WasStream();

private:
    uint32_t _convert_rgb565_to_rgb888(uint16_t color, uint8_t alpha) const;
	uint16_t _convert_alpha565(uint16_t src, uint8_t alpha) const;
    uint16_t _get_color(uint8_t index) const;
};

class PaletteTransform : public RefCounted {
    GDCLASS(PaletteTransform, RefCounted);

private:
	uint32_t start;
	uint32_t finish;

    // Red output = r * rr + g * rg + b * rb
	uint32_t rr, rg, rb;
    // Green output = r * gr + g * gg + b * gb
	uint32_t gr, gg, gb;
    // Blue output = r * br + g * bg + b * bb
	uint32_t br, bg, bb;
    
protected:
    static void _bind_methods();

public:
    // Getters / Setters
	void set_start(uint32_t p_start) { start = p_start; }
	uint32_t get_start() const { return start; }

    void set_finish(uint32_t p_finish) { finish = p_finish; }
	uint32_t get_finish() const { return finish; }

    void set_rr(uint32_t v) { rr = v; }
	uint32_t get_rr() const { return rr; }

    void set_rg(uint32_t v) { rg = v; }
	uint32_t get_rg() const { return rg; }

    void set_rb(uint32_t v) { rb = v; }
	uint32_t get_rb() const { return rb; }

    void set_gr(uint32_t v) { gr = v; }
	uint32_t get_gr() const { return gr; }

    void set_gg(uint32_t v) { gg = v; }
	uint32_t get_gg() const { return gg; }

    void set_gb(uint32_t v) { gb = v; }
	uint32_t get_gb() const { return gb; }

    void set_br(uint32_t v) { br = v; }
	uint32_t get_br() const { return br; }

    void set_bg(uint32_t v) { bg = v; }
	uint32_t get_bg() const { return bg; }

    void set_bb(uint32_t v) { bb = v; }
	uint32_t get_bb() const { return bb; }

    String to_string() const;
};
