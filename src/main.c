#include <pebble.h>
#include "zombie.h"

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
	//update_time();
	
	// Make sure point counter is updated and displayed from the start
	//read_point_counter();
	
	// Make sure human or zombie persists and is displayed from the start
	//read_humanorzombie();
  
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
