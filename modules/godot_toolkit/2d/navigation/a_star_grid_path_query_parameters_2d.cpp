/**************************************************************************/
/*  a_star_grid_path_query_parameters_2d.cpp                              */
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

#include "a_star_grid_path_query_parameters_2d.h"

#include "core/object/class_db.h"

void AStarGridPathQueryParameters2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_start_position", "start_position"), &AStarGridPathQueryParameters2D::set_start_position);
    ClassDB::bind_method(D_METHOD("get_start_position"), &AStarGridPathQueryParameters2D::get_start_position);

    ClassDB::bind_method(D_METHOD("set_target_position", "target_position"), &AStarGridPathQueryParameters2D::set_target_position);
    ClassDB::bind_method(D_METHOD("get_target_position"), &AStarGridPathQueryParameters2D::get_target_position);

    ClassDB::bind_method(D_METHOD("set_allow_partial_path", "allow_partial_path"), &AStarGridPathQueryParameters2D::set_allow_partial_path);
    ClassDB::bind_method(D_METHOD("get_allow_partial_path"), &AStarGridPathQueryParameters2D::get_allow_partial_path);

    ClassDB::bind_method(D_METHOD("set_diagonal_mode", "diagonal_mode"), &AStarGridPathQueryParameters2D::set_diagonal_mode);
    ClassDB::bind_method(D_METHOD("get_diagonal_mode"), &AStarGridPathQueryParameters2D::get_diagonal_mode);

    ClassDB::bind_method(D_METHOD("set_compute_heuristic", "compute_heuristic"), &AStarGridPathQueryParameters2D::set_compute_heuristic);
    ClassDB::bind_method(D_METHOD("get_compute_heuristic"), &AStarGridPathQueryParameters2D::get_compute_heuristic);

    ClassDB::bind_method(D_METHOD("set_estimate_heuristic", "estimate_heuristic"), &AStarGridPathQueryParameters2D::set_estimate_heuristic);
    ClassDB::bind_method(D_METHOD("get_estimate_heuristic"), &AStarGridPathQueryParameters2D::get_estimate_heuristic);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "start_position"), "set_start_position", "get_start_position");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "target_position"), "set_target_position", "get_target_position");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_partial_path"), "set_allow_partial_path", "get_allow_partial_path");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "diagonal_mode", PROPERTY_HINT_ENUM, "DIAGONAL_MODE_ALWAYS,DIAGONAL_MODE_NEVER,DIAGONAL_MODE_AT_LEAST_ONE_WALKABLE,DIAGONAL_MODE_ONLY_IF_NO_OBSTACLES,DIAGONAL_MODE_MAX"), "set_diagonal_mode", "get_diagonal_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "compute_heuristic", PROPERTY_HINT_ENUM, "HEURISTIC_EUCLIDEAN,HEURISTIC_MANHATTAN,HEURISTIC_OCTILE,HEURISTIC_CHEBYSHEV,HEURISTIC_MAX"), "set_compute_heuristic", "get_compute_heuristic");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "estimate_heuristic", PROPERTY_HINT_ENUM, "HEURISTIC_EUCLIDEAN,HEURISTIC_MANHATTAN,HEURISTIC_OCTILE,HEURISTIC_CHEBYSHEV,HEURISTIC_MAX"), "set_estimate_heuristic", "get_estimate_heuristic");
}

void AStarGridPathQueryParameters2D::set_start_position(const Vector2 p_start_position) {
    start_position = p_start_position;
}

void AStarGridPathQueryParameters2D::set_target_position(const Vector2 p_target_position) {
    target_position = p_target_position;
}

void AStarGridPathQueryParameters2D::set_allow_partial_path(bool p_allow_partial_path) {
    allow_partial_path = p_allow_partial_path;
}

void AStarGridPathQueryParameters2D::set_diagonal_mode(AStarGrid2D::DiagonalMode p_diagonal_mode) {
	ERR_FAIL_INDEX((int)p_diagonal_mode, (int)AStarGrid2D::DIAGONAL_MODE_MAX);
    diagonal_mode = p_diagonal_mode;
}

void AStarGridPathQueryParameters2D::set_compute_heuristic(AStarGrid2D::Heuristic p_heuristic) {
	ERR_FAIL_INDEX((int)p_heuristic, (int)AStarGrid2D::HEURISTIC_MAX);
    compute_heuristic = p_heuristic;
}

void AStarGridPathQueryParameters2D::set_estimate_heuristic(AStarGrid2D::Heuristic p_heuristic) {
	ERR_FAIL_INDEX((int)p_heuristic, (int)AStarGrid2D::HEURISTIC_MAX);
    estimate_heuristic = p_heuristic;
}
