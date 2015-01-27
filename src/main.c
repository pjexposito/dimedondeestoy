// Código adaptado del ejemplo http://ninedof.wordpress.com/2014/02/02/pebble-sdk-2-0-tutorial-6-appmessage-for-pebblekit-js/
// Gracias a Chris Lewis por toda la documentación

#include <pebble.h>
 
Window* window;
TextLayer *title_layer, *location_layer, *temperature_layer, *time_layer;

char location_buffer[64], temperature_buffer[64], time_buffer[64], numero_buffer[64];

enum {
	KEY_SITUACION = 0,
	KEY_CALLE = 1,
  KEY_NUMERO = 2
  
};

void process_tuple(Tuple *t)
{
	//Get key
	int key = t->key;

	//Get integer value, if present
	
  //int value = t->value->int32;
	char calle[64];
	strcpy(calle, t->value->cstring);
  
  
	//Get string value, if present
	char string_value[64];
	strcpy(string_value, t->value->cstring);
  
  char numero[64];
	strcpy(numero, t->value->cstring);

	//Decide what to do
	switch(key) {
		case KEY_SITUACION:
			//Location received
			snprintf(location_buffer, sizeof("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), "%s", string_value);
			text_layer_set_text(location_layer, (char*) &location_buffer);
			break;
		case KEY_CALLE:
			//Temperature received
			//snprintf(temperature_buffer, sizeof("Temperature: XX \u00B0C"), "Temperature: %d \u00B0C", value);
    	
      snprintf(temperature_buffer, sizeof("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), "%s", calle);

			text_layer_set_text(temperature_layer, (char*) &temperature_buffer);
			break;
    
    case KEY_NUMERO:
			//Temperature received
			//snprintf(temperature_buffer, sizeof("Temperature: XX \u00B0C"), "Temperature: %d \u00B0C", value);
    	
      snprintf(numero_buffer, sizeof("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), "%s", numero);

			text_layer_set_text(time_layer, (char*) &numero_buffer);
			break;
	}

	//Set time this update came in
	time_t temp = time(NULL);	
	struct tm *tm = localtime(&temp);
	strftime(time_buffer, sizeof("Last updated: XX:XX"), "Actualizado: %H:%M", tm);
	//text_layer_set_text(time_layer, (char*) &time_buffer);
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;
	
	//Get data
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	
	//Get next
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_overflow_mode(layer, GTextOverflowModeFill);
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}
 


void window_load(Window *window)
{
	title_layer = init_text_layer(GRect(5, 0, 139, 40), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(title_layer, "Boton central para actualizar");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(title_layer));

	location_layer = init_text_layer(GRect(5, 40, 139, 40), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(location_layer, "Location: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(location_layer));

	temperature_layer = init_text_layer(GRect(5, 80, 139, 40), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(temperature_layer, "Temperature: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(temperature_layer));

	time_layer = init_text_layer(GRect(5, 120, 139, 40), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(time_layer, "Last updated: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}
 
void window_unload(Window *window)
{	
	text_layer_destroy(title_layer);
	text_layer_destroy(location_layer);
	text_layer_destroy(temperature_layer);
	text_layer_destroy(time_layer);
}

void send_int(uint8_t key, uint8_t cmd)
{
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	
 	Tuplet value = TupletInteger(key, cmd);
 	dict_write_tuplet(iter, &value);
 	
 	app_message_outbox_send();
  
  
}



void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    //Refresca la información
  text_layer_set_text(location_layer, "Cargando...");

  vibes_double_pulse();
  

  send_int(5, 5);
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
	//Every five minutes
	if(tick_time->tm_min % 5 == 0)
	{
		//Send an arbitrary message, the response will be handled by in_received_handler()
		send_int(5, 5);
	}
}

void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 1000;
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, select_single_click_handler);
	
}
 
void init()
{
	window = window_create();
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
	window_set_window_handlers(window, handlers);

	//Register AppMessage events
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());		//Largest possible input and output buffer sizes
	
	//Register to receive minutely updates
	tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);

  window_set_click_config_provider(window, click_config_provider);

  
	window_stack_push(window, true);
}
 
void deinit()
{
	tick_timer_service_unsubscribe();
	
	window_destroy(window);
}
 
int main(void)
{
	init();
	app_event_loop();
	deinit();
}