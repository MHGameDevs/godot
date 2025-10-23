/**************************************************************************/
/*  a_star_grid_path_query_parameters_2d.h                                */
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
#include "core/math/a_star_grid_2d.h"

class AStarGridPathQueryParameters2D : public RefCounted {
    GDCLASS(AStarGridPathQueryParameters2D, RefCounted);

protected:
    static void _bind_methods();

private:
	Vector2 start_position;
	Vector2 target_position;
    bool allow_partial_path = false;
	AStarGrid2D::DiagonalMode diagonal_mode = AStarGrid2D::DIAGONAL_MODE_ALWAYS;
	AStarGrid2D::Heuristic compute_heuristic = AStarGrid2D::HEURISTIC_EUCLIDEAN;
	AStarGrid2D::Heuristic estimate_heuristic = AStarGrid2D::HEURISTIC_EUCLIDEAN;

public:
    void set_start_position(const Vector2 p_start_position);
	Vector2 get_start_position() const { return start_position; } 

	void set_target_position(const Vector2 p_target_position);
	Vector2 get_target_position() const { return target_position; }

	void set_allow_partial_path(bool p_allow_partial_path);
	bool get_allow_partial_path() const { return allow_partial_path; }

	void set_diagonal_mode(AStarGrid2D::DiagonalMode p_diagonal_mode);
	AStarGrid2D::DiagonalMode get_diagonal_mode() const { return diagonal_mode; }

	void set_compute_heuristic(AStarGrid2D::Heuristic p_heuristic);
	AStarGrid2D::Heuristic get_compute_heuristic() const { return compute_heuristic; }

	void set_estimate_heuristic(AStarGrid2D::Heuristic p_heuristic);
	AStarGrid2D::Heuristic get_estimate_heuristic() const { return estimate_heuristic; }
};
