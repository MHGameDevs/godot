/**************************************************************************/
/*  map_stream.h                                                          */
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

#include "core/io/image.h"
#include "core/object/ref_counted.h"

#define TILE_WIDTH 320
#define TILE_HEIGHT 240
#define GRID_WIDTH 20
#define GRID_HEIGHT 20

struct TileInfo;
struct MaskInfo;

class MapStream : public RefCounted {
    GDCLASS(MapStream, RefCounted);

    Ref<MemoryReader> reader;
    uint32_t width;
    uint32_t height;
    uint32_t tile_cols;
    uint32_t tile_rows;
    uint32_t mask_count;

    Vector<TileInfo> _tile_info;
    Vector<MaskInfo> _mask_info;
    PackedByteArray _cell_data;

private:
	uint32_t _decompress_mask(uint8_t *src, uint8_t *dst) const;

protected:
    static void _bind_methods();

public:
    static Ref<MapStream> load_from_file(const String &p_path);

    uint32_t get_width() const;
	uint32_t get_height() const;
	uint32_t get_grid_cols() const;
	uint32_t get_grid_rows() const;
	uint32_t get_tile_cols() const;
	uint32_t get_tile_rows() const;
	uint32_t get_mask_count() const;

    PackedByteArray get_cell_data() const;

    Ref<Image> get_tile_image(uint32_t tile_index) const;
	Ref<Image> get_mask_image(uint32_t mask_index) const;
	Rect2 get_mask_rect(uint32_t mask_index) const;
};

struct TileInfo {
	uint32_t pos;
	uint32_t size;
};

struct MaskInfo {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t size;
	uint32_t pos;
};

