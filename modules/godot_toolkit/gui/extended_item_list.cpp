/**************************************************************************/
/*  extended_item_list.cpp                                                */
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
#include "extended_item_list.h"

#include "core/config/project_settings.h"
#include "core/os/os.h"
#include "scene/theme/theme_db.h"

void ExtendedItemList::_shape_text(int p_idx) {
	Item &item = items.write[p_idx];

	item.text_buf->clear();
	if (item.text_direction == Control::TEXT_DIRECTION_INHERITED) {
		item.text_buf->set_direction(is_layout_rtl() ? TextServer::DIRECTION_RTL : TextServer::DIRECTION_LTR);
	} else {
		item.text_buf->set_direction((TextServer::Direction)item.text_direction);
	}
	item.text_buf->add_string(item.xl_text, theme_cache.font, theme_cache.font_size, item.language);
	if (icon_mode == ICON_MODE_TOP && max_text_lines > 0) {
		item.text_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_WORD_BOUND | TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_TRIM_START_EDGE_SPACES | TextServer::BREAK_TRIM_END_EDGE_SPACES);
	} else {
		item.text_buf->set_break_flags(TextServer::BREAK_NONE);
	}
	item.text_buf->set_text_overrun_behavior(text_overrun_behavior);
	item.text_buf->set_max_lines_visible(max_text_lines);

	// sub text
	item.sub_text_buf->clear();
	if (item.text_direction == Control::TEXT_DIRECTION_INHERITED) {
		item.sub_text_buf->set_direction(is_layout_rtl() ? TextServer::DIRECTION_RTL : TextServer::DIRECTION_LTR);
	} else {
		item.sub_text_buf->set_direction((TextServer::Direction)item.text_direction);
	}
	item.sub_text_buf->add_string(item.xl_sub_text, theme_cache.sub_font, theme_cache.sub_font_size, item.language);
	if (icon_mode == ICON_MODE_TOP && max_text_lines > 0) {
		item.sub_text_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_WORD_BOUND | TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_TRIM_START_EDGE_SPACES | TextServer::BREAK_TRIM_END_EDGE_SPACES);
	} else {
		item.sub_text_buf->set_break_flags(TextServer::BREAK_NONE);
	}
	item.sub_text_buf->set_text_overrun_behavior(text_overrun_behavior);
	item.sub_text_buf->set_max_lines_visible(max_text_lines);
}

int ExtendedItemList::add_item(const String &p_item, const Ref<Texture2D> &p_icon, const String &p_sub_text, const Ref<Texture2D> &p_tag_icon, bool p_selectable) {
	Item item;
	item.icon = p_icon;
	item.text = p_item;
	item.sub_text = p_sub_text;
	item.tag_icon = p_tag_icon;
	item.selectable = p_selectable;
	items.push_back(item);
	int item_id = items.size() - 1;

	items.write[item_id].xl_text = _atr(item_id, p_item);
	items.write[item_id].xl_sub_text = _atr(item_id, p_sub_text);
	_shape_text(item_id);

	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	notify_property_list_changed();
	return item_id;
}

int ExtendedItemList::add_icon_item(const Ref<Texture2D> &p_item, bool p_selectable) {
	Item item;
	item.icon = p_item;
	item.selectable = p_selectable;
	items.push_back(item);
	int item_id = items.size() - 1;

	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	notify_property_list_changed();
	return item_id;
}

void ExtendedItemList::set_item_text(int p_idx, const String &p_text) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].text == p_text) {
		return;
	}

	items.write[p_idx].text = p_text;
	items.write[p_idx].xl_text = _atr(p_idx, p_text);
	_shape_text(p_idx);
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
}

String ExtendedItemList::get_item_text(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), String());
	return items[p_idx].text;
}

void ExtendedItemList::set_item_sub_text(int p_idx, const String &p_sub_text) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].sub_text == p_sub_text) {
		return;
	}

	items.write[p_idx].sub_text = p_sub_text;
	items.write[p_idx].xl_sub_text = _atr(p_idx, p_sub_text);
	_shape_text(p_idx);
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
}

String ExtendedItemList::get_item_sub_text(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), String());
	return items[p_idx].sub_text;
}


void ExtendedItemList::set_item_text_direction(int p_idx, Control::TextDirection p_text_direction) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());
	ERR_FAIL_COND((int)p_text_direction < -1 || (int)p_text_direction > 3);
	if (items[p_idx].text_direction != p_text_direction) {
		items.write[p_idx].text_direction = p_text_direction;
		_shape_text(p_idx);
		queue_accessibility_update();
		queue_redraw();
	}
}

Control::TextDirection ExtendedItemList::get_item_text_direction(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), TEXT_DIRECTION_INHERITED);
	return items[p_idx].text_direction;
}

void ExtendedItemList::set_item_language(int p_idx, const String &p_language) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());
	if (items[p_idx].language != p_language) {
		items.write[p_idx].language = p_language;
		_shape_text(p_idx);
		queue_accessibility_update();
		queue_redraw();
	}
}

String ExtendedItemList::get_item_language(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), "");
	return items[p_idx].language;
}

void ExtendedItemList::set_item_auto_translate_mode(int p_idx, AutoTranslateMode p_mode) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());
	if (items[p_idx].auto_translate_mode != p_mode) {
		items.write[p_idx].auto_translate_mode = p_mode;
		items.write[p_idx].xl_text = _atr(p_idx, items[p_idx].text);
		_shape_text(p_idx);
		queue_accessibility_update();
		queue_redraw();
	}
}

Node::AutoTranslateMode ExtendedItemList::get_item_auto_translate_mode(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), AUTO_TRANSLATE_MODE_INHERIT);
	return items[p_idx].auto_translate_mode;
}

void ExtendedItemList::set_item_tooltip_enabled(int p_idx, const bool p_enabled) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());
	if (items[p_idx].tooltip_enabled != p_enabled) {
		items.write[p_idx].tooltip_enabled = p_enabled;
		items.write[p_idx].accessibility_item_dirty = true;
		queue_accessibility_update();
	}
}

bool ExtendedItemList::is_item_tooltip_enabled(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), false);
	return items[p_idx].tooltip_enabled;
}

void ExtendedItemList::set_item_tooltip(int p_idx, const String &p_tooltip) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].tooltip == p_tooltip) {
		return;
	}

	items.write[p_idx].tooltip = p_tooltip;
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
}

String ExtendedItemList::get_item_tooltip(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), String());
	return items[p_idx].tooltip;
}

void ExtendedItemList::set_item_icon(int p_idx, const Ref<Texture2D> &p_icon) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].icon == p_icon) {
		return;
	}

	items.write[p_idx].icon = p_icon;
	queue_redraw();
	shape_changed = true;
}

Ref<Texture2D> ExtendedItemList::get_item_icon(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Ref<Texture2D>());

	return items[p_idx].icon;
}

void ExtendedItemList::set_item_icon_transposed(int p_idx, const bool p_transposed) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].icon_transposed == p_transposed) {
		return;
	}

	items.write[p_idx].icon_transposed = p_transposed;
	queue_redraw();
	shape_changed = true;
}

bool ExtendedItemList::is_item_icon_transposed(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), false);

	return items[p_idx].icon_transposed;
}

void ExtendedItemList::set_item_icon_region(int p_idx, const Rect2 &p_region) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].icon_region == p_region) {
		return;
	}

	items.write[p_idx].icon_region = p_region;
	queue_redraw();
	shape_changed = true;
}

Rect2 ExtendedItemList::get_item_icon_region(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Rect2());

	return items[p_idx].icon_region;
}

void ExtendedItemList::set_item_icon_modulate(int p_idx, const Color &p_modulate) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].icon_modulate == p_modulate) {
		return;
	}

	items.write[p_idx].icon_modulate = p_modulate;
	queue_redraw();
}

Color ExtendedItemList::get_item_icon_modulate(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Color());

	return items[p_idx].icon_modulate;
}

void ExtendedItemList::set_item_custom_bg_color(int p_idx, const Color &p_custom_bg_color) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].custom_bg == p_custom_bg_color) {
		return;
	}

	items.write[p_idx].custom_bg = p_custom_bg_color;
	queue_redraw();
}

Color ExtendedItemList::get_item_custom_bg_color(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Color());

	return items[p_idx].custom_bg;
}

void ExtendedItemList::set_item_custom_fg_color(int p_idx, const Color &p_custom_fg_color) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].custom_fg == p_custom_fg_color) {
		return;
	}

	items.write[p_idx].custom_fg = p_custom_fg_color;
	queue_redraw();
}

Color ExtendedItemList::get_item_custom_fg_color(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Color());

	return items[p_idx].custom_fg;
}

Rect2 ExtendedItemList::get_item_rect(int p_idx, bool p_expand) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Rect2());

	Rect2 ret = items[p_idx].rect_cache;
	if (p_expand && p_idx % current_columns == current_columns - 1) {
		int width = get_size().width - theme_cache.panel_style->get_minimum_size().width;
		if (scroll_bar_v->is_visible()) {
			width -= scroll_bar_v->get_combined_minimum_size().width;
		}
		ret.size.width = width - ret.position.x;
	}
	ret.position += theme_cache.panel_style->get_offset();
	return ret;
}

void ExtendedItemList::set_item_tag_icon(int p_idx, const Ref<Texture2D> &p_tag_icon) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].tag_icon == p_tag_icon) {
		return;
	}

	items.write[p_idx].tag_icon = p_tag_icon;
	queue_redraw();
	shape_changed = true;
}

Ref<Texture2D> ExtendedItemList::get_item_tag_icon(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Ref<Texture2D>());

	return items[p_idx].tag_icon;
}

void ExtendedItemList::set_item_selectable(int p_idx, bool p_selectable) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	items.write[p_idx].selectable = p_selectable;
	items.write[p_idx].accessibility_item_dirty = true;
	queue_accessibility_update();
}

bool ExtendedItemList::is_item_selectable(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), false);
	return items[p_idx].selectable;
}

void ExtendedItemList::set_item_disabled(int p_idx, bool p_disabled) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].disabled == p_disabled) {
		return;
	}

	items.write[p_idx].disabled = p_disabled;
	items.write[p_idx].accessibility_item_dirty = true;
	queue_accessibility_update();
	queue_redraw();
}

bool ExtendedItemList::is_item_disabled(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), false);
	return items[p_idx].disabled;
}

void ExtendedItemList::set_item_metadata(int p_idx, const Variant &p_metadata) {
	if (p_idx < 0) {
		p_idx += get_item_count();
	}
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].metadata == p_metadata) {
		return;
	}

	items.write[p_idx].metadata = p_metadata;
	queue_redraw();
	shape_changed = true;
}

Variant ExtendedItemList::get_item_metadata(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), Variant());
	return items[p_idx].metadata;
}

void ExtendedItemList::select(int p_idx, bool p_single) {
	ERR_FAIL_INDEX(p_idx, items.size());

	if (p_single || select_mode == SELECT_SINGLE) {
		if (!items[p_idx].selectable || items[p_idx].disabled) {
			return;
		}

		for (int i = 0; i < items.size(); i++) {
			if (items.write[i].selected != (p_idx == i)) {
				items.write[i].selected = (p_idx == i);
				items.write[i].accessibility_item_dirty = true;
			}
		}

		current = p_idx;
		ensure_selected_visible = false;
	} else {
		if (items[p_idx].selectable && !items[p_idx].disabled) {
			items.write[p_idx].selected = true;
			items.write[p_idx].accessibility_item_dirty = true;
		}
	}
	queue_accessibility_update();
	queue_redraw();
}

void ExtendedItemList::deselect(int p_idx) {
	ERR_FAIL_INDEX(p_idx, items.size());

	if (select_mode == SELECT_SINGLE) {
		items.write[p_idx].selected = false;
		current = -1;
	} else {
		items.write[p_idx].selected = false;
	}
	items.write[p_idx].accessibility_item_dirty = true;
	queue_accessibility_update();
	queue_redraw();
}

void ExtendedItemList::deselect_all() {
	if (items.is_empty()) {
		return;
	}

	for (int i = 0; i < items.size(); i++) {
		if (items.write[i].selected) {
			items.write[i].selected = false;
			items.write[i].accessibility_item_dirty = true;
		}
	}
	current = -1;
	queue_accessibility_update();
	queue_redraw();
}

bool ExtendedItemList::is_selected(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), false);

	return items[p_idx].selected;
}

void ExtendedItemList::set_current(int p_current) {
	ERR_FAIL_INDEX(p_current, items.size());

	if (current == p_current) {
		return;
	}

	if (select_mode == SELECT_SINGLE) {
		select(p_current, true);
	} else {
		current = p_current;
		queue_accessibility_update();
		queue_redraw();
	}
}

int ExtendedItemList::get_current() const {
	return current;
}

void ExtendedItemList::move_item(int p_from_idx, int p_to_idx) {
	ERR_FAIL_INDEX(p_from_idx, items.size());
	ERR_FAIL_INDEX(p_to_idx, items.size());

	if (is_anything_selected() && get_selected_items()[0] == p_from_idx) {
		current = p_to_idx;
	}

	Item item = items[p_from_idx];
	items.remove_at(p_from_idx);
	items.insert(p_to_idx, item);

	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	notify_property_list_changed();
}

void ExtendedItemList::set_item_count(int p_count) {
	ERR_FAIL_COND(p_count < 0);

	if (items.size() == p_count) {
		return;
	}

	if (items.size() > p_count) {
		for (int i = p_count; i < items.size(); i++) {
			if (items[i].accessibility_item_element.is_valid()) {
				DisplayServer::get_singleton()->accessibility_free_element(items.write[i].accessibility_item_element);
				items.write[i].accessibility_item_element = RID();
			}
		}
	}

	items.resize(p_count);
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	notify_property_list_changed();
}

int ExtendedItemList::get_item_count() const {
	return items.size();
}

void ExtendedItemList::remove_item(int p_idx) {
	ERR_FAIL_INDEX(p_idx, items.size());

	if (items[p_idx].accessibility_item_element.is_valid()) {
		DisplayServer::get_singleton()->accessibility_free_element(items.write[p_idx].accessibility_item_element);
		items.write[p_idx].accessibility_item_element = RID();
	}
	items.remove_at(p_idx);
	if (current == p_idx) {
		current = -1;
	}
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	defer_select_single = -1;
	notify_property_list_changed();
}

void ExtendedItemList::clear() {
	for (int i = 0; i < items.size(); i++) {
		if (items[i].accessibility_item_element.is_valid()) {
			DisplayServer::get_singleton()->accessibility_free_element(items.write[i].accessibility_item_element);
			items.write[i].accessibility_item_element = RID();
		}
	}
	items.clear();
	current = -1;
	ensure_selected_visible = false;
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
	defer_select_single = -1;
	notify_property_list_changed();
}

void ExtendedItemList::set_fixed_column_width(int p_size) {
	ERR_FAIL_COND(p_size < 0);

	if (fixed_column_width == p_size) {
		return;
	}

	fixed_column_width = p_size;
	queue_redraw();
	shape_changed = true;
}

int ExtendedItemList::get_fixed_column_width() const {
	return fixed_column_width;
}

void ExtendedItemList::set_same_column_width(bool p_enable) {
	if (same_column_width == p_enable) {
		return;
	}

	same_column_width = p_enable;
	queue_redraw();
	shape_changed = true;
}

bool ExtendedItemList::is_same_column_width() const {
	return same_column_width;
}

void ExtendedItemList::set_max_text_lines(int p_lines) {
	ERR_FAIL_COND(p_lines < 1);
	if (max_text_lines != p_lines) {
		max_text_lines = p_lines;
		for (int i = 0; i < items.size(); i++) {
			if (icon_mode == ICON_MODE_TOP && max_text_lines > 0) {
				items.write[i].text_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_WORD_BOUND | TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_TRIM_START_EDGE_SPACES | TextServer::BREAK_TRIM_END_EDGE_SPACES);
				items.write[i].text_buf->set_max_lines_visible(p_lines);
			} else {
				items.write[i].text_buf->set_break_flags(TextServer::BREAK_NONE);
			}
		}
		shape_changed = true;
		queue_accessibility_update();
		queue_redraw();
	}
}

int ExtendedItemList::get_max_text_lines() const {
	return max_text_lines;
}

void ExtendedItemList::set_max_columns(int p_amount) {
	ERR_FAIL_COND(p_amount < 0);

	if (max_columns == p_amount) {
		return;
	}

	max_columns = p_amount;
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;
}

int ExtendedItemList::get_max_columns() const {
	return max_columns;
}

void ExtendedItemList::set_select_mode(SelectMode p_mode) {
	if (select_mode == p_mode) {
		return;
	}

	select_mode = p_mode;
	queue_accessibility_update();
	queue_redraw();
}

ExtendedItemList::SelectMode ExtendedItemList::get_select_mode() const {
	return select_mode;
}

void ExtendedItemList::set_icon_mode(IconMode p_mode) {
	ERR_FAIL_INDEX((int)p_mode, 2);
	if (icon_mode != p_mode) {
		icon_mode = p_mode;
		for (int i = 0; i < items.size(); i++) {
			if (icon_mode == ICON_MODE_TOP && max_text_lines > 0) {
				items.write[i].text_buf->set_break_flags(TextServer::BREAK_MANDATORY | TextServer::BREAK_WORD_BOUND | TextServer::BREAK_GRAPHEME_BOUND | TextServer::BREAK_TRIM_START_EDGE_SPACES | TextServer::BREAK_TRIM_END_EDGE_SPACES);
			} else {
				items.write[i].text_buf->set_break_flags(TextServer::BREAK_NONE);
			}
		}
		shape_changed = true;
		queue_redraw();
	}
}

ExtendedItemList::IconMode ExtendedItemList::get_icon_mode() const {
	return icon_mode;
}

void ExtendedItemList::set_fixed_icon_size(const Size2i &p_size) {
	if (fixed_icon_size == p_size) {
		return;
	}

	fixed_icon_size = p_size;
	queue_redraw();
	shape_changed = true;
}

Size2i ExtendedItemList::get_fixed_icon_size() const {
	return fixed_icon_size;
}

Size2 ExtendedItemList::Item::get_icon_size() const {
	if (icon.is_null()) {
		return Size2();
	}

	Size2 size_result = Size2(icon_region.size).abs();
	if (icon_region.size.x == 0 || icon_region.size.y == 0) {
		size_result = icon->get_size();
	}

	if (icon_transposed) {
		Size2 size_tmp = size_result;
		size_result.x = size_tmp.y;
		size_result.y = size_tmp.x;
	}

	return size_result;
}

void ExtendedItemList::set_fixed_tag_icon_size(const Size2i &p_size) {
	if (fixed_tag_icon_size == p_size) {
		return;
	}

	fixed_tag_icon_size = p_size;
	queue_redraw();
	shape_changed = true;
}

Size2i ExtendedItemList::get_fixed_tag_icon_size() const {
	return fixed_tag_icon_size;
}

void ExtendedItemList::set_tag_icon_offset(const Vector2 &p_offset) {
	if (tag_icon_offset == p_offset) {
		return;
	}

	tag_icon_offset = p_offset;
	queue_redraw();
	shape_changed = true;
}

Vector2 ExtendedItemList::get_tag_icon_offset() const {
	return tag_icon_offset;
}

void ExtendedItemList::gui_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

#define CAN_SELECT(i) (items[i].selectable && !items[i].disabled)
#define IS_SAME_ROW(i, row) (i / current_columns == row)

	double prev_scroll_v = scroll_bar_v->get_value();
	double prev_scroll_h = scroll_bar_h->get_value();
	bool scroll_value_modified = false;

	Ref<InputEventMouseMotion> mm = p_event;
	if (defer_select_single >= 0 && mm.is_valid()) {
		defer_select_single = -1;
		return;
	}

	Ref<InputEventMouseButton> mb = p_event;
	Ref<InputEventKey> ev_key = p_event;

	if (ev_key.is_valid() && ev_key->get_keycode() == Key::SHIFT && !ev_key->is_pressed()) {
		shift_anchor = -1;
	}

	if (defer_select_single >= 0 && mb.is_valid() && mb->get_button_index() == MouseButton::LEFT && !mb->is_pressed()) {
		select(defer_select_single, true);

		emit_signal(SNAME("multi_selected"), defer_select_single, true);
		defer_select_single = -1;
		return;
	}

	if (mm.is_valid()) {
		int closest = get_item_at_position(mm->get_position(), true);
		if (closest != hovered) {
			prev_hovered = hovered;
			hovered = closest;
			queue_accessibility_update();
			queue_redraw();
		}
	}

	if (mb.is_valid() && mb->is_pressed()) {
		search_string = ""; //any mousepress cancels

		int closest = get_item_at_position(mb->get_position(), true);

		if (closest != -1 && (mb->get_button_index() == MouseButton::LEFT || (allow_rmb_select && mb->get_button_index() == MouseButton::RIGHT))) {
			int i = closest;

			if (items[i].disabled) {
				// Don't emit any signal or do any action with clicked item when disabled.
				return;
			}

			if (select_mode == SELECT_MULTI && items[i].selected && mb->is_command_or_control_pressed()) {
				deselect(i);
				emit_signal(SNAME("multi_selected"), i, false);

			} else if (select_mode == SELECT_MULTI && mb->is_shift_pressed() && current >= 0 && current < items.size() && current != i) {
				// Range selection.

				int from = current;
				int to = i;
				if (i < current) {
					SWAP(from, to);
				}
				for (int j = from; j <= to; j++) {
					if (!CAN_SELECT(j)) {
						// Item is not selectable during a range selection, so skip it.
						continue;
					}
					bool selected = !items[j].selected;
					select(j, false);
					if (selected) {
						emit_signal(SNAME("multi_selected"), j, true);
					}
				}
				emit_signal(SNAME("item_clicked"), i, get_local_mouse_position(), mb->get_button_index());

			} else {
				if (!mb->is_double_click() &&
						!mb->is_command_or_control_pressed() &&
						select_mode == SELECT_MULTI &&
						items[i].selectable &&
						items[i].selected &&
						mb->get_button_index() == MouseButton::LEFT) {
					defer_select_single = i;
					return;
				}

				if (select_mode == SELECT_TOGGLE) {
					if (items[i].selectable) {
						if (items[i].selected) {
							deselect(i);
							current = i;
							emit_signal(SNAME("multi_selected"), i, false);
						} else {
							select(i, false);
							current = i;
							emit_signal(SNAME("multi_selected"), i, true);
						}
					}
				} else if (items[i].selectable && (!items[i].selected || allow_reselect)) {
					select(i, select_mode == SELECT_SINGLE || !mb->is_command_or_control_pressed());

					if (select_mode == SELECT_SINGLE) {
						emit_signal(SceneStringName(item_selected), i);
					} else {
						emit_signal(SNAME("multi_selected"), i, true);
					}
				}

				emit_signal(SNAME("item_clicked"), i, get_local_mouse_position(), mb->get_button_index());

				if (mb->get_button_index() == MouseButton::LEFT && mb->is_double_click()) {
					emit_signal(SNAME("item_activated"), i);
				}
			}

			return;
		} else if (closest != -1) {
			if (!items[closest].disabled) {
				emit_signal(SNAME("item_clicked"), closest, get_local_mouse_position(), mb->get_button_index());
			}
		} else {
			// Since closest is null, more likely we clicked on empty space, so send signal to interested controls. Allows, for example, implement items deselecting.
			emit_signal(SNAME("empty_clicked"), get_local_mouse_position(), mb->get_button_index());
		}
	}
	if (mb.is_valid()) { // Copied from ScrollContainer.
		if (mb->is_pressed()) {
			bool v_scroll_hidden = !scroll_bar_v->is_visible();
			if (mb->get_button_index() == MouseButton::WHEEL_UP) {
				// By default, the vertical orientation takes precedence. This is an exception.
				if (mb->is_shift_pressed() || v_scroll_hidden) {
					scroll_bar_h->scroll(-scroll_bar_h->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				} else {
					scroll_bar_v->scroll(-scroll_bar_v->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				}
			}
			if (mb->get_button_index() == MouseButton::WHEEL_DOWN) {
				if (mb->is_shift_pressed() || v_scroll_hidden) {
					scroll_bar_h->scroll(scroll_bar_h->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				} else {
					scroll_bar_v->scroll(scroll_bar_v->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				}
			}

			bool h_scroll_hidden = !scroll_bar_h->is_visible();
			if (mb->get_button_index() == MouseButton::WHEEL_LEFT) {
				// By default, the horizontal orientation takes precedence. This is an exception.
				if (mb->is_shift_pressed() || h_scroll_hidden) {
					scroll_bar_v->scroll(-scroll_bar_v->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				} else {
					scroll_bar_h->scroll(-scroll_bar_h->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				}
			}
			if (mb->get_button_index() == MouseButton::WHEEL_RIGHT) {
				if (mb->is_shift_pressed() || h_scroll_hidden) {
					scroll_bar_v->scroll(scroll_bar_v->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				} else {
					scroll_bar_h->scroll(scroll_bar_h->get_page() / 8 * mb->get_factor());
					scroll_value_modified = true;
				}
			}
		}
	}

	if (p_event->is_pressed() && items.size() > 0) {
		if (p_event->is_action("ui_menu", true)) {
			if (current != -1 && allow_rmb_select) {
				int i = current;

				if (items[i].disabled) {
					// Don't emit any signal or do any action with clicked item when disabled.
					return;
				}

				emit_signal(SNAME("item_clicked"), i, get_item_rect(i).position, MouseButton::RIGHT);

				accept_event();
				return;
			}
		}
		// Shift Up Selection.
		if (select_mode == SELECT_MULTI && p_event->is_action("ui_up", false) && ev_key.is_valid() && ev_key->is_shift_pressed()) {
			int next = MAX(current - max_columns, 0);
			_shift_range_select(current, next);
			accept_event();
		}

		else if (p_event->is_action("ui_up", true)) {
			if (!search_string.is_empty()) {
				uint64_t now = OS::get_singleton()->get_ticks_msec();
				uint64_t diff = now - search_time_msec;

				if (diff < uint64_t(GLOBAL_GET_CACHED(uint64_t, "gui/timers/incremental_search_max_interval_msec")) * 2) {
					for (int i = current - 1; i >= 0; i--) {
						if (CAN_SELECT(i) && items[i].text.begins_with(search_string)) {
							set_current(i);
							ensure_current_is_visible();
							if (select_mode == SELECT_SINGLE) {
								emit_signal(SceneStringName(item_selected), current);
							}

							break;
						}
					}
					accept_event();
					return;
				}
			}

			if (current >= current_columns) {
				int next = current - current_columns;
				while (next >= 0 && !CAN_SELECT(next)) {
					next = next - current_columns;
				}
				if (next < 0) {
					accept_event();
					return;
				}
				set_current(next);
				ensure_current_is_visible();
				if (select_mode == SELECT_SINGLE) {
					emit_signal(SceneStringName(item_selected), current);
				}
				accept_event();
			}
		}

		// Shift Down Selection.
		else if (select_mode == SELECT_MULTI && p_event->is_action("ui_down", false) && ev_key.is_valid() && ev_key->is_shift_pressed()) {
			int next = MIN(current + max_columns, items.size() - 1);
			_shift_range_select(current, next);
			accept_event();
		}

		else if (p_event->is_action("ui_down", true)) {
			if (!search_string.is_empty()) {
				uint64_t now = OS::get_singleton()->get_ticks_msec();
				uint64_t diff = now - search_time_msec;

				if (diff < uint64_t(GLOBAL_GET_CACHED(uint64_t, "gui/timers/incremental_search_max_interval_msec")) * 2) {
					for (int i = current + 1; i < items.size(); i++) {
						if (CAN_SELECT(i) && items[i].text.begins_with(search_string)) {
							set_current(i);
							ensure_current_is_visible();
							if (select_mode == SELECT_SINGLE) {
								emit_signal(SceneStringName(item_selected), current);
							}
							break;
						}
					}
					accept_event();
					return;
				}
			}

			if (current < items.size() - current_columns) {
				int next = current + current_columns;
				while (next < items.size() && !CAN_SELECT(next)) {
					next = next + current_columns;
				}
				if (next >= items.size()) {
					accept_event();
					return;
				}
				set_current(next);
				ensure_current_is_visible();
				if (select_mode == SELECT_SINGLE) {
					emit_signal(SceneStringName(item_selected), current);
				}
				accept_event();
			}
		} else if (p_event->is_action("ui_page_up", true)) {
			search_string = ""; //any mousepress cancels

			for (int i = 4; i > 0; i--) {
				int index = current - current_columns * i;
				if (index >= 0 && index < items.size() && CAN_SELECT(index)) {
					set_current(index);
					ensure_current_is_visible();
					if (select_mode == SELECT_SINGLE) {
						emit_signal(SceneStringName(item_selected), current);
					}
					accept_event();
					break;
				}
			}
		} else if (p_event->is_action("ui_page_down", true)) {
			search_string = ""; //any mousepress cancels

			for (int i = 4; i > 0; i--) {
				int index = current + current_columns * i;
				if (index >= 0 && index < items.size() && CAN_SELECT(index)) {
					set_current(index);
					ensure_current_is_visible();
					if (select_mode == SELECT_SINGLE) {
						emit_signal(SceneStringName(item_selected), current);
					}
					accept_event();

					break;
				}
			}
		}

		// Shift Left Selection.
		else if (select_mode == SELECT_MULTI && p_event->is_action("ui_left", false) && ev_key.is_valid() && ev_key->is_shift_pressed()) {
			int next = MAX(current - 1, 0);
			_shift_range_select(current, next);
			accept_event();
		}

		else if (p_event->is_action("ui_left", true)) {
			search_string = ""; //any mousepress cancels

			if (current % current_columns != 0) {
				int current_row = current / current_columns;
				int next = current - 1;
				while (next >= 0 && !CAN_SELECT(next)) {
					next = next - 1;
				}
				if (next < 0 || !IS_SAME_ROW(next, current_row)) {
					accept_event();
					return;
				}
				set_current(next);
				ensure_current_is_visible();
				if (select_mode == SELECT_SINGLE) {
					emit_signal(SceneStringName(item_selected), current);
				}
				accept_event();
			}
		}

		// Shift Right Selection.
		else if (select_mode == SELECT_MULTI && p_event->is_action("ui_right", false) && ev_key.is_valid() && ev_key->is_shift_pressed()) {
			int next = MIN(current + 1, items.size() - 1);
			_shift_range_select(current, next);
			accept_event();
		}

		else if (p_event->is_action("ui_right", true)) {
			search_string = ""; //any mousepress cancels

			if (current % current_columns != (current_columns - 1) && current + 1 < items.size()) {
				int current_row = current / current_columns;
				int next = current + 1;
				while (next < items.size() && !CAN_SELECT(next)) {
					next = next + 1;
				}
				if (items.size() <= next || !IS_SAME_ROW(next, current_row)) {
					accept_event();
					return;
				}
				set_current(next);
				ensure_current_is_visible();
				if (select_mode == SELECT_SINGLE) {
					emit_signal(SceneStringName(item_selected), current);
				}
				accept_event();
			}
		} else if (p_event->is_action("ui_cancel", true)) {
			search_string = "";
		} else if (p_event->is_action("ui_select", true) && (select_mode == SELECT_MULTI || select_mode == SELECT_TOGGLE)) {
			if (current >= 0 && current < items.size()) {
				if (CAN_SELECT(current) && !items[current].selected) {
					select(current, false);
					emit_signal(SNAME("multi_selected"), current, true);
				} else if (items[current].selected) {
					deselect(current);
					emit_signal(SNAME("multi_selected"), current, false);
				}
			}
		} else if (p_event->is_action("ui_accept", true)) {
			search_string = ""; //any mousepress cancels

			if (current >= 0 && current < items.size() && !items[current].disabled) {
				emit_signal(SNAME("item_activated"), current);
			}
		} else {
			Ref<InputEventKey> k = p_event;

			if (allow_search && k.is_valid() && k->get_unicode()) {
				uint64_t now = OS::get_singleton()->get_ticks_msec();
				uint64_t diff = now - search_time_msec;
				uint64_t max_interval = uint64_t(GLOBAL_GET_CACHED(uint64_t, "gui/timers/incremental_search_max_interval_msec"));
				search_time_msec = now;

				if (diff > max_interval) {
					search_string = "";
				}

				if (String::chr(k->get_unicode()) != search_string) {
					search_string += String::chr(k->get_unicode());
				}

				for (int i = current + 1; i <= items.size(); i++) {
					if (i == items.size()) {
						if (current == 0 || current == -1) {
							break;
						} else {
							i = 0;
						}
					}

					if (i == current) {
						break;
					}

					if (items[i].text.findn(search_string) == 0) {
						set_current(i);
						ensure_current_is_visible();
						if (select_mode == SELECT_SINGLE) {
							emit_signal(SceneStringName(item_selected), current);
						}
						break;
					}
				}
			}
		}
	}

	Ref<InputEventPanGesture> pan_gesture = p_event;
	if (pan_gesture.is_valid()) {
		scroll_bar_v->set_value(scroll_bar_v->get_value() + scroll_bar_v->get_page() * pan_gesture->get_delta().y / 8);
		scroll_bar_h->set_value(scroll_bar_h->get_value() + scroll_bar_h->get_page() * pan_gesture->get_delta().x / 8);
	}

	if (scroll_value_modified && (scroll_bar_v->get_value() != prev_scroll_v || scroll_bar_h->get_value() != prev_scroll_h)) {
		accept_event(); //accept event if scroll changed
	}

#undef CAN_SELECT
#undef IS_SAME_ROW
}

void ExtendedItemList::ensure_current_is_visible() {
	ensure_selected_visible = true;
	queue_redraw();
}

static Rect2 _adjust_to_max_size(Size2 p_size, Size2 p_max_size) {
	Size2 size = p_max_size;
	int tex_width = p_size.width * size.height / p_size.height;
	int tex_height = size.height;

	if (tex_width > size.width) {
		tex_width = size.width;
		tex_height = p_size.height * tex_width / p_size.width;
	}

	int ofs_x = (size.width - tex_width) / 2;
	int ofs_y = (size.height - tex_height) / 2;

	return Rect2(ofs_x, ofs_y, tex_width, tex_height);
}

RID ExtendedItemList::get_focused_accessibility_element() const {
	if (current == -1) {
		return get_accessibility_element();
	} else {
		const Item &item = items[current];
		return item.accessibility_item_element;
	}
}

void ExtendedItemList::_accessibility_action_scroll_set(const Variant &p_data) {
	const Point2 &pos = p_data;
	scroll_bar_h->set_value(pos.x);
	scroll_bar_v->set_value(pos.y);
}

void ExtendedItemList::_accessibility_action_scroll_up(const Variant &p_data) {
	if ((DisplayServer::AccessibilityScrollUnit)p_data == DisplayServer::SCROLL_UNIT_ITEM) {
		scroll_bar_v->set_value(scroll_bar_v->get_value() - scroll_bar_v->get_page() / 4);
	} else {
		scroll_bar_v->set_value(scroll_bar_v->get_value() - scroll_bar_v->get_page());
	}
}

void ExtendedItemList::_accessibility_action_scroll_down(const Variant &p_data) {
	if ((DisplayServer::AccessibilityScrollUnit)p_data == DisplayServer::SCROLL_UNIT_ITEM) {
		scroll_bar_v->set_value(scroll_bar_v->get_value() + scroll_bar_v->get_page() / 4);
	} else {
		scroll_bar_v->set_value(scroll_bar_v->get_value() + scroll_bar_v->get_page());
	}
}

void ExtendedItemList::_accessibility_action_scroll_left(const Variant &p_data) {
	if ((DisplayServer::AccessibilityScrollUnit)p_data == DisplayServer::SCROLL_UNIT_ITEM) {
		scroll_bar_h->set_value(scroll_bar_h->get_value() - scroll_bar_h->get_page() / 4);
	} else {
		scroll_bar_h->set_value(scroll_bar_h->get_value() - scroll_bar_h->get_page());
	}
}

void ExtendedItemList::_accessibility_action_scroll_right(const Variant &p_data) {
	if ((DisplayServer::AccessibilityScrollUnit)p_data == DisplayServer::SCROLL_UNIT_ITEM) {
		scroll_bar_h->set_value(scroll_bar_h->get_value() + scroll_bar_h->get_page() / 4);
	} else {
		scroll_bar_h->set_value(scroll_bar_h->get_value() + scroll_bar_h->get_page());
	}
}

void ExtendedItemList::_accessibility_action_scroll_into_view(const Variant &p_data, int p_index) {
	ERR_FAIL_INDEX(p_index, items.size());

	Rect2 r = items[p_index].rect_cache;
	int from_v = scroll_bar_v->get_value();
	int to_v = from_v + scroll_bar_v->get_page();
	int from_h = scroll_bar_h->get_value();
	int to_h = from_h + scroll_bar_h->get_page();

	if (r.position.y < from_v) {
		scroll_bar_v->set_value(r.position.y);
	} else if (r.position.y + r.size.y > to_v) {
		scroll_bar_v->set_value(r.position.y + r.size.y - (to_v - from_v));
	}
	if (r.position.x < from_h) {
		scroll_bar_h->set_value(r.position.x);
	} else if (r.position.x + r.size.x > to_h) {
		scroll_bar_h->set_value(r.position.x + r.size.x - (to_h - from_h));
	}
}

void ExtendedItemList::_accessibility_action_focus(const Variant &p_data, int p_index) {
	select(p_index);
}

void ExtendedItemList::_accessibility_action_blur(const Variant &p_data, int p_index) {
	deselect(p_index);
}
#include "core/math/math_defs.h"
void ExtendedItemList::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_EXIT_TREE:
		case NOTIFICATION_ACCESSIBILITY_INVALIDATE: {
			for (int i = 0; i < items.size(); i++) {
				items.write[i].accessibility_item_element = RID();
			}
			accessibility_scroll_element = RID();
		} break;

		case NOTIFICATION_ACCESSIBILITY_UPDATE: {
			RID ae = get_accessibility_element();
			ERR_FAIL_COND(ae.is_null());

			force_update_list_size();

			DisplayServer::get_singleton()->accessibility_update_set_role(ae, DisplayServer::AccessibilityRole::ROLE_LIST_BOX);
			DisplayServer::get_singleton()->accessibility_update_set_list_item_count(ae, items.size());
			DisplayServer::get_singleton()->accessibility_update_set_flag(ae, DisplayServer::AccessibilityFlags::FLAG_MULTISELECTABLE, select_mode == SELECT_MULTI);
			DisplayServer::get_singleton()->accessibility_update_add_action(ae, DisplayServer::AccessibilityAction::ACTION_SCROLL_DOWN, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_down));
			DisplayServer::get_singleton()->accessibility_update_add_action(ae, DisplayServer::AccessibilityAction::ACTION_SCROLL_UP, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_up));
			DisplayServer::get_singleton()->accessibility_update_add_action(ae, DisplayServer::AccessibilityAction::ACTION_SCROLL_LEFT, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_left));
			DisplayServer::get_singleton()->accessibility_update_add_action(ae, DisplayServer::AccessibilityAction::ACTION_SCROLL_RIGHT, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_right));
			DisplayServer::get_singleton()->accessibility_update_add_action(ae, DisplayServer::AccessibilityAction::ACTION_SET_SCROLL_OFFSET, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_set));

			if (accessibility_scroll_element.is_null()) {
				accessibility_scroll_element = DisplayServer::get_singleton()->accessibility_create_sub_element(ae, DisplayServer::AccessibilityRole::ROLE_CONTAINER);
			}

			Transform2D scroll_xform;
			scroll_xform.set_origin(Vector2i(-scroll_bar_h->get_value(), -scroll_bar_v->get_value()));
			DisplayServer::get_singleton()->accessibility_update_set_transform(accessibility_scroll_element, scroll_xform);
			DisplayServer::get_singleton()->accessibility_update_set_bounds(accessibility_scroll_element, Rect2(0, 0, scroll_bar_h->get_max(), scroll_bar_v->get_max()));

			for (int i = 0; i < items.size(); i++) {
				const Item &item = items.write[i];

				if (item.accessibility_item_element.is_null()) {
					item.accessibility_item_element = DisplayServer::get_singleton()->accessibility_create_sub_element(accessibility_scroll_element, DisplayServer::AccessibilityRole::ROLE_LIST_BOX_OPTION);
					item.accessibility_item_dirty = true;
				}
				if (item.accessibility_item_dirty || i == hovered || i == prev_hovered) {
					DisplayServer::get_singleton()->accessibility_update_add_action(item.accessibility_item_element, DisplayServer::AccessibilityAction::ACTION_SCROLL_INTO_VIEW, callable_mp(this, &ExtendedItemList::_accessibility_action_scroll_into_view).bind(i));
					DisplayServer::get_singleton()->accessibility_update_add_action(item.accessibility_item_element, DisplayServer::AccessibilityAction::ACTION_FOCUS, callable_mp(this, &ExtendedItemList::_accessibility_action_focus).bind(i));
					DisplayServer::get_singleton()->accessibility_update_add_action(item.accessibility_item_element, DisplayServer::AccessibilityAction::ACTION_BLUR, callable_mp(this, &ExtendedItemList::_accessibility_action_blur).bind(i));

					DisplayServer::get_singleton()->accessibility_update_set_list_item_index(item.accessibility_item_element, i);
					DisplayServer::get_singleton()->accessibility_update_set_list_item_level(item.accessibility_item_element, 0);
					DisplayServer::get_singleton()->accessibility_update_set_list_item_selected(item.accessibility_item_element, item.selected);
					DisplayServer::get_singleton()->accessibility_update_set_name(item.accessibility_item_element, item.xl_text);
					DisplayServer::get_singleton()->accessibility_update_set_flag(item.accessibility_item_element, DisplayServer::AccessibilityFlags::FLAG_DISABLED, item.disabled);
					if (item.tooltip_enabled) {
						DisplayServer::get_singleton()->accessibility_update_set_tooltip(item.accessibility_item_element, item.tooltip);
					}

					Rect2 r = get_item_rect(i);
					DisplayServer::get_singleton()->accessibility_update_set_bounds(item.accessibility_item_element, Rect2(r.position, r.size));

					item.accessibility_item_dirty = false;
				}
			}
			prev_hovered = -1;

		} break;

		case NOTIFICATION_RESIZED: {
			shape_changed = true;
			queue_redraw();
		} break;

		case NOTIFICATION_LAYOUT_DIRECTION_CHANGED:
		case NOTIFICATION_THEME_CHANGED: {
			for (int i = 0; i < items.size(); i++) {
				_shape_text(i);
			}
			shape_changed = true;
			queue_accessibility_update();
			queue_redraw();
		} break;
		case NOTIFICATION_TRANSLATION_CHANGED: {
			for (int i = 0; i < items.size(); i++) {
				items.write[i].xl_text = _atr(i, items[i].text);
				_shape_text(i);
			}
			shape_changed = true;
			queue_accessibility_update();
			queue_redraw();
		} break;

		case NOTIFICATION_DRAW: {
			force_update_list_size();

			Size2 scroll_bar_h_min = scroll_bar_h->is_visible() ? scroll_bar_h->get_combined_minimum_size() : Size2();
			Size2 scroll_bar_v_min = scroll_bar_v->is_visible() ? scroll_bar_v->get_combined_minimum_size() : Size2();

			int left_margin = is_layout_rtl() ? theme_cache.panel_style->get_margin(SIDE_RIGHT) : theme_cache.panel_style->get_margin(SIDE_LEFT);
			int right_margin = is_layout_rtl() ? theme_cache.panel_style->get_margin(SIDE_LEFT) : theme_cache.panel_style->get_margin(SIDE_RIGHT);

			scroll_bar_v->set_anchor_and_offset(SIDE_LEFT, ANCHOR_END, -scroll_bar_v_min.width - right_margin);
			scroll_bar_v->set_anchor_and_offset(SIDE_RIGHT, ANCHOR_END, -right_margin);
			scroll_bar_v->set_anchor_and_offset(SIDE_TOP, ANCHOR_BEGIN, theme_cache.panel_style->get_margin(SIDE_TOP));
			scroll_bar_v->set_anchor_and_offset(SIDE_BOTTOM, ANCHOR_END, -scroll_bar_h_min.height - theme_cache.panel_style->get_margin(SIDE_BOTTOM));

			scroll_bar_h->set_anchor_and_offset(SIDE_LEFT, ANCHOR_BEGIN, left_margin);
			scroll_bar_h->set_anchor_and_offset(SIDE_RIGHT, ANCHOR_END, -right_margin - scroll_bar_v_min.width);
			scroll_bar_h->set_anchor_and_offset(SIDE_TOP, ANCHOR_END, -scroll_bar_h_min.height - theme_cache.panel_style->get_margin(SIDE_BOTTOM));
			scroll_bar_h->set_anchor_and_offset(SIDE_BOTTOM, ANCHOR_END, -theme_cache.panel_style->get_margin(SIDE_BOTTOM));

			Size2 size = get_size();
			int width = size.width - theme_cache.panel_style->get_minimum_size().width;
			if (scroll_bar_v->is_visible()) {
				width -= scroll_bar_v_min.width;
			}

			draw_style_box(theme_cache.panel_style, Rect2(Point2(), size));

			Ref<StyleBox> sbsel;
			Ref<StyleBox> cursor;

			if (has_focus()) {
				sbsel = theme_cache.selected_focus_style;
				cursor = theme_cache.cursor_focus_style;
			} else {
				sbsel = theme_cache.selected_style;
				cursor = theme_cache.cursor_style;
			}
			bool rtl = is_layout_rtl();

			// Ensure_selected_visible needs to be checked before we draw the list.
			if (ensure_selected_visible && current >= 0 && current < items.size()) {
				Rect2 r = items[current].rect_cache;
				int from_v = scroll_bar_v->get_value();
				int to_v = from_v + scroll_bar_v->get_page();

				if (r.position.y < from_v) {
					scroll_bar_v->set_value(r.position.y);
				} else if (r.position.y + r.size.y > to_v) {
					scroll_bar_v->set_value(r.position.y + r.size.y - (to_v - from_v));
				}
				int from_h = scroll_bar_h->get_value();
				int to_h = from_h + scroll_bar_h->get_page();

				if (r.position.x < from_h) {
					scroll_bar_h->set_value(r.position.x);
				} else if (r.position.x + r.size.x > to_h) {
					scroll_bar_h->set_value(r.position.x + r.size.x - (to_h - from_h));
				}
			}

			ensure_selected_visible = false;

			Vector2 base_ofs = theme_cache.panel_style->get_offset();
			base_ofs.y -= int(scroll_bar_v->get_value());
			if (rtl) {
				base_ofs.x += int(scroll_bar_h->get_value());
			} else {
				base_ofs.x -= int(scroll_bar_h->get_value());
			}

			// Define a visible frame to check against and optimize drawing.
			if (!wraparound_items) {
				size.width += (scroll_bar_h->get_max() - scroll_bar_h->get_page());
			}
			const Rect2 clip(-base_ofs, size);

			// Do a binary search to find the first separator that is below clip_position.y.
			int64_t first_visible_separator = separators.span().bisect(clip.position.y, true);

			// If not in thumbnails mode, draw visible separators.
			if (icon_mode != ICON_MODE_TOP) {
				for (int i = first_visible_separator; i < separators.size(); i++) {
					if (separators[i] > clip.position.y + clip.size.y) {
						break; // done
					}

					const int y = base_ofs.y + separators[i];
					if (rtl && scroll_bar_v->is_visible()) {
						draw_line(Vector2(theme_cache.panel_style->get_margin(SIDE_LEFT) + scroll_bar_v_min.width, y), Vector2(width + theme_cache.panel_style->get_margin(SIDE_LEFT) + scroll_bar_v_min.width, y), theme_cache.guide_color);
					} else {
						draw_line(Vector2(theme_cache.panel_style->get_margin(SIDE_LEFT), y), Vector2(width + theme_cache.panel_style->get_margin(SIDE_LEFT), y), theme_cache.guide_color);
					}
				}
			}

			// Do a binary search to find the first item whose rect reaches below clip.position.y.
			int first_item_visible;
			{
				int lo = 0;
				int hi = items.size();
				while (lo < hi) {
					const int mid = (lo + hi) / 2;
					const Rect2 &rcache = items[mid].rect_cache;
					if (rcache.position.y + rcache.size.y < clip.position.y) {
						lo = mid + 1;
					} else {
						hi = mid;
					}
				}

				// We might end up with an item in columns 2, 3, etc, but we need the one from the first column.
				// We can also end up in a state where lo reached hi, and so no items can be rendered; we skip that.
				while (lo < hi && lo > 0 && items[lo].column > 0) {
					lo -= 1;
				}

				first_item_visible = lo;
			}

			Rect2 cursor_rcache; // Place to save the position of the cursor and draw it after everything else.

			// Draw items using the appropriate canvas (clipped if margins are set).
			RID ci;

			int item_margin_left = theme_cache.item_margin_left;
			int item_margin_top = theme_cache.item_margin_top;
			int item_margin_right = theme_cache.item_margin_right;
			int item_margin_bottom = theme_cache.item_margin_bottom;

			if (item_margin_left || item_margin_top || item_margin_right || item_margin_bottom) {
				Vector2 offset = Vector2(item_margin_left, item_margin_top);
				Rect2 clip_rect = Rect2(offset,
					size - Size2(item_margin_left + item_margin_right, item_margin_top + item_margin_bottom));

				RenderingServer::get_singleton()->canvas_item_clear(clip_canvas_item);
				RenderingServer::get_singleton()->canvas_item_set_custom_rect(clip_canvas_item, !is_visibility_clip_disabled(), clip_rect);
				RenderingServer::get_singleton()->canvas_item_set_clip(clip_canvas_item, true);
				
				base_ofs += offset;
				ci = clip_canvas_item;
			} else {
				ci = get_canvas_item();
			}

			// Draw visible items.
			for (int i = first_item_visible; i < items.size(); i++) {
				Rect2 rcache = items[i].rect_cache;

				if (rcache.position.y > clip.position.y + clip.size.y) {
					break; // done
				}

				if (!clip.intersects(rcache)) {
					continue;
				}

				if (current_columns == 1) {
					rcache.size.width = width - rcache.position.x;
				}

				bool should_draw_selected_bg = items[i].selected && hovered != i;
				bool should_draw_hovered_selected_bg = items[i].selected && hovered == i;
				bool should_draw_hovered_bg = hovered == i && !items[i].selected;
				bool should_draw_custom_bg = items[i].custom_bg.a > 0.001;

				if (should_draw_selected_bg || should_draw_hovered_selected_bg || should_draw_hovered_bg || should_draw_custom_bg) {
					Rect2 r = rcache;
					r.position += base_ofs;

					if (rtl) {
						r.position.x = size.width - r.position.x - r.size.x + theme_cache.panel_style->get_margin(SIDE_LEFT) - theme_cache.panel_style->get_margin(SIDE_RIGHT);
					}

					if (should_draw_selected_bg) {
						//draw_style_box(sbsel, r);
						sbsel->draw(ci, r);
					}
					if (should_draw_hovered_selected_bg) {
						if (has_focus()) {
							//draw_style_box(theme_cache.hovered_selected_focus_style, r);
							theme_cache.hovered_selected_focus_style->draw(ci, r);
						} else {
							//draw_style_box(theme_cache.hovered_selected_style, r);
							theme_cache.hovered_selected_style->draw(ci, r);
						}
					}
					if (should_draw_hovered_bg) {
						//draw_style_box(theme_cache.hovered_style, r);
						theme_cache.hovered_style->draw(ci, r);
					}
					if (should_draw_custom_bg) {
						//draw_rect(r, items[i].custom_bg);
						RenderingServer::get_singleton()->canvas_item_add_rect(ci, r, items[i].custom_bg, false);
					}
				}

				Vector2 text_ofs;
				Size2 icon_size;
				if (items[i].icon.is_valid()) {
					if (fixed_icon_size.x > 0 && fixed_icon_size.y > 0) {
						icon_size = fixed_icon_size * icon_scale;
					} else {
						icon_size = items[i].get_icon_size() * icon_scale;
					}

					Point2 pos = items[i].rect_cache.position + base_ofs;

					if (icon_mode == ICON_MODE_TOP) {
						pos.y += MAX(theme_cache.v_separation, 0) / 2;
					} else {
						pos.x += MAX(theme_cache.h_separation, 0) / 2;
					}

					if (icon_mode == ICON_MODE_TOP) {
						pos.x += Math::floor((items[i].rect_cache.size.width - icon_size.width) / 2);
						pos.y += theme_cache.icon_margin;
						text_ofs.y = icon_size.height + theme_cache.icon_margin * 2;
					} else {
						pos.y += Math::floor((items[i].rect_cache.size.height - icon_size.height) / 2);
						text_ofs.x = icon_size.width + theme_cache.icon_margin;
					}

					Rect2 draw_rect = Rect2(pos, icon_size);

					if (fixed_icon_size.x > 0 && fixed_icon_size.y > 0) {
						Rect2 adj = _adjust_to_max_size(items[i].get_icon_size() * icon_scale, icon_size);
						draw_rect.position += adj.position;
						draw_rect.size = adj.size;
					}

					Color icon_modulate = items[i].icon_modulate;
					if (items[i].disabled) {
						icon_modulate.a *= 0.5;
					}

					// If the icon is transposed, we have to switch the size so that it is drawn correctly
					if (items[i].icon_transposed) {
						Size2 size_tmp = draw_rect.size;
						draw_rect.size.x = size_tmp.y;
						draw_rect.size.y = size_tmp.x;
					}

					Rect2 region = (items[i].icon_region.size.x == 0 || items[i].icon_region.size.y == 0) ? Rect2(Vector2(), items[i].icon->get_size()) : Rect2(items[i].icon_region);

					if (rtl) {
						draw_rect.position.x = size.width - draw_rect.position.x - draw_rect.size.x;
					}
					//draw_texture_rect_region(items[i].icon, draw_rect, region, icon_modulate, items[i].icon_transposed);
					items[i].icon->draw_rect_region(ci, draw_rect, region, icon_modulate, items[i].icon_transposed, false);
					
					items.write[i].icon_draw_rect = draw_rect;
				}

				if (items[i].tag_icon.is_valid()) {
					Size2 tag_icon_size;
					if (fixed_tag_icon_size.x > 0 && fixed_tag_icon_size.y > 0) {
						tag_icon_size = fixed_tag_icon_size;
					} else {
						tag_icon_size = items[i].tag_icon->get_size();
					}

					Point2 draw_pos = items[i].rect_cache.position + base_ofs + tag_icon_offset;
					draw_pos.x += MAX(theme_cache.h_separation, 0) / 2;
					draw_pos.y += MAX(theme_cache.v_separation, 0) / 2;
					if (rtl) {
						draw_pos.x = size.width - draw_pos.x - tag_icon_size.x;
					}

					//draw_texture_rect(items[i].tag_icon, Rect2(draw_pos, tag_icon_size));
					items[i].tag_icon->draw_rect(ci, Rect2(draw_pos, tag_icon_size));
				}

				bool has_text = !items[i].text.is_empty();
				bool has_sub_text = !items[i].sub_text.is_empty();

				if (has_text || has_sub_text) {
					Color txt_modulate;
					if (items[i].selected && hovered == i) {
						txt_modulate = theme_cache.font_hovered_selected_color;
					} else if (items[i].selected) {
						txt_modulate = theme_cache.font_selected_color;
					} else if (hovered == i) {
						txt_modulate = theme_cache.font_hovered_color;
					} else if (items[i].custom_fg != Color()) {
						txt_modulate = items[i].custom_fg;
					} else {
						txt_modulate = theme_cache.font_color;
					}

					if (items[i].disabled) {
						txt_modulate.a *= 0.5;
					}

					Color sub_txt_modulate;
					if (items[i].selected && hovered == i) {
						sub_txt_modulate = theme_cache.sub_font_hovered_selected_color;
					} else if (items[i].selected) {
						sub_txt_modulate = theme_cache.sub_font_selected_color;
					} else if (hovered == i) {
						sub_txt_modulate = theme_cache.sub_font_hovered_color;
					} else {
						sub_txt_modulate = theme_cache.sub_font_color;
					}
					if (items[i].disabled) {
						sub_txt_modulate.a *= 0.5;
					}

					if (icon_mode == ICON_MODE_TOP && max_text_lines > 0) {
						text_ofs.y += MAX(theme_cache.v_separation, 0) / 2;
						text_ofs.x += MAX(theme_cache.h_separation, 0) / 2;

						float text_w = items[i].rect_cache.size.width - text_ofs.x * 2;
						if (wraparound_items && text_w + text_ofs.x > width) {
							text_w = width - text_ofs.x;
						}

						text_ofs += base_ofs;
						text_ofs += items[i].rect_cache.position;

						if (rtl) {
							text_ofs.x = size.width - text_ofs.x - text_w;
						}

						if (has_text) {
							items.write[i].text_buf->set_alignment(HORIZONTAL_ALIGNMENT_CENTER);
							items.write[i].text_buf->set_width(text_w);

							if (theme_cache.font_outline_size > 0 && theme_cache.font_outline_color.a > 0) {
								items[i].text_buf->draw_outline(ci, text_ofs, theme_cache.font_outline_size, theme_cache.font_outline_color);
							}

							items[i].text_buf->draw(ci, text_ofs, txt_modulate);

							if (has_sub_text) {
								text_ofs.y += items[i].text_buf->get_size().y + theme_cache.sub_text_separation;

								items.write[i].sub_text_buf->set_alignment(HORIZONTAL_ALIGNMENT_CENTER);
								items.write[i].sub_text_buf->set_width(text_w);

								if (theme_cache.sub_font_outline_size > 0 && theme_cache.sub_font_outline_color.a > 0) {
									items[i].sub_text_buf->draw_outline(ci, text_ofs, theme_cache.sub_font_outline_size, theme_cache.sub_font_outline_color);
								}

								items[i].sub_text_buf->draw(ci, text_ofs, sub_txt_modulate);
							}
						} else if (has_sub_text) {
							text_ofs.y += theme_cache.sub_text_separation;

							items.write[i].sub_text_buf->set_alignment(HORIZONTAL_ALIGNMENT_CENTER);
							items.write[i].sub_text_buf->set_width(text_w);

							if (theme_cache.sub_font_outline_size > 0 && theme_cache.sub_font_outline_color.a > 0) {
								items[i].sub_text_buf->draw_outline(ci, text_ofs, theme_cache.sub_font_outline_size, theme_cache.sub_font_outline_color);
							}

							items[i].sub_text_buf->draw(ci, text_ofs, sub_txt_modulate);
						}

					} else {
						text_ofs.y += (items[i].rect_cache.size.height - (has_text ? items[i].text_buf->get_size().y : 0) -
											  (has_sub_text ? items[i].sub_text_buf->get_size().y + theme_cache.sub_text_separation : 0)) / 2;
						text_ofs.x += MAX(theme_cache.h_separation, 0) / 2;

						real_t text_width_ofs = text_ofs.x;

						text_ofs += base_ofs;
						text_ofs += items[i].rect_cache.position;

						float text_w = items[i].rect_cache.size.width - text_width_ofs;
						if (wraparound_items && items[i].rect_cache.size.width > width) {
							text_w -= items[i].rect_cache.size.width - width;
						}

						if (has_text) {
							items.write[i].text_buf->set_width(text_w);

							if (rtl) {
								text_ofs.x = size.width - items[i].rect_cache.size.width + icon_size.x - text_ofs.x + MAX(theme_cache.h_separation, 0);
								if (wraparound_items) {
									text_ofs.x += MAX(items[i].rect_cache.size.width - width, 0);
								}
								items.write[i].text_buf->set_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
							} else {
								items.write[i].text_buf->set_alignment(HORIZONTAL_ALIGNMENT_LEFT);
							}

							if (theme_cache.font_outline_size > 0 && theme_cache.font_outline_color.a > 0) {
								items[i].text_buf->draw_outline(ci, text_ofs, theme_cache.font_outline_size, theme_cache.font_outline_color);
							}

							if (fixed_column_width > 0) {
								if (items[i].rect_cache.size.width - icon_size.x - MAX(theme_cache.h_separation, 0) > 0) {
									items[i].text_buf->draw(ci, text_ofs, txt_modulate);
								}
							} else {
								if (wraparound_items) {
									if (width - icon_size.x - MAX(theme_cache.h_separation, 0) - int(scroll_bar_h->get_value()) > 0) {
										items[i].text_buf->draw(ci, text_ofs, txt_modulate);
									}
								} else {
									items[i].text_buf->draw(ci, text_ofs, txt_modulate);
								}
							}

							if (has_sub_text) {
								text_ofs.y += items[i].text_buf->get_size().y + theme_cache.sub_text_separation;

								items.write[i].sub_text_buf->set_width(text_w);
								items.write[i].sub_text_buf->set_alignment(rtl ? HORIZONTAL_ALIGNMENT_RIGHT : HORIZONTAL_ALIGNMENT_LEFT);

								if (theme_cache.sub_font_outline_size > 0 && theme_cache.sub_font_outline_color.a > 0) {
									items[i].sub_text_buf->draw_outline(ci, text_ofs, theme_cache.sub_font_outline_size, theme_cache.sub_font_outline_color);
								}

								items[i].sub_text_buf->draw(ci, text_ofs, sub_txt_modulate);
							}
						}
					}
				}

				if (i == current && (select_mode == SELECT_MULTI || select_mode == SELECT_TOGGLE)) {
					cursor_rcache = rcache;
				}
			}

			if (cursor_rcache.size != Size2()) { // Draw cursor last, so border isn't cut off.
				cursor_rcache.position += base_ofs;

				if (rtl) {
					cursor_rcache.position.x = size.width - cursor_rcache.position.x - cursor_rcache.size.x;
				}

				draw_style_box(cursor, cursor_rcache);
			}

			if (has_focus()) {
				RenderingServer::get_singleton()->canvas_item_add_clip_ignore(get_canvas_item(), true);
				size.x -= (scroll_bar_h->get_max() - scroll_bar_h->get_page());
				draw_style_box(theme_cache.focus_style, Rect2(Point2(), size));
				RenderingServer::get_singleton()->canvas_item_add_clip_ignore(get_canvas_item(), false);
			}
		} break;
	}
}

void ExtendedItemList::force_update_list_size() {
	if (!shape_changed) {
		return;
	}

	int scroll_bar_v_minwidth = scroll_bar_v->get_minimum_size().x;
	Size2 size = get_size();
	float max_column_width = 0.0;

	//1- compute item minimum sizes
	for (int i = 0; i < items.size(); i++) {
		Size2 minsize;
		if (items[i].icon.is_valid()) {
			if (fixed_icon_size.x > 0 && fixed_icon_size.y > 0) {
				minsize = fixed_icon_size * icon_scale;
			} else {
				minsize = items[i].get_icon_size() * icon_scale;
			}

			if (!items[i].text.is_empty()) {
				if (icon_mode == ICON_MODE_TOP) {
					minsize.y += theme_cache.icon_margin;
				} else {
					minsize.x += theme_cache.icon_margin;
				}
			}
		}

		bool has_text = !items[i].text.is_empty();
		bool has_sub_text = !items[i].sub_text.is_empty();

		if (has_text || has_sub_text) {
			int max_width = -1;
			if (fixed_column_width) {
				max_width = fixed_column_width;
			}

			Size2 sub_s;
			if (has_sub_text) {
				items.write[i].sub_text_buf->set_width(max_width);

				sub_s = items[i].sub_text_buf->get_size();
				sub_s.y += theme_cache.sub_text_separation;
			}

			if (has_text) {
				items.write[i].text_buf->set_width(max_width);
				Size2 s = items[i].text_buf->get_size();

				if (icon_mode == ICON_MODE_TOP) {
					minsize.x = MAX(minsize.x, MAX(s.width, sub_s.width));
					if (max_text_lines > 0) {
						minsize.y += s.height + sub_s.height + theme_cache.line_separation * max_text_lines;
					} else {
						minsize.y += s.height + sub_s.height;
					}
				} else {
					minsize.y = MAX(minsize.y, s.height + sub_s.height);
					minsize.x += MAX(s.width, sub_s.width);
				}
			} else if (has_sub_text) {
				if (icon_mode == ICON_MODE_TOP) {
					minsize.x = MAX(minsize.x, sub_s.width);
					minsize.y += sub_s.height;
				} else {
					minsize.y = MAX(minsize.y, sub_s.height);
					minsize.x += sub_s.width;
				}
			}
		}
		
		if (fixed_column_width > 0) {
			minsize.x = fixed_column_width;
		}
		max_column_width = MAX(max_column_width, minsize.x);

		// Elements need to adapt to the selected size.
		minsize.y += MAX(theme_cache.v_separation, 0);
		minsize.x += MAX(theme_cache.h_separation, 0);

		items.write[i].rect_cache.size = minsize;
		items.write[i].min_rect_cache.size = minsize;

		items.write[i].accessibility_item_dirty = true;
	}

	int fit_size = size.x - theme_cache.panel_style->get_minimum_size().width;
	if (!wraparound_items) {
		fit_size += (scroll_bar_h->get_max() - scroll_bar_h->get_page());
	}

	//2-attempt best fit
	current_columns = 0x7FFFFFFF;
	if (max_columns > 0) {
		current_columns = max_columns;
	}

	// Repeat until all items fit.
	while (true) {
		bool all_fit = true;
		Vector2 ofs;
		int col = 0;
		int max_w = 0;
		int max_h = 0;

		separators.clear();

		for (int i = 0; i < items.size(); i++) {
			if (current_columns > 1 && items[i].rect_cache.size.width + ofs.x > fit_size && !auto_width && wraparound_items) {
				// Went past.
				current_columns = MAX(col, 1);
				all_fit = false;
				break;
			}

			if (same_column_width) {
				items.write[i].rect_cache.size.x = max_column_width + MAX(theme_cache.h_separation, 0);
			}
			items.write[i].rect_cache.position = ofs;

			max_h = MAX(max_h, items[i].rect_cache.size.y);
			ofs.x += items[i].rect_cache.size.x;
			max_w = MAX(max_w, ofs.x);

			items.write[i].column = col;
			col++;
			if (col == current_columns) {
				if (i < items.size() - 1) {
					separators.push_back(ofs.y + max_h);
				}

				for (int j = i; j >= 0 && col > 0; j--, col--) {
					items.write[j].rect_cache.size.y = max_h;
				}

				ofs.x = 0;
				ofs.y += max_h;
				col = 0;
				max_h = 0;
			}
		}

		float scroll_bar_v_page = MAX(0, size.height - theme_cache.panel_style->get_minimum_size().height - theme_cache.item_margin_top - theme_cache.item_margin_bottom);
		float scroll_bar_v_max = MAX(scroll_bar_v_page, ofs.y + max_h);
		float scroll_bar_h_page = MAX(0, size.width - theme_cache.panel_style->get_minimum_size().width - theme_cache.item_margin_left - theme_cache.item_margin_right);
		float scroll_bar_h_max = 0;
		if (!wraparound_items) {
			scroll_bar_h_max = MAX(scroll_bar_h_page, max_w);
		}

		if (scroll_bar_v_page >= scroll_bar_v_max || is_layout_rtl()) {
			fit_size -= scroll_bar_v_minwidth;
		}

		if (all_fit) {
			for (int j = items.size() - 1; j >= 0 && col > 0; j--, col--) {
				items.write[j].rect_cache.size.y = max_h;
			}

			if (auto_height) {
				auto_height_value = ofs.y + max_h + theme_cache.panel_style->get_minimum_size().height;
			}
			if (auto_width) {
				auto_width_value = max_w + theme_cache.panel_style->get_minimum_size().width;
			}
			scroll_bar_v->set_max(scroll_bar_v_max);
			scroll_bar_v->set_page(scroll_bar_v_page);
			if (scroll_bar_v_max <= scroll_bar_v_page) {
				scroll_bar_v->set_value(0);
				scroll_bar_v->hide();
			} else {
				auto_width_value += scroll_bar_v_minwidth;
				scroll_bar_v->show();

				if (do_autoscroll_to_bottom) {
					scroll_bar_v->set_value(scroll_bar_v_max);
				}
			}

			if (is_layout_rtl() && !wraparound_items) {
				scroll_bar_h->set_max(scroll_bar_h_page);
				scroll_bar_h->set_min(-(scroll_bar_h_max - scroll_bar_h_page));
			} else {
				scroll_bar_h->set_max(scroll_bar_h_max);
				scroll_bar_h->set_min(0);
			}
			scroll_bar_h->set_page(scroll_bar_h_page);
			if (scroll_bar_h_max <= scroll_bar_h_page) {
				scroll_bar_h->set_value(0);
				scroll_bar_h->hide();
			} else {
				auto_height_value += scroll_bar_h->get_minimum_size().y;
				scroll_bar_h->show();
			}
			break;
		}
	}

	update_minimum_size();
	shape_changed = false;
}

void ExtendedItemList::_scroll_changed(double) {
	queue_redraw();
}

void ExtendedItemList::_mouse_exited() {
	if (hovered > -1) {
		prev_hovered = hovered;
		hovered = -1;
		queue_accessibility_update();
		queue_redraw();
	}
}

void ExtendedItemList::_shift_range_select(int p_from, int p_to) {
	ERR_FAIL_INDEX(p_from, items.size());
	ERR_FAIL_INDEX(p_to, items.size());

	if (shift_anchor == -1) {
		shift_anchor = p_from;
	}

	for (int i = 0; i < items.size(); i++) {
		if (i >= MIN(shift_anchor, p_to) && i <= MAX(shift_anchor, p_to)) {
			if (!is_selected(i)) {
				select(i, false);
				emit_signal(SNAME("multi_selected"), i, true);
			}
		} else if (is_selected(i)) {
			deselect(i);
			emit_signal(SNAME("multi_selected"), i, false);
		}
	}

	current = p_to;
	queue_redraw();
	ensure_current_is_visible();
}

String ExtendedItemList::_atr(int p_idx, const String &p_text) const {
	ERR_FAIL_INDEX_V(p_idx, items.size(), atr(p_text));
	switch (items[p_idx].auto_translate_mode) {
		case AUTO_TRANSLATE_MODE_INHERIT: {
			return atr(p_text);
		} break;
		case AUTO_TRANSLATE_MODE_ALWAYS: {
			return tr(p_text);
		} break;
		case AUTO_TRANSLATE_MODE_DISABLED: {
			return p_text;
		} break;
	}

	ERR_FAIL_V_MSG(atr(p_text), "Unexpected auto translate mode: " + itos(items[p_idx].auto_translate_mode));
}

int ExtendedItemList::get_item_at_position(const Point2 &p_pos, bool p_exact) const {
	Vector2 pos = p_pos;
	pos -= theme_cache.panel_style->get_offset();
	pos.y += scroll_bar_v->get_value();
	pos.x += scroll_bar_h->get_value();

	if (is_layout_rtl()) {
		pos.x = get_size().width - pos.x - scroll_bar_h->get_value() - theme_cache.panel_style->get_margin(SIDE_LEFT) - theme_cache.panel_style->get_margin(SIDE_RIGHT);
	}

	int closest = -1;
	int closest_dist = 0x7FFFFFFF;

	for (int i = 0; i < items.size(); i++) {
		Rect2 rc = items[i].rect_cache;

		if (i % current_columns == current_columns - 1) { // Make sure you can still select the last item when clicking past the column.
			if (is_layout_rtl()) {
				rc.size.width = get_size().width - scroll_bar_h->get_value() + rc.position.x;
			} else {
				rc.size.width = get_size().width + scroll_bar_h->get_value() - rc.position.x;
			}
		}

		if (rc.size.x < 0) {
			continue; // Skip negative item sizes, because they are off screen.
		}

		if (rc.has_point(pos)) {
			closest = i;
			break;
		}

		float dist = rc.distance_to(pos);
		if (!p_exact && dist < closest_dist) {
			closest = i;
			closest_dist = dist;
		}
	}

	return closest;
}

bool ExtendedItemList::is_pos_at_end_of_items(const Point2 &p_pos) const {
	if (items.is_empty()) {
		return true;
	}

	Vector2 pos = p_pos;
	pos -= theme_cache.panel_style->get_offset();
	pos.y += scroll_bar_v->get_value();

	if (is_layout_rtl()) {
		pos.x = get_size().width - pos.x;
	}

	Rect2 endrect = items[items.size() - 1].rect_cache;
	return (pos.y > endrect.position.y + endrect.size.y);
}

String ExtendedItemList::get_tooltip(const Point2 &p_pos) const {
	if (!tooltip_enabled) {
		return "";
	}

	int closest = get_item_at_position(p_pos, true);

	if (closest != -1) {
		if (!items[closest].tooltip_enabled) {
			return "";
		}
		if (is_tooltip_icon_only() && !items[closest].icon_draw_rect.has_point(p_pos)) {
			return "";
		}
		if (!items[closest].tooltip.is_empty()) {
			return items[closest].tooltip;
		}
		if (!items[closest].text.is_empty()) {
			return items[closest].text;
		}
	}

	return Control::get_tooltip(p_pos);
}

void ExtendedItemList::sort_items_by_text() {
	items.sort();
	queue_accessibility_update();
	queue_redraw();
	shape_changed = true;

	if (select_mode == SELECT_SINGLE) {
		for (int i = 0; i < items.size(); i++) {
			if (items[i].selected) {
				select(i);
				return;
			}
		}
	}
}

int ExtendedItemList::find_metadata(const Variant &p_metadata) const {
	for (int i = 0; i < items.size(); i++) {
		if (items[i].metadata == p_metadata) {
			return i;
		}
	}

	return -1;
}

void ExtendedItemList::set_allow_rmb_select(bool p_allow) {
	allow_rmb_select = p_allow;
}

bool ExtendedItemList::get_allow_rmb_select() const {
	return allow_rmb_select;
}

void ExtendedItemList::set_allow_reselect(bool p_allow) {
	allow_reselect = p_allow;
}

bool ExtendedItemList::get_allow_reselect() const {
	return allow_reselect;
}

void ExtendedItemList::set_allow_search(bool p_allow) {
	allow_search = p_allow;
}

bool ExtendedItemList::get_allow_search() const {
	return allow_search;
}

void ExtendedItemList::set_icon_scale(real_t p_scale) {
	ERR_FAIL_COND(!Math::is_finite(p_scale));

	if (icon_scale == p_scale) {
		return;
	}

	icon_scale = p_scale;
	queue_redraw();
	shape_changed = true;
}

real_t ExtendedItemList::get_icon_scale() const {
	return icon_scale;
}

Vector<int> ExtendedItemList::get_selected_items() {
	Vector<int> selected;
	for (int i = 0; i < items.size(); i++) {
		if (items[i].selected) {
			selected.push_back(i);
			if (select_mode == SELECT_SINGLE) {
				break;
			}
		}
	}
	return selected;
}

bool ExtendedItemList::is_anything_selected() {
	for (int i = 0; i < items.size(); i++) {
		if (items[i].selected) {
			return true;
		}
	}

	return false;
}

Size2 ExtendedItemList::get_minimum_size() const {
	Size2 min_size;
	if (auto_width) {
		min_size.x = auto_width_value;
	}

	if (auto_height) {
		min_size.y = auto_height_value;
	}
	return min_size;
}

void ExtendedItemList::set_autoscroll_to_bottom(const bool p_enable) {
	do_autoscroll_to_bottom = p_enable;
}

void ExtendedItemList::set_auto_width(bool p_enable) {
	if (auto_width == p_enable) {
		return;
	}

	auto_width = p_enable;
	shape_changed = true;
	queue_accessibility_update();
	queue_redraw();
}

bool ExtendedItemList::has_auto_width() const {
	return auto_width;
}

void ExtendedItemList::set_auto_height(bool p_enable) {
	if (auto_height == p_enable) {
		return;
	}

	auto_height = p_enable;
	shape_changed = true;
	queue_accessibility_update();
	queue_redraw();
}

bool ExtendedItemList::has_auto_height() const {
	return auto_height;
}

void ExtendedItemList::set_text_overrun_behavior(TextServer::OverrunBehavior p_behavior) {
	if (text_overrun_behavior != p_behavior) {
		text_overrun_behavior = p_behavior;
		for (int i = 0; i < items.size(); i++) {
			items.write[i].text_buf->set_text_overrun_behavior(p_behavior);
		}
		shape_changed = true;
		queue_redraw();
	}
}

TextServer::OverrunBehavior ExtendedItemList::get_text_overrun_behavior() const {
	return text_overrun_behavior;
}

void ExtendedItemList::set_wraparound_items(bool p_enable) {
	if (wraparound_items == p_enable) {
		return;
	}

	wraparound_items = p_enable;
	shape_changed = true;
	queue_redraw();
}

bool ExtendedItemList::has_wraparound_items() const {
	return wraparound_items;
}

void ExtendedItemList::set_tooltip_icon_only(bool p_enable) {
	tooltip_icon_only = p_enable;
}

bool ExtendedItemList::is_tooltip_icon_only() const {
	return tooltip_icon_only;
}

void ExtendedItemList::set_tooltip_enabled(bool p_enabled) {
	tooltip_enabled = p_enabled;
}

bool ExtendedItemList::is_tooltip_enabled() const {
	return tooltip_enabled;
}

bool ExtendedItemList::_set(const StringName &p_name, const Variant &p_value) {
	if (property_helper.property_set_value(p_name, p_value)) {
		return true;
	}

#ifndef DISABLE_DEPRECATED
	// Compatibility.
	if (p_name == "items") {
		Array arr = p_value;
		ERR_FAIL_COND_V(arr.size() % 3, false);
		clear();

		for (int i = 0; i < arr.size(); i += 3) {
			String text = arr[i + 0];
			Ref<Texture2D> icon = arr[i + 1];
			bool disabled = arr[i + 2];

			int idx = get_item_count();
			add_item(text, icon);
			set_item_disabled(idx, disabled);
		}
	}
#endif
	return false;
}

void ExtendedItemList::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_item", "text", "icon", "sub_text", "tag_icon", "selectable"), &ExtendedItemList::add_item, DEFVAL(Variant()), DEFVAL(String()), DEFVAL(Variant()), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("add_icon_item", "icon", "selectable"), &ExtendedItemList::add_icon_item, DEFVAL(true));

	ClassDB::bind_method(D_METHOD("set_item_text", "idx", "text"), &ExtendedItemList::set_item_text);
	ClassDB::bind_method(D_METHOD("get_item_text", "idx"), &ExtendedItemList::get_item_text);

	ClassDB::bind_method(D_METHOD("set_item_sub_text", "idx", "sub_text"), &ExtendedItemList::set_item_sub_text);
	ClassDB::bind_method(D_METHOD("get_item_sub_text", "idx"), &ExtendedItemList::get_item_sub_text);

	ClassDB::bind_method(D_METHOD("set_item_icon", "idx", "icon"), &ExtendedItemList::set_item_icon);
	ClassDB::bind_method(D_METHOD("get_item_icon", "idx"), &ExtendedItemList::get_item_icon);

	ClassDB::bind_method(D_METHOD("set_item_tag_icon", "idx", "icon"), &ExtendedItemList::set_item_tag_icon);
	ClassDB::bind_method(D_METHOD("get_item_tag_icon", "idx"), &ExtendedItemList::get_item_tag_icon);

	ClassDB::bind_method(D_METHOD("set_item_text_direction", "idx", "direction"), &ExtendedItemList::set_item_text_direction);
	ClassDB::bind_method(D_METHOD("get_item_text_direction", "idx"), &ExtendedItemList::get_item_text_direction);

	ClassDB::bind_method(D_METHOD("set_item_language", "idx", "language"), &ExtendedItemList::set_item_language);
	ClassDB::bind_method(D_METHOD("get_item_language", "idx"), &ExtendedItemList::get_item_language);

	ClassDB::bind_method(D_METHOD("set_item_auto_translate_mode", "idx", "mode"), &ExtendedItemList::set_item_auto_translate_mode);
	ClassDB::bind_method(D_METHOD("get_item_auto_translate_mode", "idx"), &ExtendedItemList::get_item_auto_translate_mode);

	ClassDB::bind_method(D_METHOD("set_item_icon_transposed", "idx", "transposed"), &ExtendedItemList::set_item_icon_transposed);
	ClassDB::bind_method(D_METHOD("is_item_icon_transposed", "idx"), &ExtendedItemList::is_item_icon_transposed);

	ClassDB::bind_method(D_METHOD("set_item_icon_region", "idx", "rect"), &ExtendedItemList::set_item_icon_region);
	ClassDB::bind_method(D_METHOD("get_item_icon_region", "idx"), &ExtendedItemList::get_item_icon_region);

	ClassDB::bind_method(D_METHOD("set_item_icon_modulate", "idx", "modulate"), &ExtendedItemList::set_item_icon_modulate);
	ClassDB::bind_method(D_METHOD("get_item_icon_modulate", "idx"), &ExtendedItemList::get_item_icon_modulate);

	ClassDB::bind_method(D_METHOD("set_item_selectable", "idx", "selectable"), &ExtendedItemList::set_item_selectable);
	ClassDB::bind_method(D_METHOD("is_item_selectable", "idx"), &ExtendedItemList::is_item_selectable);

	ClassDB::bind_method(D_METHOD("set_item_disabled", "idx", "disabled"), &ExtendedItemList::set_item_disabled);
	ClassDB::bind_method(D_METHOD("is_item_disabled", "idx"), &ExtendedItemList::is_item_disabled);

	ClassDB::bind_method(D_METHOD("set_item_metadata", "idx", "metadata"), &ExtendedItemList::set_item_metadata);
	ClassDB::bind_method(D_METHOD("get_item_metadata", "idx"), &ExtendedItemList::get_item_metadata);

	ClassDB::bind_method(D_METHOD("set_item_custom_bg_color", "idx", "custom_bg_color"), &ExtendedItemList::set_item_custom_bg_color);
	ClassDB::bind_method(D_METHOD("get_item_custom_bg_color", "idx"), &ExtendedItemList::get_item_custom_bg_color);

	ClassDB::bind_method(D_METHOD("set_item_custom_fg_color", "idx", "custom_fg_color"), &ExtendedItemList::set_item_custom_fg_color);
	ClassDB::bind_method(D_METHOD("get_item_custom_fg_color", "idx"), &ExtendedItemList::get_item_custom_fg_color);

	ClassDB::bind_method(D_METHOD("get_item_rect", "idx", "expand"), &ExtendedItemList::get_item_rect, DEFVAL(true));

	ClassDB::bind_method(D_METHOD("set_item_tooltip_enabled", "idx", "enable"), &ExtendedItemList::set_item_tooltip_enabled);
	ClassDB::bind_method(D_METHOD("is_item_tooltip_enabled", "idx"), &ExtendedItemList::is_item_tooltip_enabled);

	ClassDB::bind_method(D_METHOD("set_item_tooltip", "idx", "tooltip"), &ExtendedItemList::set_item_tooltip);
	ClassDB::bind_method(D_METHOD("get_item_tooltip", "idx"), &ExtendedItemList::get_item_tooltip);

	ClassDB::bind_method(D_METHOD("select", "idx", "single"), &ExtendedItemList::select, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("deselect", "idx"), &ExtendedItemList::deselect);
	ClassDB::bind_method(D_METHOD("deselect_all"), &ExtendedItemList::deselect_all);

	ClassDB::bind_method(D_METHOD("is_selected", "idx"), &ExtendedItemList::is_selected);
	ClassDB::bind_method(D_METHOD("get_selected_items"), &ExtendedItemList::get_selected_items);

	ClassDB::bind_method(D_METHOD("move_item", "from_idx", "to_idx"), &ExtendedItemList::move_item);

	ClassDB::bind_method(D_METHOD("set_item_count", "count"), &ExtendedItemList::set_item_count);
	ClassDB::bind_method(D_METHOD("get_item_count"), &ExtendedItemList::get_item_count);
	ClassDB::bind_method(D_METHOD("remove_item", "idx"), &ExtendedItemList::remove_item);

	ClassDB::bind_method(D_METHOD("clear"), &ExtendedItemList::clear);
	ClassDB::bind_method(D_METHOD("sort_items_by_text"), &ExtendedItemList::sort_items_by_text);

	ClassDB::bind_method(D_METHOD("set_fixed_column_width", "width"), &ExtendedItemList::set_fixed_column_width);
	ClassDB::bind_method(D_METHOD("get_fixed_column_width"), &ExtendedItemList::get_fixed_column_width);

	ClassDB::bind_method(D_METHOD("set_same_column_width", "enable"), &ExtendedItemList::set_same_column_width);
	ClassDB::bind_method(D_METHOD("is_same_column_width"), &ExtendedItemList::is_same_column_width);

	ClassDB::bind_method(D_METHOD("set_max_text_lines", "lines"), &ExtendedItemList::set_max_text_lines);
	ClassDB::bind_method(D_METHOD("get_max_text_lines"), &ExtendedItemList::get_max_text_lines);

	ClassDB::bind_method(D_METHOD("set_max_columns", "amount"), &ExtendedItemList::set_max_columns);
	ClassDB::bind_method(D_METHOD("get_max_columns"), &ExtendedItemList::get_max_columns);

	ClassDB::bind_method(D_METHOD("set_select_mode", "mode"), &ExtendedItemList::set_select_mode);
	ClassDB::bind_method(D_METHOD("get_select_mode"), &ExtendedItemList::get_select_mode);

	ClassDB::bind_method(D_METHOD("set_icon_mode", "mode"), &ExtendedItemList::set_icon_mode);
	ClassDB::bind_method(D_METHOD("get_icon_mode"), &ExtendedItemList::get_icon_mode);

	ClassDB::bind_method(D_METHOD("set_fixed_icon_size", "size"), &ExtendedItemList::set_fixed_icon_size);
	ClassDB::bind_method(D_METHOD("get_fixed_icon_size"), &ExtendedItemList::get_fixed_icon_size);

	ClassDB::bind_method(D_METHOD("set_fixed_tag_icon_size", "size"), &ExtendedItemList::set_fixed_tag_icon_size);
	ClassDB::bind_method(D_METHOD("get_fixed_tag_icon_size"), &ExtendedItemList::get_fixed_tag_icon_size);

	ClassDB::bind_method(D_METHOD("set_tag_icon_offset", "offset"), &ExtendedItemList::set_tag_icon_offset);
	ClassDB::bind_method(D_METHOD("get_tag_icon_offset"), &ExtendedItemList::get_tag_icon_offset);

	ClassDB::bind_method(D_METHOD("set_icon_scale", "scale"), &ExtendedItemList::set_icon_scale);
	ClassDB::bind_method(D_METHOD("get_icon_scale"), &ExtendedItemList::get_icon_scale);

	ClassDB::bind_method(D_METHOD("set_allow_rmb_select", "allow"), &ExtendedItemList::set_allow_rmb_select);
	ClassDB::bind_method(D_METHOD("get_allow_rmb_select"), &ExtendedItemList::get_allow_rmb_select);

	ClassDB::bind_method(D_METHOD("set_allow_reselect", "allow"), &ExtendedItemList::set_allow_reselect);
	ClassDB::bind_method(D_METHOD("get_allow_reselect"), &ExtendedItemList::get_allow_reselect);

	ClassDB::bind_method(D_METHOD("set_allow_search", "allow"), &ExtendedItemList::set_allow_search);
	ClassDB::bind_method(D_METHOD("get_allow_search"), &ExtendedItemList::get_allow_search);

	ClassDB::bind_method(D_METHOD("set_auto_width", "enable"), &ExtendedItemList::set_auto_width);
	ClassDB::bind_method(D_METHOD("has_auto_width"), &ExtendedItemList::has_auto_width);

	ClassDB::bind_method(D_METHOD("set_auto_height", "enable"), &ExtendedItemList::set_auto_height);
	ClassDB::bind_method(D_METHOD("has_auto_height"), &ExtendedItemList::has_auto_height);

	ClassDB::bind_method(D_METHOD("is_anything_selected"), &ExtendedItemList::is_anything_selected);

	ClassDB::bind_method(D_METHOD("get_item_at_position", "position", "exact"), &ExtendedItemList::get_item_at_position, DEFVAL(false));

	ClassDB::bind_method(D_METHOD("ensure_current_is_visible"), &ExtendedItemList::ensure_current_is_visible);

	ClassDB::bind_method(D_METHOD("get_v_scroll_bar"), &ExtendedItemList::get_v_scroll_bar);
	ClassDB::bind_method(D_METHOD("get_h_scroll_bar"), &ExtendedItemList::get_h_scroll_bar);

	ClassDB::bind_method(D_METHOD("set_text_overrun_behavior", "overrun_behavior"), &ExtendedItemList::set_text_overrun_behavior);
	ClassDB::bind_method(D_METHOD("get_text_overrun_behavior"), &ExtendedItemList::get_text_overrun_behavior);

	ClassDB::bind_method(D_METHOD("set_wraparound_items", "enable"), &ExtendedItemList::set_wraparound_items);
	ClassDB::bind_method(D_METHOD("has_wraparound_items"), &ExtendedItemList::has_wraparound_items);

	ClassDB::bind_method(D_METHOD("set_tooltip_icon_only", "enable"), &ExtendedItemList::set_tooltip_icon_only);
	ClassDB::bind_method(D_METHOD("is_tooltip_icon_only"), &ExtendedItemList::is_tooltip_icon_only);
	ClassDB::bind_method(D_METHOD("set_tooltip_enabled", "enable"), &ExtendedItemList::set_tooltip_enabled);
	ClassDB::bind_method(D_METHOD("is_tooltip_enabled"), &ExtendedItemList::is_tooltip_enabled);

	ClassDB::bind_method(D_METHOD("force_update_list_size"), &ExtendedItemList::force_update_list_size);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "select_mode", PROPERTY_HINT_ENUM, "Single,Multi,Toggle"), "set_select_mode", "get_select_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_reselect"), "set_allow_reselect", "get_allow_reselect");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_rmb_select"), "set_allow_rmb_select", "get_allow_rmb_select");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_search"), "set_allow_search", "get_allow_search");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_text_lines", PROPERTY_HINT_RANGE, "1,10,1,or_greater"), "set_max_text_lines", "get_max_text_lines");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_width"), "set_auto_width", "has_auto_width");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_height"), "set_auto_height", "has_auto_height");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "text_overrun_behavior", PROPERTY_HINT_ENUM, "Trim Nothing,Trim Characters,Trim Words,Ellipsis (6+ Characters),Word Ellipsis (6+ Characters),Ellipsis (Always),Word Ellipsis (Always)"), "set_text_overrun_behavior", "get_text_overrun_behavior");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "wraparound_items"), "set_wraparound_items", "has_wraparound_items");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "tooltip_icon_only"), "set_tooltip_icon_only", "is_tooltip_icon_only");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "tooltip_enabled"), "set_tooltip_enabled", "is_tooltip_enabled");
	ADD_ARRAY_COUNT("Items", "item_count", "set_item_count", "get_item_count", "item_");
	ADD_GROUP("Columns", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_columns", PROPERTY_HINT_RANGE, "0,10,1,or_greater"), "set_max_columns", "get_max_columns");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "same_column_width"), "set_same_column_width", "is_same_column_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fixed_column_width", PROPERTY_HINT_RANGE, "0,100,1,or_greater,suffix:px"), "set_fixed_column_width", "get_fixed_column_width");
	ADD_GROUP("Icon", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "icon_mode", PROPERTY_HINT_ENUM, "Top,Left"), "set_icon_mode", "get_icon_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "icon_scale"), "set_icon_scale", "get_icon_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "fixed_icon_size", PROPERTY_HINT_NONE, "suffix:px"), "set_fixed_icon_size", "get_fixed_icon_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "fixed_tag_icon_size", PROPERTY_HINT_NONE, "suffix:px"), "set_fixed_tag_icon_size", "get_fixed_tag_icon_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "tag_icon_offset", PROPERTY_HINT_NONE, "suffix:px"), "set_tag_icon_offset", "get_tag_icon_offset");

	BIND_ENUM_CONSTANT(ICON_MODE_TOP);
	BIND_ENUM_CONSTANT(ICON_MODE_LEFT);

	BIND_ENUM_CONSTANT(SELECT_SINGLE);
	BIND_ENUM_CONSTANT(SELECT_MULTI);
	BIND_ENUM_CONSTANT(SELECT_TOGGLE);

	ADD_SIGNAL(MethodInfo("item_selected", PropertyInfo(Variant::INT, "index")));
	ADD_SIGNAL(MethodInfo("empty_clicked", PropertyInfo(Variant::VECTOR2, "at_position"), PropertyInfo(Variant::INT, "mouse_button_index")));
	ADD_SIGNAL(MethodInfo("item_clicked", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::VECTOR2, "at_position"), PropertyInfo(Variant::INT, "mouse_button_index")));
	ADD_SIGNAL(MethodInfo("multi_selected", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::BOOL, "selected")));
	ADD_SIGNAL(MethodInfo("item_activated", PropertyInfo(Variant::INT, "index")));

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, h_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, v_separation);

	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, panel_style, "panel");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, focus_style, "focus");

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, ExtendedItemList, font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, ExtendedItemList, font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, font_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, font_hovered_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, font_hovered_selected_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, font_selected_color);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, font_outline_size, "outline_size");
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, font_outline_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, ExtendedItemList, sub_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, ExtendedItemList, sub_font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, sub_font_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, sub_font_hovered_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, sub_font_hovered_selected_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, sub_font_selected_color);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, sub_font_outline_size, "sub_outline_size");
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, sub_font_outline_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, sub_text_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, item_margin_left);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, item_margin_top);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, item_margin_right);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, item_margin_bottom);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, line_separation);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, ExtendedItemList, icon_margin);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, hovered_style, "hovered");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, hovered_selected_style, "hovered_selected");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, hovered_selected_focus_style, "hovered_selected_focus");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, selected_style, "selected");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, selected_focus_style, "selected_focus");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, cursor_style, "cursor_unfocused");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_STYLEBOX, ExtendedItemList, cursor_focus_style, "cursor");
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, ExtendedItemList, guide_color);

	Item defaults(true);

	base_property_helper.set_prefix("item_");
	base_property_helper.set_array_length_getter(&ExtendedItemList::get_item_count);
	base_property_helper.register_property(PropertyInfo(Variant::STRING, "text"), defaults.text, &ExtendedItemList::set_item_text, &ExtendedItemList::get_item_text);
	base_property_helper.register_property(PropertyInfo(Variant::OBJECT, "icon", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), defaults.icon, &ExtendedItemList::set_item_icon, &ExtendedItemList::get_item_icon);
	base_property_helper.register_property(PropertyInfo(Variant::STRING, "sub_text"), defaults.sub_text, &ExtendedItemList::set_item_sub_text, &ExtendedItemList::get_item_sub_text);
	base_property_helper.register_property(PropertyInfo(Variant::OBJECT, "tag_icon", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), defaults.tag_icon, &ExtendedItemList::set_item_tag_icon, &ExtendedItemList::get_item_tag_icon);
	base_property_helper.register_property(PropertyInfo(Variant::BOOL, "selectable"), defaults.selectable, &ExtendedItemList::set_item_selectable, &ExtendedItemList::is_item_selectable);
	base_property_helper.register_property(PropertyInfo(Variant::BOOL, "disabled"), defaults.disabled, &ExtendedItemList::set_item_disabled, &ExtendedItemList::is_item_disabled);
	PropertyListHelper::register_base_helper(&base_property_helper);
}

ExtendedItemList::ExtendedItemList() {
	scroll_bar_v = memnew(VScrollBar);
	add_child(scroll_bar_v, false, INTERNAL_MODE_FRONT);
	scroll_bar_v->connect(SceneStringName(value_changed), callable_mp(this, &ExtendedItemList::_scroll_changed));

	scroll_bar_h = memnew(HScrollBar);
	add_child(scroll_bar_h, false, INTERNAL_MODE_FRONT);
	scroll_bar_h->connect(SceneStringName(value_changed), callable_mp(this, &ExtendedItemList::_scroll_changed));

	connect(SceneStringName(mouse_exited), callable_mp(this, &ExtendedItemList::_mouse_exited));

	set_focus_mode(FOCUS_ALL);
	set_clip_contents(true);

	clip_canvas_item = RS::get_singleton()->canvas_item_create();
	RS::get_singleton()->canvas_item_set_parent(clip_canvas_item, get_canvas_item());

	property_helper.setup_for_instance(base_property_helper, this);
}

ExtendedItemList::~ExtendedItemList() {
	RS::get_singleton()->free(clip_canvas_item);
}
