#include <pebble.h>
#include "zombie.h"

// BUTTON CLICKS
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	//update_humanorzombie();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
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
