#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#define LOCATION_LATITUDE 0
#define LOCATION_LONGITUDE 1
  
#define PERSIST_POINT_COUNTER 10
#define PERSIST_RACE 11

// Constants, structs, and enums
static const SmartstrapServiceId SERVICE_ID = 0x1001;

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
static Attribute attribute = {
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

#endif
