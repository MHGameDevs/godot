/**************************************************************************/
/*  tile_map_viewer.cpp                                                   */
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

#include "tile_map_viewer.h"

#include "scene/2d/sprite_2d.h"
#include "scene/2d/camera_2d.h"
#include "scene/main/viewport.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/atlas_texture.h"

Ref<Shader> TileMapViewer::s_mask_shader = nullptr;

void TileMapViewer::load(const String &path) {
	// If the requested path is the same as the currently loaded map, do nothing
	if (map_file_path == path) {
		return;
	}

	// Remove and free all mask nodes
	for (MaskData mask : masks) {
		if (mask.mask_node) {
			mask.mask_node->get_parent()->remove_child(mask.mask_node);
			mask.mask_node->queue_free();
		}
	}

	// Clear tile and mask data arrays
	tiles.clear();
	masks.clear();

	// Release the AStarGrid2D reference so the grid data can be freed
	astar_grid.unref();

	// If the path is empty, clear all map data and nodes
	if (path.is_empty()) {
		// Release the map stream
		map_stream.unref();
		map_file_path = "";
		// Request a redraw to update the empty map
		queue_redraw();
		return;
	}

	// Load the new map from file
	map_stream = MapStream::load_from_file(path);
	map_file_path = path;

	// Initialize tile data (positions and sizes)
	_init_tiles();

	// Initialize mask data and mask fragments
	_init_mask();

	// Initialize the AStarGrid2D for pathfinding based on the map's grid data.
	_init_astar_grid();

#ifdef TOOLS_ENABLED
	// If running in the editor, load all tiles immediately for preview
	if (Engine::get_singleton()->is_editor_hint()) {
		_load_all_tiles_and_masks_editor();
	}
#endif

	// If any tiles were loaded, request a redraw
	if (needs_redraw) {
		queue_redraw();
	}
}


void TileMapViewer::_init_static_shader() {
	if (s_mask_shader.is_valid()) {
		return;
	}

	s_mask_shader.instantiate();
	s_mask_shader->set_code(R"(
		shader_type canvas_item;

		uniform vec2 tex_position;
		uniform vec2 tex_size;
		uniform sampler2D mask_tex: filter_nearest;
		uniform vec2 mask_offset;
		uniform vec2 mask_tex_size;

		void fragment() {
			vec2 real_tex_size = vec2(1.0) / TEXTURE_PIXEL_SIZE;
			vec2 pos = UV * real_tex_size;
			if (pos.x < tex_position.x || pos.x > (tex_position.x + tex_size.x) ||
				pos.y < tex_position.y || pos.y > (tex_position.y + tex_size.y)) {
				discard;
			} else {
				vec2 mask_uv = (pos + mask_offset) / mask_tex_size;
				float mask_val = texture(mask_tex, mask_uv).a;
    			vec4 base_color = texture(TEXTURE, UV);
				COLOR = vec4(base_color.rgb, base_color.a * mask_val * 0.5);
			}
		}
	)");
}

void TileMapViewer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path"), &TileMapViewer::load);
	ClassDB::bind_method(D_METHOD("get_map_file"), &TileMapViewer::get_map_file);

	ClassDB::bind_method(D_METHOD("set_buffer_tiles", "buffer"), &TileMapViewer::set_buffer_tiles);
	ClassDB::bind_method(D_METHOD("get_buffer_tiles"), &TileMapViewer::get_buffer_tiles);

	ClassDB::bind_method(D_METHOD("get_rect"), &TileMapViewer::get_rect);

	ClassDB::bind_method(D_METHOD("get_astar_grid"), &TileMapViewer::get_astar_grid);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "map_file", PROPERTY_HINT_FILE, "*.map"), "load", "get_map_file");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "buffer_tiles"), "set_buffer_tiles", "get_buffer_tiles");
}

void TileMapViewer::_init_tiles() {
	// Get the number of tiles horizontally and vertically
	uint32_t tile_cols = map_stream->get_tile_cols();
	uint32_t tile_rows = map_stream->get_tile_rows();

	// Map total size and individual tile size
	Vector2 map_size(map_stream->get_width(), map_stream->get_height());
	Vector2 tile_size(TILE_WIDTH, TILE_HEIGHT);

	// Resize the tile array to fit all tiles
	tiles.resize(tile_cols * tile_rows);

	// Initialize each tile's region rectangle
	for (uint32_t y = 0; y < tile_rows; y++) {
		for (uint32_t x = 0; x < tile_cols; x++) {
			uint32_t tile_index = x + y * tile_cols;

			// Compute tile position and default region size
			Vector2 tile_pos = Vector2(x, y) * tile_size;
			Rect2 region_rect(tile_pos, tile_size);

			// Adjust region size for tiles that exceed map boundaries
			Vector2 end = region_rect.get_end();
			if (end.x > map_size.x || end.y > map_size.y) {
				region_rect = Rect2(tile_pos, map_size - region_rect.position);
			}

			// Create TileData and store in the array
			TileData tile;
			tile.region_rect = region_rect;
			tiles.set(tile_index, tile);
		}
	}
}

void TileMapViewer::_init_mask() {
	// Get the total number of masks
	uint32_t mask_count = map_stream->get_mask_count();
	masks.resize(mask_count);

	 // Initialize each mask
	for (uint32_t mask_index = 0; mask_index < mask_count; mask_index++) {
		// Get the mask rectangle
		Rect2 mask_rect = map_stream->get_mask_rect(mask_index);

		MaskData mask;
		mask.region_rect = mask_rect;

		// Get all tiles that intersect with this mask
		PackedInt32Array tile_indices = _get_tile_indices_in_rect(mask_rect);
		for (uint32_t tile_index : tile_indices) {
			// Add this mask index to the tile's mask list
			TileData &tile = tiles.write[tile_index];
			tile.mask_indices.append(mask_index);

			// Create a MaskFragment for this tile
			MaskFragment fragment;
			fragment.tile_index = tile_index;
			mask.fragments.append(fragment);
		}

		// Store the mask
		masks.set(mask_index, mask);
	}
}

void TileMapViewer::_init_astar_grid() {
	// Create a new instance of AStarGrid2D
	astar_grid.instantiate();

	// Get the number of columns and rows for the grid from the map stream
	uint32_t grid_cols = map_stream->get_grid_cols();
	uint32_t grid_rows = map_stream->get_grid_rows();

	// Define the grid region based on the number of columns and rows from the map stream
	// Rect2i(origin_x, origin_y, width_in_cells, height_in_cells)
	astar_grid->set_region(Rect2i(0, 0, grid_cols, grid_rows));

	// Set the pixel size of each grid cell (e.g. 20x20 px per tile)
	astar_grid->set_cell_size(Size2(GRID_WIDTH, GRID_HEIGHT));

	// Offset the navigation points so that pathfinding is calculated from the cell centers
	astar_grid->set_offset(astar_grid->get_cell_size() / 2);

	// Finalize and initialize the grid data for pathfinding
	astar_grid->update();

	// Get the cell data from the map stream (0 = walkable, 1 = solid/blocked)
	PackedByteArray cell_data = map_stream->get_cell_data();

	 // Iterate through each cell in the grid
	for (uint32_t x = 0; x < grid_cols; x++) {
		for (uint32_t y = 0; y < grid_rows; y++) {
			uint32_t index = x + y * grid_cols;
			// If the cell is marked as solid (blocked), set it as a solid point in the AStarGrid2D
			// Solid points cannot be traversed by the pathfinding algorithm
			if (cell_data[index] == 1) {
				astar_grid->set_point_solid(Vector2i(x, y));
			}
		}
	}
}

#ifdef TOOLS_ENABLED

void TileMapViewer::_load_all_tiles_and_masks_editor() {
	// Iterate over all tile positions and load each tile
	for (uint32_t y = 0; y < map_stream->get_tile_rows(); y++) {
		for (uint32_t x = 0; x < map_stream->get_tile_cols(); x++) {
			_load_tile(x, y);
		}
	}

	// Load all mask fragments for the loaded tiles
	// In editor mode, all tiles are loaded, so all masks should be processed
	_load_masks_for_visible_tiles();
}

#endif

PackedInt32Array TileMapViewer::_get_tile_indices_in_rect(const Rect2 &p_rect) const {
	// Get the total number of tiles horizontally and vertically
	uint32_t tile_cols = map_stream->get_tile_cols();
	uint32_t tile_rows = map_stream->get_tile_rows();

	// Calculate the tile range that intersects with the input rectangle
	uint32_t left_tile = floor(p_rect.position.x / TILE_WIDTH);
	uint32_t top_tile = floor(p_rect.position.y / TILE_HEIGHT);
	uint32_t right_tile = ceil((p_rect.position.x + p_rect.size.x) / TILE_WIDTH) - 1;
	uint32_t bottom_tile = ceil((p_rect.position.y + p_rect.size.y) / TILE_HEIGHT) - 1;

	// Clamp the tile indices to be within the map bounds
	left_tile = CLAMP(left_tile, uint32_t(0), tile_cols - 1);
	right_tile = CLAMP(right_tile, uint32_t(0), tile_cols - 1);
	top_tile = CLAMP(top_tile, uint32_t(0), tile_rows - 1);
	bottom_tile = CLAMP(bottom_tile, uint32_t(0), tile_rows - 1);

	// Collect all tile indices within the calculated rectangle
	PackedInt32Array result;
	for (uint32_t y = top_tile; y <= bottom_tile; y++) {
		for (uint32_t x = left_tile; x <= right_tile; x++) {
			uint32_t index = x + y * tile_cols;
			result.append(index);
		}
	}
	return result;
}

void TileMapViewer::_update_camera_limits() const {
	// Ensure map_stream exists
	if (map_stream.is_null()) {
		return;
	}

	// Get the current viewport
	Viewport *viewport = get_viewport();
	if (!viewport) {
		return;
	}

	// Get the camera from the viewport
	Camera2D *camera = viewport->get_camera_2d();
	if (!camera) {
		return;
	}

	// Set the camera's movement limits to match the map size
	camera->set_limit_rect(Rect2(0, 0, map_stream->get_width(), map_stream->get_height()));
}

Rect2 TileMapViewer::_get_camera_view_rect() const {
	// Get the current viewport
	Viewport *viewport = get_viewport();
	if (!viewport) {
		return Rect2();
	}

	// Get the camera from the viewport
	Camera2D *camera = viewport->get_camera_2d();
	if (!camera) {
		return Rect2();
	}

	// Camera center position
	Point2 center = camera->get_camera_screen_center();

	// Compute view rectangle in world coordinates
	Rect2 view_rect;
	// Adjust size by global scale
	Size2 global_scale = get_global_transform().get_scale();
	view_rect.size = camera->get_viewport_rect().size / global_scale;
	// Top-left corner
	view_rect.position = center - view_rect.size / 2.0;
	return view_rect;
}

void TileMapViewer::_load_visible_tiles() {
	// Ensure map_stream exists
	if (map_stream.is_null()) {
		return;
	}
	
	// Update camera limits to match the new map size
	_update_camera_limits();

	// Get camera visible area in world coordinates
	Rect2 view_rect = _get_camera_view_rect();
	if (view_rect.size == Size2(0, 0)) {
		return;
	}

	// Extend view rect to preload tiles around viewport
	view_rect.position -= Vector2(buffer_tiles * TILE_WIDTH, buffer_tiles * TILE_HEIGHT);
	view_rect.size += Vector2(buffer_tiles * 2 * TILE_WIDTH, buffer_tiles * 2 * TILE_HEIGHT);

	uint32_t tile_cols = map_stream->get_tile_cols();
	uint32_t tile_rows = map_stream->get_tile_rows();

	// Compute tile index range
	int start_x = MAX(int(0), int(view_rect.position.x / TILE_WIDTH));
	int start_y = MAX(int(0), int(view_rect.position.y / TILE_HEIGHT));
	int end_x = MIN(int(tile_cols), int((view_rect.position.x + view_rect.size.x) / TILE_WIDTH + 1));
	int end_y = MIN(int(tile_rows), int((view_rect.position.y + view_rect.size.y) / TILE_HEIGHT + 1));

	// Load tiles within the range
	for (int y = start_y; y < end_y; y++) {
		for (int x = start_x; x < end_x; x++) {
			_load_tile(x, y);
		}
	}

	// Load mask fragments for the visible tiles
	_load_masks_for_visible_tiles();

	// Trigger redraw if new tiles/masks were loaded
	if (needs_redraw) {
		queue_redraw();
	}
}

void TileMapViewer::_load_tile(uint32_t x, uint32_t y) {
	// Ensure map_stream exists
	if (map_stream.is_null()) {
		return;
	}

	uint32_t tile_cols = map_stream->get_tile_cols();
	uint32_t tile_rows = map_stream->get_tile_rows();

	// Check bounds
	if (x >= tile_cols || y >= tile_rows) {
		return;
	}

	uint32_t tile_index = x + y * tile_cols;
	TileData &tile = tiles.write[tile_index];

	// Skip if already loaded
	if (tile.loaded) {
		return;
	}

	// Load tile image
	Ref<Image> image = map_stream->get_tile_image(tile_index);
	if (image.is_valid()) {
		tile.texture = ImageTexture::create_from_image(image);
		tile.loaded = true;
		needs_redraw = true; // Mark for redraw
	}
}

void TileMapViewer::_load_masks_for_visible_tiles() {
	// Iterate over all tiles
	for (TileData tile : tiles) {
		if (!tile.loaded) {
			continue; // Skip tiles that are not loaded yet
		}

		// Iterate over all mask indices associated with this tile
		for (uint32_t mask_index : tile.mask_indices) {
			MaskData &mask = masks.write[mask_index];

			// Load mask texture if not loaded
			if (!mask.loaded) {
				Ref<Image> image = map_stream->get_mask_image(mask_index);
				if (image.is_null()) {
					continue; // Skip if mask image is not available
				}
				mask.texture = ImageTexture::create_from_image(image);
				mask.loaded = true;
			}

			// Iterate over all fragments of this mask
			for (MaskFragment &fragment : mask.fragments) {
				if (fragment.loaded) {
					continue; // Skip already loaded fragments
				}

				// Get the tile corresponding to this fragment
				const TileData mask_tile = tiles[fragment.tile_index];

				// Ensure the mask tile is loaded and has a valid texture
				if (mask_tile.loaded && mask_tile.texture.is_valid()) {

					// Compute the intersection rectangle between mask and tile
					Rect2 inter_rect = mask_tile.region_rect.intersection(mask.region_rect);

					// Define the texture region relative to the tile
					Rect2 region = Rect2(inter_rect.position - mask_tile.region_rect.position, inter_rect.size);

					// Compute offset of mask relative to the mask node
					Vector2 mask_offset = mask_offset = mask_tile.region_rect.position - mask.region_rect.position;

					 // Create the mask node if it does not exist yet
					if (!mask.mask_node) {
						Point2 pos = Point2(mask.region_rect.position.x, mask.region_rect.position.y + mask.region_rect.size.y);

						Node2D *mask_node = memnew(Node2D);
						mask_node->set_position(pos);
						mask.mask_node = mask_node;
						add_child(mask_node); // Add mask node to the TileMapViewer
					}

					// Ensure the static shader is initialized
					_init_static_shader();

					 // Create a ShaderMaterial for this fragment
					Ref<ShaderMaterial> mat;
					mat.instantiate();
					mat->set_shader(s_mask_shader);
					mat->set_shader_parameter("tex_position", region.position);
					mat->set_shader_parameter("tex_size", region.size);
					mat->set_shader_parameter("mask_tex", mask.texture);
					mat->set_shader_parameter("mask_offset", mask_offset);
					mat->set_shader_parameter("mask_tex_size", mask.region_rect.size);

					 // Create an AtlasTexture for the tile fragment
					Ref<AtlasTexture> tex;
					tex.instantiate();
					tex->set_atlas(mask_tile.texture);
					tex->set_region(region);

					 // Create a Sprite2D for this fragment and assign the texture and material
					Sprite2D *mask_sprite = memnew(Sprite2D);
					mask_sprite->set_texture(tex);
					mask_sprite->set_material(mat);
					mask_sprite->set_centered(false);
					mask_sprite->set_position(inter_rect.position - mask.mask_node->get_position());

					// Add the fragment sprite as a child of the mask node
					mask.mask_node->add_child(mask_sprite);

					// Mark this fragment as loaded
					fragment.loaded = true;
				}
			}
		}
	}
}

void TileMapViewer::_draw_tiles() {
	if (map_stream.is_null()) {
		return;
	}

	for (TileData tile : tiles) {
		if (tile.loaded && tile.texture.is_valid()) {
			Rect2 src_rect = Rect2(Vector2(0, 0), tile.region_rect.size);
			draw_texture_rect_region(tile.texture, tile.region_rect, src_rect);
		}
	}
	needs_redraw = false;
}

void TileMapViewer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			_draw_tiles();
		} break;

		case NOTIFICATION_PROCESS: {
			_load_visible_tiles();
		} break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			set_process(is_visible_in_tree());
		} break;
	}
}

bool TileMapViewer::_edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const {
	return get_rect().has_point(p_point);
}

Rect2 TileMapViewer::_edit_get_rect() const {
	return get_rect();
}

bool TileMapViewer::_edit_use_rect() const {
	return map_stream.is_valid();
}

Rect2 TileMapViewer::get_rect() const {
	if (map_stream.is_null()) {
		return Rect2(0, 0, 1, 1);
	}
	Size2i s(map_stream->get_width(), map_stream->get_height());
	if (s == Size2(0, 0)) {
		s = Size2(1, 1);
	}
	return Rect2(Point2(0, 0), s);
}

TileMapViewer::~TileMapViewer() {
	print_line("TileMapViewer release.");
}
