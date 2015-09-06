#include <pebble.h>
#include "zombie.h"

#define LOCATION_LATITUDE 0
#define LOCATION_LONGITUDE 1
	
#define PERSIST_POINT_COUNTER 10
#define PERSIST_RACE 11

static void update_point_counter() {
	snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
	text_layer_set_text(s_location_layer, point_counter_buffer);
	
	persist_write_int(PERSIST_POINT_COUNTER, point_counter);
}

//Smartstrap code
static void prv_availability_changed(SmartstrapServiceId service_id, bool available) {
  if (service_id != SERVICE_ID) {
    return;
  }
/**
  if (available) {
    text_layer_set_background_color(status_text_layer, GColorGreen);
    text_layer_set_text(status_text_layer, "Connected!");
  } else {
    text_layer_set_background_color(status_text_layer, GColorRed);
    text_layer_set_text(status_text_layer, "Disconnected!");
  }
  */
}
/*
static void prv_set_led_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(attribute.led.ptr, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(attribute.led.ptr, 1, false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}
*/
static void prv_set_tpin23_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(attribute.tpin23.ptr, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(attribute.tpin23.ptr, 1, false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}

static void prv_set_tpin22_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(attribute.tpin22.ptr, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(attribute.tpin22.ptr, 1, false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}

//Smartstrap Code End

static void read_point_counter() {
	// Check to see if a count already exists
  if (persist_exists(PERSIST_POINT_COUNTER)) {
    // Load stored count
    point_counter = persist_read_int(PERSIST_POINT_COUNTER);
  }
	
	update_point_counter();
}

static void update_humanorzombie() {
  if (race == human) {
		window_set_background_color(s_main_window, GColorRed);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
    race = zombie;
    prv_set_tpin23_attribute(false);
    prv_set_tpin22_attribute(true);
		status_t success = 	persist_write_int(PERSIST_RACE, 1);
		//persist_write_int(PERSIST_RACE, 1);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)success);

		point_counter++; 
		
		update_point_counter();
		
	} else if (race == zombie) {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		race = human; // change to human
    prv_set_tpin23_attribute(true);
    prv_set_tpin22_attribute(false);
		status_t success = persist_write_int(PERSIST_RACE, 0);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)success);

		point_counter++;

		update_point_counter();
	} else {
		window_set_background_color(s_main_window, GColorBlack);
	}
}

static void read_humanorzombie() {
	
	// Check to see if human or zombie already exists
  if (persist_exists(PERSIST_RACE)) {
    // Load stored count
    race = persist_read_int(PERSIST_RACE);
  }
	
	if (race == human) {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		persist_write_int(PERSIST_RACE, 0);
	} else if (race == zombie) {
		window_set_background_color(s_main_window, GColorRed);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		persist_write_int(PERSIST_RACE, 1);
	} else {
		window_set_background_color(s_main_window, GColorBlack);
	}
}

static void update_time() {
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

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	
	// Get location update every minute
	if(tick_time->tm_min % 1 == 0) {
		// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

		// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

		// Send the message!
		app_message_outbox_send();
	}
}

// The primary method of communication for all Pebble watchapps and watchfaces is the AppMessage API. 
// This allows the construction of key-value dictionaries for transmission between the watch and connected phone.
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case LOCATION_LATITUDE:
  		printf("latitude %s", t->value->cstring);
			//snprintf(latitude_buffer, sizeof(latitude_buffer), "%s", t->value->cstring);
      break;
    case LOCATION_LONGITUDE:
			printf("longitude %s", t->value->cstring);
			//snprintf(longitude_buffer, sizeof(longitude_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
	
	// Assemble full string and display
	//snprintf(full_location_buffer, sizeof(full_location_buffer), "%s, %s", latitude_buffer, longitude_buffer);
	//text_layer_set_text(s_location_layer, full_location_buffer);	
	
	snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
	text_layer_set_text(s_location_layer, point_counter_buffer);
}

// static void point_counter_display() {
// 	snprintf(point_counter_buffer, sizeof(point_counter_buffer), "%d", point_counter);
// 	text_layer_set_text(s_location_layer, point_counter_buffer);
// }

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

