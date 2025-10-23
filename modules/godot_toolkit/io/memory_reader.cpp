/**************************************************************************/
/*  memory_reader.cpp                                                     */
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

#include "memory_reader.h"

MemoryReader::MemoryReader(const PackedByteArray &buffer) {
	_buffer = buffer;
	base = _buffer.ptr();
	ptr = base;
	end = base + _buffer.size();
}

Ref<MemoryReader> MemoryReader::from_file(const Ref<FileAccess> &p_file) {
	if (p_file.is_null()) {
		return Ref<MemoryReader>();
	}
	uint64_t size = p_file->get_length();
	
	PackedByteArray buffer;
	buffer.resize(size);

	p_file->seek(0);
	p_file->get_buffer(buffer.ptrw(), size);

	Ref<MemoryReader> reader;
	reader.instantiate(buffer);
	return reader;
}

size_t MemoryReader::get_position() const {
	return static_cast<size_t>(ptr - base);
}

size_t MemoryReader::get_length() const {
	return static_cast<size_t>(end - base);
}

void MemoryReader::seek(size_t position) {
	if (position <= get_length()) {
		ptr = base + position;
	}
}

void MemoryReader::skip(size_t bytes) {
	ptr += bytes;
	if (ptr > end) {
		ptr = end;
	}
}

bool MemoryReader::readable() const {
	return ptr < end;
}

uint8_t MemoryReader::get_8() {
	if (ptr + 1 > end) {
		return 0;
	}
	return *ptr++;
}

uint16_t MemoryReader::get_16() {
	if (ptr + 2 > end) {
		return 0;
	}
	uint16_t v;
	memcpy(&v, ptr, sizeof(uint16_t));
	ptr += 2;
	return v;
}

int8_t MemoryReader::get_s8() {
	if (ptr + 1 > end) {
		return 0;
	}
	return static_cast<int8_t>(*ptr++);
}

int16_t MemoryReader::get_s16() {
	if (ptr + 2 > end) {
		return 0;
	}
	int16_t v;
	memcpy(&v, ptr, sizeof(int16_t));
	ptr += 2;
	return v;
}

uint32_t MemoryReader::get_32() {
	if (ptr + 4 > end) {
		return 0;
	}
	uint32_t v;
	memcpy(&v, ptr, sizeof(uint32_t));
	ptr += 4;
	return v;
}

int32_t MemoryReader::get_s32() {
	if (ptr + 4 > end) {
		return 0;
	}
	int32_t v;
	memcpy(&v, ptr, sizeof(int32_t));
	ptr += 4;
	return v;
}

size_t MemoryReader::get_buffer(uint8_t *dst, size_t size) {
	if (ptr + size > end) {
		size = end - ptr;
	}
	memcpy(dst, ptr, size);
	ptr += size;
	return size;
}

PackedByteArray MemoryReader::get_buffer(size_t size) {
	PackedByteArray out;
	if (!ptr || size == 0 || ptr + size > end) {
		return out;
	}
	out.resize(size);
	memcpy(out.ptrw(), ptr, size);
	ptr += size;
	return out;
}
