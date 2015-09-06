#include <pebble.h>
#include "zombie.h"

void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

void prv_availability_changed(SmartstrapServiceId service_id, bool available) {
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

