#include <pebble.h>
#include "zombie.h"

// BUTTON CLICKS
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	//update_humanorzombie();
}

void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

