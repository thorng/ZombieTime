#ifndef _ZOMBIE_H
#define _ZOMBIE_H

// Constants, structs, and enums
static const SmartstrapServiceId SERVICE_ID = 0x1001;

typedef struct {
  SmartstrapAttributeId id;
  size_t length;
  SmartstrapAttribute *ptr;
} attribute_tuple;

typedef struct {
  attribute_tuple led;
  attribute_tuple tpin23;
  attribute_tuple tpin22;
} Attribute;

enum Race {
  human,
  zombie
};

// static Variables
static Attribute attribute = {
  .led = {
    .id = 0x0001,
    .length = 1,
    .ptr = NULL
  },
 .tpin22 = {
    .id = 0x0003,
    .length = 1,
    .ptr = NULL
  },
 .tpin23 = {
    .id = 0x0004,
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

#endif
