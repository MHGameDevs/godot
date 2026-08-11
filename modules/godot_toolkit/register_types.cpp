/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "core/config/engine.h"
#include "core/io/resource_loader.h"
#include "core/object/class_db.h"

#include "2d/tile_map_viewer.h"
#include "2d/was_sprite_2d.h"
#include "2d/was_animated_sprite_2d.h"
#include "2d/navigation/a_star_grid_agent_2d.h"
#include "2d/navigation/a_star_grid_server_2d.h"
#include "2d/navigation/a_star_grid_path_query_parameters_2d.h"

#include "gui/was_texture_button.h"
#include "gui/extended_item_list.h"

#include "resources/was_style_box_texture.h"
#include "resources/was_texture.h"


#include "io/was_stream.h"
#include "io/map_stream.h"

static Ref<ResourceFormatLoaderWasTexture> resource_loader_was_texture;

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "resources/resource_importer_was.h"

static void _editor_init() {
	Ref<ResourceImporterWas> was_importer;
	was_importer.instantiate();
	ResourceFormatImporter::get_singleton()->add_importer(was_importer);
}
#endif

void initialize_godot_toolkit_module(ModuleInitializationLevel p_level) {
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(ResourceImporterWas);

		EditorNode::add_init_callback(_editor_init);
	}
#endif

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<TileMapViewer>();
	ClassDB::register_class<WasSprite2D>();
	ClassDB::register_class<WasAnimatedSprite2D>();
	ClassDB::register_class<AStarGridAgent2D>();
	ClassDB::register_class<AStarGridServer2D>();
	ClassDB::register_class<AStarGridPathQueryParameters2D>();

	TileMapViewer::init_shaders();

	ClassDB::register_class<WasTextureButton>();
	ClassDB::register_class<ExtendedItemList>();

	ClassDB::register_class<WasStyleBoxTexture>();
	ClassDB::register_class<WasTexture>();
	ClassDB::register_class<WasPaletteTransformSet>();

	ClassDB::register_class<WasStream>();
	ClassDB::register_class<WasImage>();
	ClassDB::register_class<MapStream>();

	Engine::get_singleton()->add_singleton(Engine::Singleton("AStarGridServer2D", AStarGridServer2D::get_singleton()));

	resource_loader_was_texture.instantiate();
	ResourceLoader::add_resource_format_loader(resource_loader_was_texture);
}

void uninitialize_godot_toolkit_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	TileMapViewer::finish_shaders();

	ResourceLoader::remove_resource_format_loader(resource_loader_was_texture);
	resource_loader_was_texture.unref();
}
