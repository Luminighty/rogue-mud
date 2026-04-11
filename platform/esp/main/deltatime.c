#include "deltatime.h"
#include <stdint.h>
#include "esp_timer.h"

static uint64_t last_time;

void delta_time_init() {
	last_time = esp_timer_get_time();
}

double delta_time_get() {
	uint64_t time = esp_timer_get_time();

	double delta_ms = (double)(time - last_time) / 1000.0;
	last_time = time;

	if (delta_ms < 0)
		delta_ms = 0;
	return delta_ms;
}

