#include <pebble.h>
#include "zombie.h"

void prv_set_race_attribute(enum Race currentRace) {
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
