/**************************************************************************/
/*  was_stream.cpp                                                         */
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

#include "was_stream.h"

#include "core/os/os.h"
#include "core/string/print_string.h"

Ref<WasStream> WasStream::load_from_file(const String &p_path) {

	if (Engine::get_singleton()->is_editor_hint()) {
		print_line(vformat("Load was file: %s", p_path));
	}

    Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);

    if (file.is_null()) {
        ERR_FAIL_V_MSG(Ref<WasStream>(), vformat("Failed to open file: %s", p_path));
    } 
    if (file->get_error() != OK) {
        ERR_FAIL_V_MSG(Ref<WasStream>(), vformat("Error opening file: %s", p_path));
    }

	Ref<MemoryReader> reader = MemoryReader::from_file(file);
	
    uint16_t bf_type = reader->get_16();
    if (bf_type != 0x5053) {
        ERR_FAIL_V_MSG(Ref<WasStream>(), vformat("Invalid WAS file signature in: %s", p_path));
    }

	Ref<WasStream> stream;
	stream.instantiate();
	stream->reader = reader;
	stream->file_path = p_path;
	stream->bl_size = reader->get_16();
	stream->vframes = reader->get_16();
	stream->hframes = reader->get_16();
	stream->width = reader->get_16();
	stream->height = reader->get_16();
	stream->offset_x = reader->get_s16();
	stream->offset_y = reader->get_s16();

    if (stream->bl_size > 12) {
		reader->skip(stream->bl_size - 12);
    }
	reader->get_buffer(stream->palette, 512);

	stream->frame_offsets = reader->raw<uint32_t>();
    return stream;
}

void WasStream::change_palette(const Ref<WasPaletteTransformSet> &p_set) {
	if (p_set.is_null()) {
		return;
	}

	for (size_t idx = 0; idx < p_set->get_palette_transform_count(); idx++) {
		uint8_t start = p_set->get_palette_transform_start(idx);
		uint8_t finish = p_set->get_palette_transform_finish(idx);

		uint16_t rr = p_set->get_palette_transform_rr(idx);
		uint16_t rg = p_set->get_palette_transform_rg(idx);
		uint16_t rb = p_set->get_palette_transform_rb(idx);

		uint16_t gr = p_set->get_palette_transform_gr(idx);
		uint16_t gg = p_set->get_palette_transform_gg(idx);
		uint16_t gb = p_set->get_palette_transform_gb(idx);

		uint16_t br = p_set->get_palette_transform_br(idx);
		uint16_t bg = p_set->get_palette_transform_bg(idx);
		uint16_t bb = p_set->get_palette_transform_bb(idx);

		for (uint8_t i = start; i < finish; i++) {
			uint16_t color = palette[i];
			uint16_t r = (color & 0xF800) >> 11;
			uint16_t g = (color & 0x07E0) >> 5;
			uint16_t b = (color & 0x001F);

			uint16_t r2 = (r * rr) + (g * rg) + (b * rb);
			uint16_t g2 = (r * gr) + (g * gg) + (b * gb);
			uint16_t b2 = (r * br) + (g * bg) + (b * bb);

			r2 = r2 >> 8;
			g2 = g2 >> 8;
			b2 = b2 >> 8;

			if (r2 > 0x1f) {
				r2 = 0x1F;
			}
			if (g2 > 0x3f) {
				g2 = 0x3F;
			}
			if (b2 > 0x1f) {
				b2 = 0x1F;
			}

			palette[i] = (r2 << 11) | (g2 << 5) | b2;
		}
	}
}

void WasStream::reset_palette() {
    reader->seek(16);
    if (bl_size > 12) {
		reader->seek(bl_size - 12);
    }
	reader->get_buffer(palette, 512);
}

struct Frame {
	int32_t offset_x = 0;
	int32_t offset_y = 0;
	int32_t width = 0;
	int32_t height = 0;

	uint32_t position;
	const uint32_t *line_offset;
};

Ref<WasImage> WasStream::get_image() const {
	Vector2 offset;

	int32_t end_x = 0;
	int32_t end_y = 0;

	Vector<Frame> frames;

	for (uint16_t i = 0; i < vframes; i++) {
		for (uint16_t j = 0; j < hframes; j++) {
			Frame frame;

			uint32_t index = i * hframes + j;

			frame.position = frame_offsets[index] + bl_size + 4;

			if (frame.position == bl_size + 4) {
				frame.width = 1;
				frame.height = 1;
				continue;
			}

			reader->seek(frame.position);

			frame.offset_x = reader->get_s32();
			frame.offset_y = reader->get_s32();
			frame.width = reader->get_s32();
			frame.height = reader->get_s32();
			frame.line_offset = reader->raw<uint32_t>();

			frames.append(frame);

			if (index == 0 || offset.x < frame.offset_x) {
				offset.x = frame.offset_x;
			}

			if (index == 0 || offset.y < frame.offset_y) {
				offset.y = frame.offset_y;
			}

			if (index == 0 || end_x < frame.width - frame.offset_x) {
				end_x = frame.width - frame.offset_x;
			}

			if (index == 0 || end_y < frame.height - frame.offset_y) {
				end_y = frame.height - frame.offset_y;
			}
		}
	}

	uint32_t frame_width = end_x + offset.x;
	uint32_t frame_height = end_y + offset.y;

	// image size
	uint32_t image_width = frame_width * hframes;
	uint32_t image_height = frame_height * vframes;

	// tga image
	uint32_t size = image_width * image_height * sizeof(uint32_t) + 18; // 4 bytes per pixel(RGBA) + header
	PackedByteArray tga;
	tga.resize(size);
	tga.fill(0);

	// tga header
	uint8_t header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 40 }; // 32bpp, 8 bits alpha
	memcpy(header + 12, &image_width, 2);
	memcpy(header + 14, &image_height, 2);
	memcpy(tga.ptrw(), header, 18);

	// tga pixels
	uint32_t *pixels = reinterpret_cast<uint32_t *>(tga.ptrw() + 18);

	for (uint16_t i = 0; i < vframes; i++) {
		for (uint16_t j = 0; j < hframes; j++) {

			const Frame &frame = frames[i * hframes + j];

			if (frame.position == bl_size + 4) {
				continue;
			}

			for (int32_t y = 0; y < frame.height; y++) {
				reader->seek(frame.position + frame.line_offset[y]);

				uint32_t image_x = j * frame_width + offset.x - frame.offset_x;
				uint32_t image_y = i * frame_height + offset.y - frame.offset_y + y;
				// Compute the linear index in the image buffer
				uint32_t offset = image_x + image_y * image_width;

				uint32_t x = 0;
				uint32_t skip = 0;
				
				while (x < frame_width && reader->readable()) {
					uint8_t c = reader->get_8();
					uint32_t repeat;
					uint16_t color;

					if (c == 0) {
						skip++;
						if (skip == frame_width) {
							skip--;
							break;
						}
					} else {
						skip = 0;
					}

					switch (c & 0xc0) {
						case 0:
							if ((c & 0x20) > 0) {
								uint8_t alpha = c & 0x1f;
								color = _get_color(reader->get_8() & 0xff);
								pixels[offset + x++] = _convert_rgb565_to_rgb888(_convert_alpha565(color, alpha), alpha * 8);
							} else if (c != 0) {
								repeat = c & 0x1f;
								uint8_t alpha = reader->get_8() & 0x1f;
								color = _get_color(reader->get_8() & 0xff);
								for (uint32_t n = 0; n < repeat; n++) {
									pixels[offset + x++] = _convert_rgb565_to_rgb888(_convert_alpha565(color, alpha), alpha * 8);
								}
							} else {
								if (x > frame_width) {
									continue;
								}
								if (x != 0) {
									x = frame_width;
								}
							}
							break;
						case 64:
							repeat = c & 0x3f;
							for (uint32_t n = 0; n < repeat; n++) {
								color = _get_color(reader->get_8() & 0xff);
								pixels[offset + x++] = _convert_rgb565_to_rgb888(color, 0xff);
							}
							break;
						case 128:
							repeat = c & 0x3f;
							color = _get_color(reader->get_8() & 0xff);
							for (uint32_t n = 0; n < repeat; n++) {
								pixels[offset + x++] = _convert_rgb565_to_rgb888(color, 0xff);
							}
							break;
						case 192:
							x += c & 0x3f;
							break;
					}
				}
				
			}

		}
	}

	// create image
	Ref<Image> image;
	image.instantiate();
	image->load_tga_from_buffer(tga);

	Ref<WasImage> was_image;
	was_image.instantiate();
	was_image->set_size(Size2(frame_width, frame_height));
	was_image->set_offset(-offset);
	was_image->set_image(image);
	return was_image;
}

uint32_t WasStream::get_width() const {
    return width;
}

uint32_t WasStream::get_height() const {
    return height;
}

uint32_t WasStream::get_hframes() const {
    return hframes;
}

uint32_t WasStream::get_vframes() const {
    return vframes;
}

Vector2 WasStream::get_offset() const {
	return Vector2(-offset_x, -offset_y);
}

WasStream::~WasStream() {

}

uint32_t WasStream::_convert_rgb565_to_rgb888(uint16_t color, uint8_t alpha) const {
    uint32_t r = (color >> 11) & 0x1f;
    uint32_t g = (color >> 5) & 0x3f;
    uint32_t b = (color) & 0x1f;

    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);

    return ((alpha & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + ((b & 0xff) << 0);
}

uint16_t WasStream::_convert_alpha565(uint16_t src, uint8_t alpha) const {
    uint32_t r = (src & 0xF800) >> 11;
    uint32_t g = (src & 0x07E0) >> 5;
    uint32_t b = (src & 0x001F) >> 0;

    r = (r * alpha) >> 5;
    g = (g * alpha) >> 5;
    b = (b * alpha) >> 5;

    return r << 11 | g << 5 | b;
}

uint16_t WasStream::_get_color(uint8_t index) const {
    return palette[index];
}

void WasStream::_bind_methods() {
	ClassDB::bind_static_method("WasStream", D_METHOD("load_from_file", "path"), &WasStream::load_from_file);
    ClassDB::bind_method(D_METHOD("change_palette", "palette"), &WasStream::change_palette);
    ClassDB::bind_method(D_METHOD("reset_palette"), &WasStream::reset_palette);
    ClassDB::bind_method(D_METHOD("get_image"), &WasStream::get_image);
    ClassDB::bind_method(D_METHOD("get_width"), &WasStream::get_width);
    ClassDB::bind_method(D_METHOD("get_height"), &WasStream::get_height);
    ClassDB::bind_method(D_METHOD("get_hframes"), &WasStream::get_hframes);
    ClassDB::bind_method(D_METHOD("get_vframes"), &WasStream::get_vframes);
    ClassDB::bind_method(D_METHOD("get_offset"), &WasStream::get_offset);
}

void WasImage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_image"), &WasImage::get_image);
	ClassDB::bind_method(D_METHOD("get_size"), &WasImage::get_size);
	ClassDB::bind_method(D_METHOD("get_offset"), &WasImage::get_offset);
}

void WasImage::set_image(const Ref<Image>& p_image) {
	image = p_image;
}

void WasImage::set_size(const Size2 &p_size) {
	size = p_size;
}

void WasImage::set_offset(const Vector2& p_offset) {
	offset = p_offset;
}

