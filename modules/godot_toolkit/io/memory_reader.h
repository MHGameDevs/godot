/**************************************************************************/
/*  memory_reader.h                                                       */
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

#include "core/object/ref_counted.h"
#include "core/io/file_access.h"

class MemoryReader : public RefCounted {
	GDCLASS(MemoryReader, RefCounted);

private:
	const uint8_t *base;
	const uint8_t *ptr;
	const uint8_t *end;
	PackedByteArray _buffer;

public:
	MemoryReader(const PackedByteArray &buffer);

	static Ref<MemoryReader> from_file(const Ref<FileAccess> &p_file);

	size_t get_position() const;

	size_t get_length() const;

	void seek(size_t position);

	void skip(size_t bytes);

	bool readable() const;

	uint8_t get_8();

	uint16_t get_16();

	uint32_t get_32();

	int8_t get_s8();

	int16_t get_s16();

	int32_t get_s32();

	size_t get_buffer(uint8_t *dst, size_t size);

	PackedByteArray get_buffer(size_t size);

	template <typename T>
	size_t get_buffer(T *dst, size_t size) {
		return get_buffer(reinterpret_cast<uint8_t *>(dst), size);
	}

	template <typename T>
	const T *raw() const {
		return reinterpret_cast<const T *>(ptr);
	}
};
