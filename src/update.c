#include <pebble.h>
#include "zombie.h"

void update_point_counter() {
	snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
	text_layer_set_text(s_location_layer, point_counter_buffer);
	
	persist_write_int(PERSIST_POINT_COUNTER, point_counter);
}

void update_race() {
  status_t success;

  if (race == human) {
		window_set_background_color(s_main_window, GColorRed);

    // Create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));
		
    race = zombie;
		success = 	persist_write_int(PERSIST_RACE, 1);
	} else if (race == zombie) {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));
		
		race = human;
		success = persist_write_int(PERSIST_RACE, 0);
	} else {
		window_set_background_color(s_main_window, GColorBlack);
    return;
	}

  text_layer_set_font(s_time_layer, s_time_font);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int) success);

  point_counter++;

  update_point_counter();
}

void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
	// Copy date into buffer from tm structure
	static char date_buffer[16];
	strftime(date_buffer, sizeof(date_buffer), "%a, %d %b", tick_time);

	// Show the date
	text_layer_set_text(s_date_layer, date_buffer);
}
