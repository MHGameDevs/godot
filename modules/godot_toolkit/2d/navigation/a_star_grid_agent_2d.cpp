/**************************************************************************/
/*  a_star_grid_agent_2d.cpp                                              */
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

#include "a_star_grid_agent_2d.h"
#include "a_star_grid_server_2d.h"

#include "core/math/geometry_2d.h"

void AStarGridAgent2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_path_desired_distance", "desired_distance"), &AStarGridAgent2D::set_path_desired_distance);
	ClassDB::bind_method(D_METHOD("get_path_desired_distance"), &AStarGridAgent2D::get_path_desired_distance);

	ClassDB::bind_method(D_METHOD("set_target_desired_distance", "desired_distance"), &AStarGridAgent2D::set_target_desired_distance);
	ClassDB::bind_method(D_METHOD("get_target_desired_distance"), &AStarGridAgent2D::get_target_desired_distance);

	ClassDB::bind_method(D_METHOD("set_path_max_distance", "max_speed"), &AStarGridAgent2D::set_path_max_distance);
	ClassDB::bind_method(D_METHOD("get_path_max_distance"), &AStarGridAgent2D::get_path_max_distance);

	ClassDB::bind_method(D_METHOD("set_allow_partial_path", "allow_partial_path"), &AStarGridAgent2D::set_allow_partial_path);
	ClassDB::bind_method(D_METHOD("get_allow_partial_path"), &AStarGridAgent2D::get_allow_partial_path);

	ClassDB::bind_method(D_METHOD("set_diagonal_mode", "diagonal_mode"), &AStarGridAgent2D::set_diagonal_mode);
	ClassDB::bind_method(D_METHOD("get_diagonal_mode"), &AStarGridAgent2D::get_diagonal_mode);

	ClassDB::bind_method(D_METHOD("set_compute_heuristic", "heuristic"), &AStarGridAgent2D::set_compute_heuristic);
	ClassDB::bind_method(D_METHOD("get_compute_heuristic"), &AStarGridAgent2D::get_compute_heuristic);

	ClassDB::bind_method(D_METHOD("set_estimate_heuristic", "heuristic"), &AStarGridAgent2D::set_estimate_heuristic);
	ClassDB::bind_method(D_METHOD("get_estimate_heuristic"), &AStarGridAgent2D::get_estimate_heuristic);

	ClassDB::bind_method(D_METHOD("set_target_position", "position"), &AStarGridAgent2D::set_target_position);
	ClassDB::bind_method(D_METHOD("get_target_position"), &AStarGridAgent2D::get_target_position);

	ClassDB::bind_method(D_METHOD("get_path_length"), &AStarGridAgent2D::get_path_length);

	ClassDB::bind_method(D_METHOD("get_next_path_position"), &AStarGridAgent2D::get_next_path_position);

	ClassDB::bind_method(D_METHOD("get_current_navigation_path"), &AStarGridAgent2D::get_current_navigation_path);
	ClassDB::bind_method(D_METHOD("get_current_navigation_path_index"), &AStarGridAgent2D::get_current_navigation_path_index);

	ClassDB::bind_method(D_METHOD("distance_to_target"), &AStarGridAgent2D::distance_to_target);
	ClassDB::bind_method(D_METHOD("is_target_reached"), &AStarGridAgent2D::is_target_reached);
	ClassDB::bind_method(D_METHOD("is_target_reachable"), &AStarGridAgent2D::is_target_reachable);
	ClassDB::bind_method(D_METHOD("is_navigation_finished"), &AStarGridAgent2D::is_navigation_finished);
	ClassDB::bind_method(D_METHOD("get_final_position"), &AStarGridAgent2D::get_final_position);

	ADD_GROUP("Pathfinding", "");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "target_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_target_position", "get_target_position");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "path_desired_distance", PROPERTY_HINT_RANGE, "0.1,1000,0.01,or_greater,suffix:px"), "set_path_desired_distance", "get_path_desired_distance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "target_desired_distance", PROPERTY_HINT_RANGE, "0.1,1000,0.01,or_greater,suffix:px"), "set_target_desired_distance", "get_target_desired_distance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "path_max_distance", PROPERTY_HINT_RANGE, "10,1000,1,or_greater,suffix:px"), "set_path_max_distance", "get_path_max_distance");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_partial_path"), "set_allow_partial_path", "get_allow_partial_path");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "diagonal_mode", PROPERTY_HINT_ENUM, "DIAGONAL_MODE_ALWAYS,DIAGONAL_MODE_NEVER,DIAGONAL_MODE_AT_LEAST_ONE_WALKABLE,DIAGONAL_MODE_ONLY_IF_NO_OBSTACLES,DIAGONAL_MODE_MAX"), "set_diagonal_mode", "get_diagonal_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "compute_heuristic", PROPERTY_HINT_ENUM, "HEURISTIC_EUCLIDEAN,HEURISTIC_MANHATTAN,HEURISTIC_OCTILE,HEURISTIC_CHEBYSHEV,HEURISTIC_MAX"), "set_compute_heuristic", "get_compute_heuristic");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "estimate_heuristic", PROPERTY_HINT_ENUM, "HEURISTIC_EUCLIDEAN,HEURISTIC_MANHATTAN,HEURISTIC_OCTILE,HEURISTIC_CHEBYSHEV,HEURISTIC_MAX"), "set_estimate_heuristic", "get_estimate_heuristic");

	ADD_GROUP("Avoidance", "");
	ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"), &AStarGridAgent2D::set_debug_enabled);
	ClassDB::bind_method(D_METHOD("get_debug_enabled"), &AStarGridAgent2D::get_debug_enabled);
	ClassDB::bind_method(D_METHOD("set_debug_path_custom_color", "color"), &AStarGridAgent2D::set_debug_path_custom_color);
	ClassDB::bind_method(D_METHOD("get_debug_path_custom_color"), &AStarGridAgent2D::get_debug_path_custom_color);
	ClassDB::bind_method(D_METHOD("set_debug_path_custom_point_size", "point_size"), &AStarGridAgent2D::set_debug_path_custom_point_size);
	ClassDB::bind_method(D_METHOD("get_debug_path_custom_point_size"), &AStarGridAgent2D::get_debug_path_custom_point_size);
	ClassDB::bind_method(D_METHOD("set_debug_path_custom_line_width", "line_width"), &AStarGridAgent2D::set_debug_path_custom_line_width);
	ClassDB::bind_method(D_METHOD("get_debug_path_custom_line_width"), &AStarGridAgent2D::get_debug_path_custom_line_width);

	ADD_GROUP("Debug", "debug_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_enabled"), "set_debug_enabled", "get_debug_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "debug_path_custom_color"), "set_debug_path_custom_color", "get_debug_path_custom_color");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "debug_path_custom_point_size", PROPERTY_HINT_RANGE, "0,50,0.01,or_greater,suffix:px"), "set_debug_path_custom_point_size", "get_debug_path_custom_point_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "debug_path_custom_line_width", PROPERTY_HINT_RANGE, "-1,50,0.01,or_greater,suffix:px"), "set_debug_path_custom_line_width", "get_debug_path_custom_line_width");

	ADD_SIGNAL(MethodInfo("path_changed"));
	ADD_SIGNAL(MethodInfo("target_reached"));
	ADD_SIGNAL(MethodInfo("waypoint_reached", PropertyInfo(Variant::VECTOR2, "position")));
	ADD_SIGNAL(MethodInfo("navigation_finished"));
}

void AStarGridAgent2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POST_ENTER_TREE: {
			// need to use POST_ENTER_TREE cause with normal ENTER_TREE not all required Nodes are ready.
			// cannot use READY as ready does not get called if Node is re-added to SceneTree
			set_agent_parent(get_parent());
			set_physics_process_internal(true);

#ifdef DEBUG_ENABLED
			debug_path_dirty = true;
#endif // DEBUG_ENABLED
		} break;

		case NOTIFICATION_PARENTED: {
			if (is_inside_tree() && (get_parent() != agent_parent)) {
				// only react to PARENTED notifications when already inside_tree and parent changed, e.g. users switch nodes around
				// PARENTED notification fires also when Node is added in scripts to a parent
				// this would spam transforms fails and world fails while Node is outside SceneTree
				// when node gets reparented when joining the tree POST_ENTER_TREE takes care of this
				set_agent_parent(get_parent());
				set_physics_process_internal(true);
			}
		} break;

		case NOTIFICATION_UNPARENTED: {
			// if agent has no parent no point in processing it until reparented
			set_agent_parent(nullptr);
			set_physics_process_internal(false);
		} break;

		case NOTIFICATION_EXIT_TREE: {
			set_agent_parent(nullptr);
			set_physics_process_internal(false);

#ifdef DEBUG_ENABLED
			if (debug_path_instance.is_valid()) {
				RenderingServer::get_singleton()->canvas_item_set_visible(debug_path_instance, false);
			}
#endif // DEBUG_ENABLED
		} break;

		case NOTIFICATION_UNSUSPENDED: {
			if (get_tree()->is_paused()) {
				break;
			}
			[[fallthrough]];
		}

		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
#ifdef DEBUG_ENABLED
			if (debug_path_dirty) {
				_update_debug_path();
			}
#endif // DEBUG_ENABLED
		} break;
	}
}

AStarGridAgent2D::AStarGridAgent2D() {
	path_query = Ref<AStarGridPathQueryParameters2D>();
	path_query.instantiate();
}

void AStarGridAgent2D::set_agent_parent(Node *p_agent_parent) {
	if (agent_parent == p_agent_parent) {
		return;
	}

	if (Object::cast_to<Node2D>(p_agent_parent) != nullptr) {
		agent_parent = Object::cast_to<Node2D>(p_agent_parent);
	} else {
		agent_parent = nullptr;
	}
}

void AStarGridAgent2D::set_path_desired_distance(real_t p_path_desired_distance) {
	if (Math::is_equal_approx(path_desired_distance, p_path_desired_distance)) {
		return;
	}

	path_desired_distance = p_path_desired_distance;
}

void AStarGridAgent2D::set_target_desired_distance(real_t p_target_desired_distance) {
	if (Math::is_equal_approx(target_desired_distance, p_target_desired_distance)) {
		return;
	}

	target_desired_distance = p_target_desired_distance;
}

void AStarGridAgent2D::set_path_max_distance(real_t p_path_max_distance) {
	if (Math::is_equal_approx(path_max_distance, p_path_max_distance)) {
		return;
	}

	path_max_distance = p_path_max_distance;
}

void AStarGridAgent2D::set_diagonal_mode(AStarGrid2D::DiagonalMode p_diagonal_mode) {
	path_query->set_diagonal_mode(p_diagonal_mode);
}

AStarGrid2D::DiagonalMode AStarGridAgent2D::get_diagonal_mode() const {
	return path_query->get_diagonal_mode();
}

void AStarGridAgent2D::set_compute_heuristic(AStarGrid2D::Heuristic p_heuristic) {
	path_query->set_compute_heuristic(p_heuristic);
}

AStarGrid2D::Heuristic AStarGridAgent2D::get_compute_heuristic() const {
	return path_query->get_compute_heuristic();
}

void AStarGridAgent2D::set_estimate_heuristic(AStarGrid2D::Heuristic p_heuristic) {
	path_query->set_estimate_heuristic(p_heuristic);
}

AStarGrid2D::Heuristic AStarGridAgent2D::get_estimate_heuristic() const {
	return path_query->get_estimate_heuristic();
}

real_t AStarGridAgent2D::get_path_max_distance() const {
	return path_max_distance;
}

void AStarGridAgent2D::set_allow_partial_path(bool p_allow_partial_path) {
	path_query->set_allow_partial_path(p_allow_partial_path);
}

void AStarGridAgent2D::set_target_position(const Vector2 p_position) {
	// Intentionally not checking for equality of the parameter, as we want to update the path even if the target position is the same in case the world changed.

	target_position = p_position;
	target_position_submitted = true;

	_request_repath();
}

Vector2 AStarGridAgent2D::get_target_position() const {
	return target_position;
}

float AStarGridAgent2D::get_path_length() const {
	return navigation_path.size();
}

Vector2 AStarGridAgent2D::get_next_path_position() {
	_update_navigation();

	if (navigation_path.is_empty()) {
		ERR_FAIL_NULL_V_MSG(agent_parent, Vector2(), "The agent has no parent.");
		return agent_parent->get_global_position();
	} else {
		return navigation_path[navigation_path_index];
	}
}

real_t AStarGridAgent2D::distance_to_target() const {
	ERR_FAIL_NULL_V_MSG(agent_parent, 0.0, "The agent has no parent.");
	return agent_parent->get_global_position().distance_to(target_position);
}

bool AStarGridAgent2D::is_target_reached() const {
	return target_reached;
}

bool AStarGridAgent2D::is_target_reachable() {
	_update_navigation();
	return _is_target_reachable();
}

bool AStarGridAgent2D::_is_target_reachable() const {
	return target_desired_distance >= _get_final_position().distance_to(target_position);
}

bool AStarGridAgent2D::is_navigation_finished() {
	_update_navigation();
	return navigation_finished;
}
Vector2 AStarGridAgent2D::get_final_position() {
	_update_navigation();
	return _get_final_position();
}

Vector2 AStarGridAgent2D::_get_final_position() const {
	if (navigation_path.is_empty()) {
		return Vector2();
	}
	return navigation_path[navigation_path.size() - 1];
}

void AStarGridAgent2D::_update_navigation() {
	if (agent_parent == nullptr) {
		return;
	}
	if (!agent_parent->is_inside_tree()) {
		return;
	}
	if (!target_position_submitted) {
		return;
	}

	Vector2 origin = agent_parent->get_global_position();

	bool reload_path = false;

	if (navigation_path.is_empty()) {
		reload_path = true;
	} else {
		// Check if too far from the navigation path
		if (navigation_path_index > 0) {
			const Vector2 segment_a = navigation_path[navigation_path_index - 1];
			const Vector2 segment_b = navigation_path[navigation_path_index];
			Vector2 p = Geometry2D::get_closest_point_to_segment(origin, segment_a, segment_b);
			if (origin.distance_to(p) >= path_max_distance) {
				// To faraway, reload path
				reload_path = true;
			}
		}
	}

	if (reload_path) {
		path_query->set_start_position(origin);
		path_query->set_target_position(target_position);

		navigation_path = AStarGridServer2D::get_singleton()->query_path(path_query);

#ifdef DEBUG_ENABLED
		debug_path_dirty = true;
#endif // DEBUG_ENABLED
		navigation_finished = false;
		last_waypoint_reached = false;
		navigation_path_index = 0;
		emit_signal(SNAME("path_changed"));
	}

	if (navigation_path.is_empty()) {
		return;
	}

	// Check if the navigation has already finished.
	if (navigation_finished) {
		return;
	}

	// Check if we reached the target.
	if (_is_within_target_distance(origin)) {
		// Emit waypoint_reached in case we also moved within distance of a waypoint.
		_advance_waypoints(origin);
		_transition_to_target_reached();
		_transition_to_navigation_finished();
	} else {
		// Advance waypoints if possible.
		_advance_waypoints(origin);
		// Keep navigation running even after reaching the last waypoint if the target is reachable.
		if (last_waypoint_reached && !_is_target_reachable()) {
			_transition_to_navigation_finished();
		}
	}
}

void AStarGridAgent2D::_advance_waypoints(const Vector2 &p_origin) {
	if (last_waypoint_reached) {
		return;
	}

	// Advance to the farthest possible waypoint.
	while (_is_within_waypoint_distance(p_origin)) {
		_trigger_waypoint_reached();

		if (_is_last_waypoint()) {
			last_waypoint_reached = true;
			break;
		}

		_move_to_next_waypoint();
	}
}


void AStarGridAgent2D::_request_repath() {
	navigation_path.clear();
	target_reached = false;
	navigation_finished = false;
	last_waypoint_reached = false;
}

bool AStarGridAgent2D::_is_last_waypoint() const {
	return navigation_path_index == navigation_path.size() - 1;
}

void AStarGridAgent2D::_move_to_next_waypoint() {
	navigation_path_index += 1;
}

bool AStarGridAgent2D::_is_within_waypoint_distance(const Vector2 &p_origin) const {
	return p_origin.distance_to(navigation_path[navigation_path_index]) < path_desired_distance;
}

bool AStarGridAgent2D::_is_within_target_distance(const Vector2 &p_origin) const {
	return p_origin.distance_to(target_position) < target_desired_distance;
}

void AStarGridAgent2D::_trigger_waypoint_reached() {
	const Vector2 waypoint = navigation_path[navigation_path_index];

	// Emit a signal for the waypoint.
	emit_signal(SNAME("waypoint_reached"), waypoint);
}

void AStarGridAgent2D::_transition_to_navigation_finished() {
	navigation_finished = true;
	target_position_submitted = false;

	emit_signal(SNAME("navigation_finished"));
}

void AStarGridAgent2D::_transition_to_target_reached() {
	target_reached = true;
	emit_signal(SNAME("target_reached"));
}

////////DEBUG////////////////////////////////////////////////////////////

void AStarGridAgent2D::set_debug_enabled(bool p_enabled) {
#ifdef DEBUG_ENABLED
	if (debug_enabled == p_enabled) {
		return;
	}

	debug_enabled = p_enabled;
	debug_path_dirty = true;
#endif // DEBUG_ENABLED
}

bool AStarGridAgent2D::get_debug_enabled() const {
	return debug_enabled;
}

void AStarGridAgent2D::set_debug_path_custom_color(Color p_color) {
#ifdef DEBUG_ENABLED
	if (debug_path_custom_color == p_color) {
		return;
	}

	debug_path_custom_color = p_color;
	debug_path_dirty = true;
#endif // DEBUG_ENABLED
}

Color AStarGridAgent2D::get_debug_path_custom_color() const {
	return debug_path_custom_color;
}

void AStarGridAgent2D::set_debug_path_custom_point_size(float p_point_size) {
#ifdef DEBUG_ENABLED
	if (Math::is_equal_approx(debug_path_custom_point_size, p_point_size)) {
		return;
	}

	debug_path_custom_point_size = MAX(0.0, p_point_size);
	debug_path_dirty = true;
#endif // DEBUG_ENABLED
}

float AStarGridAgent2D::get_debug_path_custom_point_size() const {
	return debug_path_custom_point_size;
}

void AStarGridAgent2D::set_debug_path_custom_line_width(float p_line_width) {
#ifdef DEBUG_ENABLED
	if (Math::is_equal_approx(debug_path_custom_line_width, p_line_width)) {
		return;
	}

	debug_path_custom_line_width = p_line_width;
	debug_path_dirty = true;
#endif // DEBUG_ENABLED
}

float AStarGridAgent2D::get_debug_path_custom_line_width() const {
	return debug_path_custom_line_width;
}

#ifdef DEBUG_ENABLED
void AStarGridAgent2D::_navigation_debug_changed() {
	debug_path_dirty = true;
}

void AStarGridAgent2D::_update_debug_path() {
	if (!debug_path_dirty) {
		return;
	}
	debug_path_dirty = false;

	if (!debug_path_instance.is_valid()) {
		debug_path_instance = RenderingServer::get_singleton()->canvas_item_create();
	}

	RenderingServer::get_singleton()->canvas_item_clear(debug_path_instance);

	if (!debug_enabled) {
		return;
	}

	if (!(agent_parent && agent_parent->is_inside_tree())) {
		return;
	}

	RenderingServer::get_singleton()->canvas_item_set_parent(debug_path_instance, agent_parent->get_canvas());
	RenderingServer::get_singleton()->canvas_item_set_z_index(debug_path_instance, RS::CANVAS_ITEM_Z_MAX - 1);
	RenderingServer::get_singleton()->canvas_item_set_visible(debug_path_instance, agent_parent->is_visible_in_tree());

	if (navigation_path.size() <= 1) {
		return;
	}

	Color debug_path_color = debug_path_custom_color;

	Vector<Color> debug_path_colors;
	debug_path_colors.resize(navigation_path.size());
	debug_path_colors.fill(debug_path_color);

	RenderingServer::get_singleton()->canvas_item_add_polyline(debug_path_instance, navigation_path, debug_path_colors, debug_path_custom_line_width, false);

	if (debug_path_custom_point_size <= 0.0) {
		return;
	}

	float point_size = debug_path_custom_point_size;
	float half_point_size = point_size * 0.5;

	for (int i = 0; i < navigation_path.size(); i++) {
		const Vector2 &vert = navigation_path[i];
		Rect2 path_point_rect = Rect2(vert.x - half_point_size, vert.y - half_point_size, point_size, point_size);
		RenderingServer::get_singleton()->canvas_item_add_rect(debug_path_instance, path_point_rect, debug_path_color);
	}
}
#endif // DEBUG_ENABLED
