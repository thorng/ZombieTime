#include <pebble.h>

#define PERSIST_POINT_COUNTER 10
#define PERSIST_RACE 11

// Constants, structs, and enums
const SmartstrapServiceId SERVICE_ID = 0x1001;

typedef struct {
  SmartstrapAttributeId id;
  size_t length;
  SmartstrapAttribute *ptr;
} attribute_tuple;

typedef struct {
  attribute_tuple race;
} Attribute;

enum Race {
  human,
  zombie
};

// static Variables
Attribute attribute = {
    .race = {
      .id = 0x0001,
      .length = 1,
      .ptr = NULL
    }
  };

static enum Race race = human;

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_location_layer;

static GFont s_time_font;
static GFont s_date_font;
static GFont s_location_font;

static char point_counter_buffer[32];
static int point_counter = 0;
static bool smart_enabled = false;

// Function prototypes
// main.c
int main(void);
// static void init(void);
// static void deinit(void);

// window.c
void main_window_load(Window *window);
void main_window_unload(Window *window);

// handlers.c
void select_click_handler(ClickRecognizerRef recognizer, void *context);
void click_config_provider(void *context);
void tick_handler(struct tm *tick_time, TimeUnits units_changed);

// storage.c
void prv_set_race_attribute(enum Race currentRace);
void read_point_counter();
void read_race();

// update.c
void update_point_counter(void);
void update_race();
void update_time();

// callback.c
void click_config_provider(void *context);
void prv_availability_changed(SmartstrapServiceId service_id, bool available);

void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

void prv_availability_changed(SmartstrapServiceId service_id, bool available) {
  if (service_id != SERVICE_ID) {
    return;
  }
  
  smart_enabled = available;
  
}

// BUTTON CLICKS
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_race();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


void prv_set_race_attribute(enum Race currentRace) {
  if(!smart_enabled)
    return;
  SmartstrapResult result;
  uint8_t *buffer;
  size_t length;
  result = smartstrap_attribute_begin_write(attribute.race.ptr, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Begin write failed with error %d", result);
    return;
  }

  memcpy(buffer, &currentRace, sizeof(currentRace));

  result = smartstrap_attribute_end_write(attribute.race.ptr, sizeof(currentRace), false);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "End write failed with error %d", result);
    return;
  }
}

void read_point_counter() {
	// Check to see if a count already exists
  if (persist_exists(PERSIST_POINT_COUNTER)) {
    // Load stored count
    point_counter = persist_read_int(PERSIST_POINT_COUNTER);
  }
	
	update_point_counter();
}

void read_race() {
	
	// Check to see if human or zombie already exists
  if (persist_exists(PERSIST_RACE)) {
    // Load stored count
    race = persist_read_int(PERSIST_RACE);
  } else {
    race = human;
  }

  update_race(race);
}

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

  APP_LOG(APP_LOG_LEVEL_DEBUG, "write: %d", (int) success);

  point_counter++;

  update_point_counter();
  
  prv_set_race_attribute(race);
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

void attribute_setup(attribute_tuple *tuple) {
 (*tuple).ptr = smartstrap_attribute_create(SERVICE_ID, (*tuple).id, (*tuple).length);
}

static void init() {
	
  // Create main Window element and assign to pointer
  s_main_window = window_create();
	
	// Register the config provider with the window when it is being created
	window_set_click_config_provider(s_main_window, click_config_provider);
	
	// Register with TickTimerService
  //tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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
	read_race();
  
  //Smartstrap setup
  // setup smartstrap

  SmartstrapHandlers handlers = (SmartstrapHandlers) {
    .availability_did_change = prv_availability_changed,
    //.did_read = prv_did_read,
    //.notified = prv_notified,
  };
  smartstrap_subscribe(handlers);
  attribute_setup(&attribute.race);
}

static void deinit() {
	// Destroy Window
	//persist_write_int(point_counter, point_counter);
	window_destroy(s_main_window);
  smartstrap_attribute_destroy(attribute.race.ptr);
  //smartstrap_attribute_destroy(attribute.tpin23.ptr);
  //smartstrap_attribute_destroy(attribute.tpin22.ptr);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
