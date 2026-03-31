#ifndef GCD_H
#define GCD_H

#include <stdbool.h>


typedef struct {
	double remaining;
} GCD;


static inline void gcd_set(GCD *gcd, double cooldown) {
	gcd->remaining = cooldown;
}

static inline void gcd_tick(GCD *gcd, double dt) {
	if (gcd->remaining < dt) {
		gcd->remaining = 0;
	} else {
		gcd->remaining -= dt;
	}
}

static inline bool gcd_is_free(GCD *gcd) {
	return gcd->remaining <= 0.0;
}
static inline double gcd_remaining(GCD *gcd) {
	return gcd->remaining;
}


#endif // GCD_H
