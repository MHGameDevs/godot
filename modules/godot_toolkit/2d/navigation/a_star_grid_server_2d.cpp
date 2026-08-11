/**************************************************************************/
/*  a_star_grid_server_2d.cpp                                             */
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

#include "a_star_grid_server_2d.h"

AStarGridServer2D *AStarGridServer2D::singleton = nullptr;

void AStarGridServer2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("map_set_active", "map", "active"), &AStarGridServer2D::map_set_active);
	ClassDB::bind_method(D_METHOD("map_is_active", "map"), &AStarGridServer2D::map_is_active);
	ClassDB::bind_method(D_METHOD("query_path", "parameters"), &AStarGridServer2D::query_path);
}

AStarGridServer2D::AStarGridServer2D() {
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;
}

AStarGridServer2D::~AStarGridServer2D() {
	singleton = nullptr;
}

AStarGridServer2D *AStarGridServer2D::get_singleton() {
	if (singleton == nullptr) {
		singleton = new AStarGridServer2D();
	}
	return singleton;
}

void AStarGridServer2D::map_set_active(TileMapViewer *p_tilemap, bool p_active) {
	ERR_FAIL_NULL(p_tilemap);

	int map_index = active_maps.find(p_tilemap);
	ERR_FAIL_COND_MSG(map_index < 0, "TileMapViewer not registered.");

	if (p_active) {
		if (map_index != active_maps.size() - 1) {
			active_maps.remove_at(map_index);
			active_maps.push_back(p_tilemap);
		}
	} else {
		if (map_index != 0) {
			active_maps.remove_at(map_index);
			active_maps.insert(0, p_tilemap);
		}
	}
}

bool AStarGridServer2D::map_is_active(TileMapViewer *p_tilemap) const {
	ERR_FAIL_NULL_V(p_tilemap, false);

	if (active_maps.is_empty()) {
		return false;
	}
	return active_maps[active_maps.size() - 1] == p_tilemap;
}

void AStarGridServer2D::register_map(TileMapViewer *p_tilemap) {
	ERR_FAIL_NULL(p_tilemap);

	int map_index = active_maps.find(p_tilemap);
	ERR_FAIL_COND_MSG(map_index != -1, "TileMapViewer already registered.");

	active_maps.push_back(p_tilemap);
}

void AStarGridServer2D::unregister_map(TileMapViewer *p_tilemap) {
	ERR_FAIL_COND(p_tilemap == nullptr);

	int map_index = active_maps.find(p_tilemap);
	ERR_FAIL_COND_MSG(map_index < 0, "TileMapViewer not registered.");

	active_maps.remove_at(map_index);
}

Vector<Vector2> AStarGridServer2D::query_path(const Ref<AStarGridPathQueryParameters2D> &p_query_parameters) {
	Vector<Vector2> result;

	if (active_maps.is_empty()) {
		ERR_FAIL_V_MSG(result, "No active TileMapViewer in stack.");
	}

	TileMapViewer *active = active_maps[active_maps.size() - 1];
	ERR_FAIL_COND_V(active == nullptr, result);

	Ref<AStarGrid2D> astar = active->get_astar_grid();
	ERR_FAIL_COND_V(astar == nullptr, result);

	Size2 cell_size = astar->get_cell_size();

	astar->set_diagonal_mode(p_query_parameters->get_diagonal_mode());
	astar->set_default_compute_heuristic(p_query_parameters->get_compute_heuristic());
	astar->set_default_estimate_heuristic(p_query_parameters->get_estimate_heuristic());

	return astar->get_point_path(p_query_parameters->get_start_position() / cell_size, p_query_parameters->get_target_position() / cell_size, p_query_parameters->get_allow_partial_path());
}
