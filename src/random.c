#include "random.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PRIME1 198491317
#define PRIME2 6542989
#define PRIME3 357239

static unsigned int inner_state = 0;

// Code from https://gist.github.com/kevinmoran/0198d8e9de0da7057abe8b8b34d50f86
unsigned int random1d(unsigned int seed, int value) {
	static const unsigned int SQ5_BIT_NOISE1 = 0xd2a80a3f;
	static const unsigned int SQ5_BIT_NOISE2 = 0xa884f197;
	static const unsigned int SQ5_BIT_NOISE3 = 0x6C736F4B;
	static const unsigned int SQ5_BIT_NOISE4 = 0xB79F3ABB;
	static const unsigned int SQ5_BIT_NOISE5 = 0x1b56c4f5;

	unsigned int mangled = (unsigned int) value;
	mangled *= SQ5_BIT_NOISE1;
	mangled += seed;
	mangled ^= (mangled >> 9);
	mangled += SQ5_BIT_NOISE2;
	mangled ^= (mangled >> 11);
	mangled *= SQ5_BIT_NOISE3;
	mangled ^= (mangled >> 13);
	mangled += SQ5_BIT_NOISE4;
	mangled ^= (mangled >> 15);
	mangled *= SQ5_BIT_NOISE5;
	mangled ^= (mangled >> 17);
	return mangled;
}


unsigned int random2d(unsigned int seed, int x, int y) {
	return random1d(seed, x + (PRIME1 * y));
}

unsigned int random3d(unsigned int seed, int x, int y, int z) {
	return random1d(seed, x + (PRIME1 * y) + (PRIME2 * z));
}

unsigned int random4d(unsigned int seed, int x, int y, int z, int w) {
	return random1d(seed, x + (PRIME1 * y) + (PRIME2 * z) + (PRIME3 * w));
}


#define ONE_OVER_MAX_UINT (1.0 / (double)0xFFFFFFFF)
#define to_float(value) (float)(ONE_OVER_MAX_UINT * (double)(value))

float randomf(unsigned int seed, int value)	 { return to_float(random1d(seed, value)); }
float randomf2d(unsigned int seed, int x, int y) { return to_float(random2d(seed, x, y)); }
float randomf3d(unsigned int seed, int x, int y, int z)		{ return to_float(random3d(seed, x, y, z)); }
float randomf4d(unsigned int seed, int x, int y, int z, int w)	{ return to_float(random4d(seed, x, y, z, w)); }


int random_range(int min, int max) {
	return (random1d(0, inner_state++) % (max - min)) + min;
}
float random_frange(float min, float max) {
	return fmodf(randomf(0, inner_state++), max - min) + min;
}

unsigned int random_generate_seed() {
	return rand();
}

void random_set_seed(unsigned int seed) {
	inner_state = seed;
}

void random_init() {
	srandom(time(NULL));
	random_set_seed(random_generate_seed());
}

