/**************************************************************************/
/*  a_star_grid_agent_2d.h                                                */
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

#include "scene/2d/node_2d.h"

class AStarGridAgent2D : public Node2D {
    GDCLASS(AStarGridAgent2D, Node2D);

	Node2D *agent_parent = nullptr;
	TileMapViewer *agent_tilemap = nullptr;

	real_t path_desired_distance = 20.0;
	real_t target_desired_distance = 10.0;
	real_t path_max_distance = 100.0;
	bool allow_partial_path = false;
	AStarGrid2D::DiagonalMode diagonal_mode = AStarGrid2D::DIAGONAL_MODE_ALWAYS;
	AStarGrid2D::Heuristic default_compute_heuristic = AStarGrid2D::HEURISTIC_EUCLIDEAN;
	AStarGrid2D::Heuristic default_estimate_heuristic = AStarGrid2D::HEURISTIC_EUCLIDEAN;

	Vector2 target_position;

	bool target_position_submitted = false;

	bool target_reached = false;
	bool navigation_finished = true;
	bool last_waypoint_reached = false;

	PackedVector2Array navigation_path;
    int navigation_path_index = 0;

	// Debug properties for exposed bindings
	bool debug_enabled = false;
	float debug_path_custom_point_size = 4.0;
	float debug_path_custom_line_width = -1.0;
	Color debug_path_custom_color = Color(1.0, 0.0, 0.0, 1.0);

#ifdef DEBUG_ENABLED
	// Debug properties internal only
	bool debug_path_dirty = true;
	RID debug_path_instance;
#endif // DEBUG_ENABLED

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_agent_parent(Node *p_agent_parent);

	void set_path_desired_distance(real_t p_dd);
	real_t get_path_desired_distance() const { return path_desired_distance; }

	void set_target_desired_distance(real_t p_dd);
	real_t get_target_desired_distance() const { return target_desired_distance; }

	void set_path_max_distance(real_t p_pmd);
	real_t get_path_max_distance() const;

	void set_allow_partial_path(bool p_allow_partial_path);
	bool get_allow_partial_path() const { return allow_partial_path; };

	void set_diagonal_mode(AStarGrid2D::DiagonalMode p_diagonal_mode);
	AStarGrid2D::DiagonalMode get_diagonal_mode() const;

	void set_default_compute_heuristic(AStarGrid2D::Heuristic p_heuristic);
	AStarGrid2D::Heuristic get_default_compute_heuristic() const;

	void set_default_estimate_heuristic(AStarGrid2D::Heuristic p_heuristic);
	AStarGrid2D::Heuristic get_default_estimate_heuristic() const;

	void set_target_position(Vector2 p_position);
	Vector2 get_target_position() const;

    float get_path_length() const;

	Vector2 get_next_path_position();

    const PackedVector2Array &get_current_navigation_path() const { return navigation_path; }
    int get_current_navigation_path_index() const { return navigation_path_index; }
    
	real_t distance_to_target() const;
	bool is_target_reached() const;
	bool is_target_reachable();
	bool is_navigation_finished();
	Vector2 get_final_position();

    void set_debug_enabled(bool p_enabled);
	bool get_debug_enabled() const;

    void set_debug_path_custom_color(Color p_color);
	Color get_debug_path_custom_color() const;

    void set_debug_path_custom_point_size(float p_point_size);
	float get_debug_path_custom_point_size() const;

	void set_debug_path_custom_line_width(float p_line_width);
	float get_debug_path_custom_line_width() const;

private:
	bool _is_target_reachable() const;
	Vector2 _get_final_position() const;

	void _update_agent_tilemap();

	void _update_navigation();
	void _advance_waypoints(const Vector2 &p_origin);
	void _request_repath();

	bool _is_last_waypoint() const;
	void _move_to_next_waypoint();
	bool _is_within_waypoint_distance(const Vector2 &p_origin) const;
	bool _is_within_target_distance(const Vector2 &p_origin) const;

	void _trigger_waypoint_reached();
	void _transition_to_navigation_finished();
	void _transition_to_target_reached();

#ifdef DEBUG_ENABLED
	void _navigation_debug_changed();
	void _update_debug_path();
#endif // DEBUG_ENABLED
};
