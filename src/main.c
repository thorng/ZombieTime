#include <pebble.h>

//Smartstrap start
  
static const SmartstrapServiceId SERVICE_ID = 0x1001;
static const SmartstrapAttributeId LED_ATTRIBUTE_ID = 0x0001;
static const size_t LED_ATTRIBUTE_LENGTH = 1;
static const SmartstrapAttributeId UPTIME_ATTRIBUTE_ID = 0x0002;
static const size_t UPTIME_ATTRIBUTE_LENGTH = 4;
static const SmartstrapAttributeId TPIN23_ATTRIBUTE_ID = 0x0003;
static const size_t TPIN23_ATTRIBUTE_LENGTH = 1;
static const SmartstrapAttributeId TPIN22_ATTRIBUTE_ID = 0x0004;
static const size_t TPIN22_ATTRIBUTE_LENGTH = 1;



static SmartstrapAttribute *led_attribute;
static SmartstrapAttribute *uptime_attribute;
static SmartstrapAttribute *tpin23_attribute;
static SmartstrapAttribute *tpin22_attribute;

#define LOCATION_LATITUDE 0
#define LOCATION_LONGITUDE 1
	
#define PERSIST_POINT_COUNTER 10
#define PERSIST_HUMANORZOMBIE 11
	
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_location_layer;
static GFont s_time_font, s_date_font, s_location_font;

// latitude/longitude buffers
// static char latitude_buffer[32];
// static char longitude_buffer[32];
// static char full_location_buffer[32];

// point counter buffer
static char point_counter_buffer[32];

// ZOMBIE OR HUMAN? Human = 0 = blue; Zombie = 1 = red;
int red_or_blue = 0;

//Led toggles,
int tpin23 = 1;
int tpin22 = 0;

// Point counter
int point_counter = 0;

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

//  if (available) {
//    text_layer_set_background_color(status_text_layer, GColorGreen);
//    text_layer_set_text(status_text_layer, "Connected!");
//  } else {
//    text_layer_set_background_color(status_text_layer, GColorRed);
//    text_layer_set_text(status_text_layer, "Disconnected!");
//  }
}

static void prv_did_read(SmartstrapAttribute *attr, SmartstrapResult result,
                         const uint8_t *data, size_t length) {
  if (attr != uptime_attribute) {
    return;
  }
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Read failed with result %d", result);
    return;
  }
  if (length != UPTIME_ATTRIBUTE_LENGTH) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Got response of unexpected length (%d)", length);
    return;
  }

//  static char uptime_buffer[20];
//  snprintf(uptime_buffer, 20, "%u", (unsigned int)*(uint32_t *)data);
//  text_layer_set_text(uptime_text_layer, uptime_buffer);
}

static void prv_set_led_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(led_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(led_attribute, 1, false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}


static void prv_set_tpin23_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(tpin23_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(tpin23_attribute, 1, false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}

static void prv_set_tpin22_attribute(bool on) {
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(tpin22_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  buffer[0] = on;

  result = smartstrap_attribute_end_write(tpin22_attribute, 1, false);
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
	// 0 = Human, 1 = Zombie
	if (red_or_blue == 0) {
		window_set_background_color(s_main_window, GColorRed);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		red_or_blue = 1; // change to zombie
    tpin23_attribute = 0;
    tpin22_attribute = 1;
		status_t success = 	persist_write_int(PERSIST_HUMANORZOMBIE, 1);
		//persist_write_int(PERSIST_HUMANORZOMBIE, 1);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)success);

		point_counter++; 
		
		update_point_counter();
		
	} else if (red_or_blue == 1) {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		red_or_blue = 0; // change to human
    tpin23_attribute = 1;
    tpin22_attribute = 0;
		status_t success = persist_write_int(PERSIST_HUMANORZOMBIE, 0);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)success);

		point_counter++;

		update_point_counter();
	} else {
		window_set_background_color(s_main_window, GColorBlack);
	}
}

static void read_humanorzombie() {
	
	// Check to see if human or zombie already exists
  if (persist_exists(PERSIST_HUMANORZOMBIE)) {
    // Load stored count
    red_or_blue = persist_read_int(PERSIST_HUMANORZOMBIE);
  }
	
	if (red_or_blue == 0) {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		persist_write_int(PERSIST_HUMANORZOMBIE, 0);
	} else if (red_or_blue == 1) {
		window_set_background_color(s_main_window, GColorRed);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		persist_write_int(PERSIST_HUMANORZOMBIE, 1);
	} else {
		window_set_background_color(s_main_window, GColorBlack);
	}
}

// BUTTON CLICKS
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	update_humanorzombie();
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
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

static void main_window_load(Window *window) {
	
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

static void main_window_unload(Window *window) {
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

// static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
//   APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
// }

static void init() {
	
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	//app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  // Create main Window element and assign to pointer
  s_main_window = window_create();
	
	// Register the config provider with the window when it is being created
	window_set_click_config_provider(s_main_window, click_config_provider);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	// Make sure the time is displayed from the start
	update_time();
	
	// Make sure point counter is updated and displayed from the start
	read_point_counter();
	
	// Make sure human or zombie persists and is displayed from the start
	read_humanorzombie();
  
  //Smartstrap setup
    // setup smartstrap
  SmartstrapHandlers handlers = (SmartstrapHandlers) {
    .availability_did_change = prv_availability_changed,
    .did_read = prv_did_read,
    .notified = prv_notified,
  };
  smartstrap_subscribe(handlers);
  led_attribute = smartstrap_attribute_create(SERVICE_ID, LED_ATTRIBUTE_ID, LED_ATTRIBUTE_LENGTH);
  uptime_attribute = smartstrap_attribute_create(SERVICE_ID, UPTIME_ATTRIBUTE_ID, UPTIME_ATTRIBUTE_LENGTH);
  tpin23_attribute = smartstrap_attribute_create(SERVICE_ID, TPIN23_ATTRIBUTE_ID, TPIN23_ATTRIBUTE_LENGTH);
  tpin22_attribute = smartstrap_attribute_create(SERVICE_ID, TPIN22_ATTRIBUTE_ID, TPIN22_ATTRIBUTE_LENGTH);
}

static void deinit() {
	// Destroy Window
	persist_write_int(point_counter, point_counter);
	window_destroy(s_main_window);
  smartstrap_attribute_destroy(led_attribute);
  smartstrap_attribute_destroy(uptime_attribute);
  smartstrap_attribute_destroy(tpin23_attribute);
  smartstrap_attribute_destroy(tpin22_attribute);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}