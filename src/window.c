#include <pebble.h>
#include "zombie.h"

void main_window_load(Window *window) {
	
	window_set_background_color(window, GColorBlue);
	
	// TIME LAYER //
	
	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);

  // Improve the layout to be more like a watchface
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	// Create GFont
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

	// Apply GFont to TextLayer
	text_layer_set_font(s_time_layer, s_time_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	// DATE LAYER //
	
	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 0, 144, 25));
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_15));
	text_layer_set_font(s_date_layer, s_date_font);
	
	// POINT COUNT LAYER //
	
	// Create point Layer
	s_location_layer = text_layer_create(GRect(0, 130, 144, 25));
	text_layer_set_background_color(s_location_layer, GColorClear);
	text_layer_set_text_color(s_location_layer, GColorWhite);
	text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
	
	// Print out point counter on app
	text_layer_set_text(s_location_layer, "Loading...");
	
	snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
	text_layer_set_text(s_location_layer, point_counter_buffer);
	
	// Create second custom font, apply it and add to Window
	s_location_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_15));
	text_layer_set_font(s_location_layer, s_location_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));
}

void main_window_unload(Window *window) {
	// Unload GFont
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);

	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	
	// Destroy location elements
	text_layer_destroy(s_location_layer);
	fonts_unload_custom_font(s_location_font);
}
