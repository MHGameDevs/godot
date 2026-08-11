/**************************************************************************/
/*  map_stream.cpp                                                        */
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

#include "map_stream.h"

#define VER_MAPX	0x4D415058	// 大话2旧地图
#define VER_MAP1	0x4D312E30	// 大话2新地图 梦幻地图
#define VER_MAP2_5	0x4D322E35	// 大话3内测版
#define VER_MAP3	0x4D332E30	// 大话3正式版
#define VER_ROL0	0x524F4C30	// 大话3地图背景文件

#define FLAG_GAMI	0x494D4147
#define FLAG_2KSM	0x4D534B32
#define FLAG_KSAM	0x4D41534B
#define FLAG_GEPJ	0x4A504547
#define FLAG_KOLB	0x424C4F4B
#define FLAG_LLEC	0x43454C4C
#define FLAG_GIRB	0x42524947
#define FLAG_TGIL	0x4C494754
#define FLAG_END	0x454E4420

#define TILE_GRID_COLS 16
#define TILE_GRID_ROWS 12

Ref<MapStream> MapStream::load_from_file(const String& p_path) {
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);

	if (file.is_null()) {
		ERR_FAIL_V_MSG(Ref<MapStream>(), vformat("Failed to open file: %s", p_path));
	}
	if (file->get_error() != OK) {
		ERR_FAIL_V_MSG(Ref<MapStream>(), vformat("Error opening file: %s", p_path));
	}

	Ref<MemoryReader> reader = MemoryReader::from_file(file);

	uint32_t bf_type = reader->get_32();

	switch (bf_type) {
		case VER_MAPX:
		case VER_MAP1:
		case VER_MAP2_5:
		case VER_MAP3:
		case VER_ROL0:
			break;
		default:
			ERR_FAIL_V_MSG(Ref<MapStream>(), vformat("Unsupported map version: 0x%08X in file: %s", bf_type, p_path));
	}

	Ref<MapStream> stream;
	stream.instantiate();
	stream->reader = reader;

	// read map size
	stream->width = reader->get_32();
	stream->height = reader->get_32();
	stream->tile_cols = ceil(stream->width / float(TILE_WIDTH));
	stream->tile_rows = ceil(stream->height / float(TILE_HEIGHT));

	// read tile offset
	size_t tile_offsets_size = stream->tile_cols * stream->tile_rows * sizeof(uint32_t);
	const uint32_t *tile_offsets = reader->raw<uint32_t>();
	reader->skip(tile_offsets_size);

	// head length & mask number
	uint32_t head_len = reader->get_32();
	stream->mask_count = reader->get_32();

	// read mask offset
	size_t mask_offsets_size = stream->mask_count * sizeof(uint32_t);
	const uint32_t *mask_offsets = reader->raw<uint32_t>();

	// read mask block info
	stream->_mask_info.resize(stream->mask_count);
	for (uint32_t i = 0; i < stream->mask_count; i++) {
		MaskInfo info;
		info.pos = mask_offsets[i];
		reader->seek(info.pos);
		reader->get_buffer(&info, 20);
		stream->_mask_info.set(i, info);
	}

	// read map block info
	stream->_tile_info.resize(stream->tile_cols * stream->tile_rows);
	uint32_t horizontal_size = stream->tile_cols * TILE_GRID_COLS;
	uint32_t vertical_size = stream->tile_rows * TILE_GRID_ROWS;

	PackedByteArray cell_data;
	cell_data.resize(horizontal_size * vertical_size);
	cell_data.fill(0);

	// read unit data
	for (uint32_t y = 0; y < stream->tile_rows; y++) {
		for (uint32_t x = 0; x < stream->tile_cols; x++) {

			uint32_t i = y * stream->tile_cols + x;
			reader->seek(tile_offsets[i]);

			uint32_t unknown = reader->get_32();
			if (unknown > 0) {
				reader->seek(reader->get_position() + unknown * sizeof(uint32_t));
			}

			bool loop = true;
			while (loop) {
				uint32_t flag = reader->get_32();
				uint32_t size = reader->get_32();

				switch (flag) {
					case FLAG_GAMI:
					case FLAG_2KSM:
					case FLAG_KSAM:
					case FLAG_KOLB:
					case FLAG_GIRB:
					case FLAG_TGIL:
						reader->seek(reader->get_position() + size);
						break;
					case FLAG_GEPJ:
						TileInfo info;
						info.pos = reader->get_position();
						info.size = size;
						stream->_tile_info.set(i, info);
						reader->seek(reader->get_position() + size);
						break;
					case FLAG_LLEC:
						for (uint32_t h = 0; h < TILE_GRID_ROWS; h++) {
							uint32_t offset = x * TILE_GRID_COLS + (y * TILE_GRID_ROWS + h) * horizontal_size;
							reader->get_buffer(cell_data.ptrw() + offset, TILE_GRID_COLS);
						}
						break;
					case FLAG_END:
						reader->seek(reader->get_position() + size);
						loop = false;
						break;
					default:
						loop = false;
						break;
				}
			}
		}
	}

	// align cell_data
	uint32_t grid_cols = stream->get_grid_cols();
	uint32_t grid_rows = stream->get_grid_rows();

	if (grid_cols != horizontal_size || grid_rows != vertical_size) {
		stream->_cell_data.resize(grid_cols * grid_rows);
		stream->_cell_data.fill(0);
		for (uint32_t col = 0; col < grid_cols; col++) {
			for (uint32_t row = 0; row < grid_rows; row++) {
				uint32_t index = col + row * horizontal_size;
				uint32_t actually_index = col + row * grid_cols;
				if (cell_data[index] != 0) {
					stream->_cell_data.set(actually_index, 1);
				}
			}
		}
	} else {
		stream->_cell_data = cell_data;
	}

	return stream;
}

uint32_t MapStream::get_width() const {
	return width;
}

uint32_t MapStream::get_height() const {
	return height;
}

uint32_t MapStream::get_grid_cols() const {
	return width / GRID_WIDTH;
}

uint32_t MapStream::get_grid_rows() const {
	return height / GRID_HEIGHT;
}

uint32_t MapStream::get_tile_cols() const {
	return tile_cols;
}

uint32_t MapStream::get_tile_rows() const {
	return tile_rows;
}

uint32_t MapStream::get_mask_count() const {
	return mask_count;
}

PackedByteArray MapStream::get_cell_data() const {
	return _cell_data;
}

Ref<Image> MapStream::get_tile_image(uint32_t tile_index) const {
	uint32_t max_index = MAX(tile_cols * tile_rows, _tile_info.size()) - 1;

	if (tile_index < 0 || tile_index > max_index) {
		ERR_FAIL_V_MSG(Ref<Image>(), vformat("Tile index %d is out of range. Valid range: 0 to %d.", max_index));
	}

	// get tile info
	TileInfo info = _tile_info.get(tile_index);

	// read tile data
	reader->seek(info.pos);
	PackedByteArray raw_tile_data = reader->get_buffer(info.size);

	// dynamic memory
	PackedByteArray buffer;
	buffer.append_array(raw_tile_data.slice(0, 2));

	bool is_filled = false;
	uint32_t start = 4;

	for (uint32_t i = 4; i < info.size - 2; i++) {

		if (!is_filled && raw_tile_data[i] == 0xFF && raw_tile_data[++i] == 0xDA) {
			is_filled = true;

			// update JPEG fill
			raw_tile_data.set(i + 2, (uint8_t)12);
			buffer.append_array(raw_tile_data.slice(start, i + 10));
			buffer.push_back((uint8_t)0);
			buffer.push_back((uint8_t)63);
			buffer.push_back((uint8_t)0);

			start = i + 10;
			i += 9;
		}

		if (is_filled && raw_tile_data[i] == 0xFF) {
			buffer.append_array(raw_tile_data.slice(start, i + 1));
			buffer.push_back((uint8_t)0);
			start = i + 1;
		}
	}

	buffer.append_array(raw_tile_data.slice(start, info.size));

	// create image
	Ref<Image> image;
	image.instantiate();
	image->load_jpg_from_buffer(buffer);
	return image;
}

Ref<Image> MapStream::get_mask_image(uint32_t mask_index) const {
	if (mask_index < 0 || mask_index >= mask_count) {
		ERR_FAIL_V_MSG(Ref<Image>(), vformat("Mask index %d is out of range. Valid range: 0 to %d.", mask_count - 1));
	}

	// get mask info
	MaskInfo info = _mask_info[mask_index];

	// read mask data
	Vector<uint8_t> mask_data_src;
	mask_data_src.resize(info.size);
	reader->seek(info.pos + 20);
	reader->get_buffer(mask_data_src.ptrw(), info.size);

	// 4k align
	uint32_t align_width = (info.width / 4 + (info.width % 4 != 0)) * 4;

	// decompress
	Vector<uint8_t> mask_data_dec;
	mask_data_dec.resize(align_width * info.height / 4);
	_decompress_mask(mask_data_src.ptrw(), mask_data_dec.ptrw());

	// tga image
	uint32_t size = info.width * info.height * sizeof(uint32_t) + 18;
	PackedByteArray tga;
	tga.resize(size);
	tga.fill(0);

	// tga header
	uint8_t header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 40 };
	memcpy(header + 12, &info.width, 2);
	memcpy(header + 14, &info.height, 2);
	memcpy(tga.ptrw(), header, 18);

	// tga pixels
	uint32_t *pixels = reinterpret_cast<uint32_t *>(tga.ptrw() + 18);

	for (uint32_t h = 0; h < info.height; h++) {
		for (uint32_t w = 0; w < info.width; w++) {
			uint32_t index = (h * align_width + w) * 2;
			uint8_t value = mask_data_dec[index / 8];
			value = value >> (index % 8);

			if ((value & 3) == 3) {
				pixels[w + h * info.width] = 0xFFFFFFFF;
			}
		}
	}

	// create
	Ref<Image> image;
	image.instantiate();
	image->load_tga_from_buffer(tga);
	return image;
}

Rect2 MapStream::get_mask_rect(uint32_t mask_index) const {
	if (mask_index < 0 || mask_index >= mask_count) {
		ERR_FAIL_V_MSG(Rect2(), vformat("Mask index %d is out of range. Valid range: 0 to %d.", mask_count - 1));
	}
	MaskInfo info = _mask_info[mask_index];
	return Rect2(info.x, info.y, info.width, info.height);
}

void MapStream::_bind_methods() {
    ClassDB::bind_static_method("MapStream", D_METHOD("load_from_file", "path"), &MapStream::load_from_file);
    ClassDB::bind_method(D_METHOD("get_width"), &MapStream::get_width);
    ClassDB::bind_method(D_METHOD("get_height"), &MapStream::get_height);
    ClassDB::bind_method(D_METHOD("get_grid_cols"), &MapStream::get_grid_cols);
    ClassDB::bind_method(D_METHOD("get_grid_rows"), &MapStream::get_grid_rows);
	ClassDB::bind_method(D_METHOD("get_tile_cols"), &MapStream::get_tile_cols);
	ClassDB::bind_method(D_METHOD("get_tile_rows"), &MapStream::get_tile_rows);
	ClassDB::bind_method(D_METHOD("get_mask_count"), &MapStream::get_mask_count);
	ClassDB::bind_method(D_METHOD("get_cell_data"), &MapStream::get_cell_data);
	ClassDB::bind_method(D_METHOD("get_tile_image", "tile_index"), &MapStream::get_tile_image);
	ClassDB::bind_method(D_METHOD("get_mask_image", "mask_index"), &MapStream::get_mask_image);
	ClassDB::bind_method(D_METHOD("get_mask_rect", "mask_index"), &MapStream::get_mask_rect);

	ClassDB::bind_integer_constant(get_class_static(), StringName("MapStream"), "TILE_WIDTH", TILE_WIDTH);
	ClassDB::bind_integer_constant(get_class_static(), StringName("MapStream"), "TILE_HEIGHT", TILE_HEIGHT);
	ClassDB::bind_integer_constant(get_class_static(), StringName("MapStream"), "GRID_WIDTH", GRID_WIDTH);
	ClassDB::bind_integer_constant(get_class_static(), StringName("MapStream"), "GRID_HEIGHT", GRID_HEIGHT);
}

uint32_t MapStream::_decompress_mask(uint8_t *src, uint8_t *dst) const {
	uint8_t *op;
	uint8_t *ip;
	unsigned t;
	uint8_t *m_pos;

	op = (uint8_t *)dst;
	ip = (uint8_t *)src;

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4) {
			goto match_next;
		}
		do {
			*op++ = *ip++;
		} while (--t > 0);
		goto first_literal_run;
	}

	while (1) {
		t = *ip++;
		if (t >= 16) {
			goto match;
		}
		if (t == 0) {
			while (*ip == 0) {
				t += 255;
				ip++;
			}
			t += 15 + *ip++;
		}

		*(unsigned *)op = *(unsigned *)ip;
		op += 4;
		ip += 4;
		if (--t > 0) {
			if (t >= 4) {
				do {
					*(unsigned *)op = *(unsigned *)ip;
					op += 4;
					ip += 4;
					t -= 4;
				} while (t >= 4);
				if (t > 0) {
					do {
						*op++ = *ip++;
					} while (--t > 0);
				}
			} else {
				do {
					*op++ = *ip++;
				} while (--t > 0);
			}
		}

	first_literal_run:

		t = *ip++;
		if (t >= 16) {
			goto match;
		}

		m_pos = op - 0x0801;
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;

		*op++ = *m_pos++;
		*op++ = *m_pos++;
		*op++ = *m_pos;

		goto match_done;

		while (1) {
		match:
			if (t >= 64) {
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;

				goto copy_match;

			} else if (t >= 32) {
				t &= 31;
				if (t == 0) {
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 31 + *ip++;
				}

				m_pos = op - 1;
				m_pos -= (*(unsigned short *)ip) >> 2;
				ip += 2;
			} else if (t >= 16) {
				m_pos = op;
				m_pos -= (t & 8) << 11;
				t &= 7;
				if (t == 0) {
					while (*ip == 0) {
						t += 255;
						ip++;
					}
					t += 7 + *ip++;
				}
				m_pos -= (*(unsigned short *)ip) >> 2;
				ip += 2;
				if (m_pos == op) {
					goto eof_found;
				}
				m_pos -= 0x4000;
			} else {
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				*op++ = *m_pos++;
				*op++ = *m_pos;
				goto match_done;
			}
			if (t >= 6 && (op - m_pos) >= 4) {
				*(unsigned *)op = *(unsigned *)m_pos;
				op += 4;
				m_pos += 4;
				t -= 2;
				do {
					*(unsigned *)op = *(unsigned *)m_pos;
					op += 4;
					m_pos += 4;
					t -= 4;
				} while (t >= 4);
				if (t > 0) {
					do {
						*op++ = *m_pos++;
					} while (--t > 0);
				}
			} else {
			copy_match:
				*op++ = *m_pos++;
				*op++ = *m_pos++;
				do {
					*op++ = *m_pos++;
				} while (--t > 0);
			}
		match_done:
			t = ip[-2] & 3;
			if (t == 0) {
				break;
			}
		match_next:
			do {
				*op++ = *ip++;
			} while (--t > 0);
			t = *ip++;
		}
	}
eof_found:
	return (op - (uint8_t *)dst);
}
