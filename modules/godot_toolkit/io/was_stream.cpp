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
	stream->bl_size = reader->get_16();
	stream->vframes = reader->get_16();
	stream->hframes = reader->get_16();
	stream->width = reader->get_16();
	stream->height = reader->get_16();
	stream->offset_x = reader->get_16();
	stream->offset_y = reader->get_16();

    if (stream->bl_size > 12) {
		reader->seek(stream->bl_size - 12);
    }
	reader->get_buffer(stream->palette, 512);

	stream->frame_offsets = reader->raw<uint32_t>();
    return stream;
}

void WasStream::change_palette(Ref<PaletteTransform> p) {
	for (uint32_t i = p->get_start(); i < p->get_finish(); i++) {
        uint16_t color = palette[i];
        uint16_t r = (color & 0xF800) >> 11;
        uint16_t g = (color & 0x07E0) >> 5;
        uint16_t b = (color & 0x001F);
        
        uint16_t r2 = (r * p->get_rr()) + (g * p->get_rg()) + (b * p->get_rb());
		uint16_t g2 = (r * p->get_gr()) + (g * p->get_gg()) + (b * p->get_gb());
		uint16_t b2 = (r * p->get_br()) + (g * p->get_bg()) + (b * p->get_bb());

        r2 = r2 >> 8;
        g2 = g2 >> 8;
        b2 = b2 >> 8;

        if(r2 > 0x1f) r2 = 0x1F;
        if(g2 > 0x3f) g2 = 0x3F;
        if(b2 > 0x1f) b2 = 0x1F;

        palette[i] = (r2 << 11) | (g2 << 5) | b2;
    }
}

void WasStream::reset_palette() {
    reader->seek(16);
    if (bl_size > 12) {
		reader->seek(bl_size - 12);
    }
	reader->get_buffer(palette, 512);
}

Ref<Image> WasStream::get_image() const {
    // image size
	uint32_t image_width = width * hframes;
	uint32_t image_height = height * vframes;

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

	// parse
	for (uint16_t i = 0; i < vframes; i++) {
		for (uint16_t j = 0; j < hframes; j++) {
			
			uint32_t position = frame_offsets[i * hframes + j] + bl_size + 4;

			uint32_t frame_offset_x;
			uint32_t frame_offset_y;
			uint32_t frame_width;
			uint32_t frame_height;

			if (position == bl_size + 4) {
				frame_width = 1;
				frame_height = 1;
				continue;
			}

			reader->seek(position);
			frame_offset_x = reader->get_32();
			frame_offset_y = reader->get_32();
			frame_width = reader->get_32();
			frame_height = reader->get_32();

			// line offsets
			size_t line_offset_size = frame_height * sizeof(uint32_t);
			const uint32_t *line_offset = reader->raw<uint32_t>();
			reader->skip(line_offset_size);
			
			for (uint32_t y = 0; y < frame_height; y++) {
				reader->seek(position + line_offset[y]);

				uint32_t frame_offset = (offset_x - frame_offset_x) + (offset_y - frame_offset_y) * image_width;
				uint32_t offset = width * j + image_width * (height * i + y) + frame_offset;

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
	return image;
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

String PaletteTransform::to_string() const {
    return vformat("PaletteTransform(start=%d, finish=%d, rr=%d, rg=%d, rb=%d, gr=%d, gg=%d, gb=%d, br=%d, bg=%d, bb=%d)",
                   start, finish, rr, rg, rb, gr, gg, gb, br, bg, bb);
}

void PaletteTransform::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_start", "start"), &PaletteTransform::set_start);
    ClassDB::bind_method(D_METHOD("get_start"), &PaletteTransform::get_start);
    ClassDB::bind_method(D_METHOD("set_finish", "finish"), &PaletteTransform::set_finish);
    ClassDB::bind_method(D_METHOD("get_finish"), &PaletteTransform::get_finish);
    ClassDB::bind_method(D_METHOD("set_rr", "rr"), &PaletteTransform::set_rr);
    ClassDB::bind_method(D_METHOD("get_rr"), &PaletteTransform::get_rr);
    ClassDB::bind_method(D_METHOD("set_rg", "rg"), &PaletteTransform::set_rg);
    ClassDB::bind_method(D_METHOD("get_rg"), &PaletteTransform::get_rg);
    ClassDB::bind_method(D_METHOD("set_rb", "rb"), &PaletteTransform::set_rb);
    ClassDB::bind_method(D_METHOD("get_rb"), &PaletteTransform::get_rb);
    ClassDB::bind_method(D_METHOD("set_gr", "gr"), &PaletteTransform::set_gr);
    ClassDB::bind_method(D_METHOD("get_gr"), &PaletteTransform::get_gr);
    ClassDB::bind_method(D_METHOD("set_gg", "gg"), &PaletteTransform::set_gg);
    ClassDB::bind_method(D_METHOD("get_gg"), &PaletteTransform::get_gg);
    ClassDB::bind_method(D_METHOD("set_br", "br"), &PaletteTransform::set_br);
    ClassDB::bind_method(D_METHOD("get_br"), &PaletteTransform::get_br);
    ClassDB::bind_method(D_METHOD("set_bg", "bg"), &PaletteTransform::set_bg);
    ClassDB::bind_method(D_METHOD("get_bg"), &PaletteTransform::get_bg);
}
