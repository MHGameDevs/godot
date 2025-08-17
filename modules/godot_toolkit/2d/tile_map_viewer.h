/**************************************************************************/
/*  tile_map_viewer.h                                                     */
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

#include "../io/map_stream.h"

#include "scene/2d/node_2d.h"
#include "core/math/a_star_grid_2d.h"

class TileMapViewer : public Node2D {
    GDCLASS(TileMapViewer, Node2D);

private:

	struct TileData {
		Ref<Texture2D> texture;
		bool loaded = false;
		Rect2 region_rect;
		PackedInt32Array mask_indices;
	};

	struct MaskFragment {
		uint32_t tile_index;
		bool loaded = false;
	};

	struct MaskData {
		Ref<Texture2D> texture;
		bool loaded = false;
		Rect2 region_rect;
		Node2D *mask_node = nullptr;
		Vector<MaskFragment> fragments;
	};

	static Ref<Shader> s_mask_shader;

	String map_file_path;
	int buffer_tiles = 1;

	Ref<MapStream> map_stream;
	Vector<TileData> tiles;
	Vector<MaskData> masks;
	Ref<AStarGrid2D> astar_grid;

	bool map_dirty = false;

	// Debug properties for exposed bindings
	bool debug_enabled = false;
	float debug_custom_line_width = -1.0;
	Color debug_custom_line_color = Color(1.0, 0.0, 0.0, 1.0);
	Color debug_custom_solid_color = Color(1.0, 0.5, 0.5, 0.4);
#ifdef DEBUG_ENABLED
	// Debug properties internal only
	//RID debug_map_instance;
	Node2D *debug_map_node = nullptr;
#endif // DEBUG_ENABLED

protected:
	static void _bind_methods();

	void _notification(int p_what);
public:

	void load(const String &path);
	String get_map_file() const { return map_file_path; }

	void set_buffer_tiles(int b) { buffer_tiles = b; }
	int get_buffer_tiles() const { return buffer_tiles; }

	void set_debug_enabled(bool p_enabled);
	bool get_debug_enabled() const;

	void set_debug_custom_line_color(Color p_color);
	Color get_debug_custom_line_color() const;

	void set_debug_custom_line_width(float p_line_width);
	float get_debug_custom_line_width() const;

	void set_debug_custom_solid_color(Color p_color);
	Color get_debug_custom_solid_color() const;

	Rect2 get_rect() const;
	Size2 get_cell_size() const;

	Ref<AStarGrid2D> get_astar_grid() const { return astar_grid; }

#ifdef DEBUG_ENABLED
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const override;
	virtual Rect2 _edit_get_rect() const override;
	virtual bool _edit_use_rect() const override;
#endif // DEBUG_ENABLED

private:
	static void _init_static_shader();

	void _reset_map();

	void _init_tiles();
	void _init_mask();
	void _init_astar_grid();

	PackedInt32Array _get_tile_indices_in_rect(const Rect2 &p_rect) const;

	void _update_camera_limits() const;
	Rect2 _get_camera_view_rect() const;

	void _load_visible_tiles();
	void _load_tile(uint32_t x, uint32_t y);
	void _load_masks_for_visible_tiles();

	void _draw_map();

#ifdef TOOLS_ENABLED
	void _load_all_tiles_and_masks_editor();
#endif

#ifdef DEBUG_ENABLED
	//void _draw_debug_map();
#endif // DEBUG_ENABLED
};
