#include <pebble.h>

#define LOCATION_LATITUDE 0
#define LOCATION_LONGITUDE 1
	
static Window *s_main_window;

static TextLayer *s_time_layer, *s_date_layer, *s_location_layer;

static GFont s_time_font, s_date_font, s_location_font;

// latitude/longitude buffers
// static char latitude_buffer[32];
// static char longitude_buffer[32];
// static char full_location_buffer[32];

// point counter buffer
static char point_counter_buffer[32];

// ZOMBIE OR HUMAN?
int red_or_blue = 0;

// Point counter
int point_counter = 0;

// BUTTON CLICKS
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (red_or_blue == 0) {
		window_set_background_color(s_main_window, GColorRed);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ZOMBIE_CONTROL_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		red_or_blue = 1;
		point_counter++;
		
		snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
		text_layer_set_text(s_location_layer, point_counter_buffer);
		
	} else {
		window_set_background_color(s_main_window, GColorBlue);
		
		// Create GFont
		s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HUMAN_FONT_40));

		// Apply GFont to TextLayer
		text_layer_set_font(s_time_layer, s_time_font);
		
		red_or_blue = 0;
		point_counter++;
		
		snprintf(point_counter_buffer, sizeof(point_counter_buffer), "Score: %d", point_counter);
		text_layer_set_text(s_location_layer, point_counter_buffer);
		
	}
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
	strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);

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
	
	// LOCATION LAYER //
	
	// Create location Layer
	s_location_layer = text_layer_create(GRect(0, 130, 144, 25));
	text_layer_set_background_color(s_location_layer, GColorClear);
	text_layer_set_text_color(s_location_layer, GColorWhite);
	text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
	
	// Print out point counter on app
	text_layer_set_text(s_location_layer, "Loading...");
	
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
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}