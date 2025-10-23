/**************************************************************************/
/*  a_star_grid_server_2d.h                                               */
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

#include "../tile_map_viewer.h"
#include "a_star_grid_path_query_parameters_2d.h"

#include "core/math/a_star_grid_2d.h"

class AStarGridServer2D : public Object {
	GDCLASS(AStarGridServer2D, Object);

	static AStarGridServer2D *singleton;

	LocalVector<TileMapViewer *> active_maps;

protected:
	static void _bind_methods();

public:
	static AStarGridServer2D *get_singleton();

	void map_set_active(TileMapViewer *p_tilemap, bool p_active);
	bool map_is_active(TileMapViewer *p_tilemap) const;

	void register_map(TileMapViewer* p_tilemap);
	void unregister_map(TileMapViewer *p_tilemap);

	Vector<Vector2> query_path(const Ref<AStarGridPathQueryParameters2D> &p_query_parameters);

	AStarGridServer2D();
	~AStarGridServer2D();
};
