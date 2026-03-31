#include "deltatime.h"

#include <time.h>

static struct timespec last_time, current_time;

void delta_time_init() {
	timespec_get(&last_time, TIME_UTC);
}

double delta_time_get() {
	timespec_get(&current_time, TIME_UTC);

	double delta_ms = (double)(current_time.tv_sec - last_time.tv_sec) * 1000.0 +
		  (double)(current_time.tv_nsec - last_time.tv_nsec) / 1000000.0;
	last_time = current_time;
	if (delta_ms < 0)
		delta_ms = 0;
	return delta_ms;
}

