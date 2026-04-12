#ifndef RANDOM_H
#define RANDOM_H

void random_init();
void random_set_seed(unsigned int seed);
unsigned int random_generate_seed();

unsigned int random1d(unsigned int seed, int value);
unsigned int random2d(unsigned int seed, int x, int y);
unsigned int random3d(unsigned int seed, int x, int y, int z);
unsigned int random4d(unsigned int seed, int x, int y, int z, int w);

float randomf1d(unsigned int seed, int value);
float randomf2d(unsigned int seed, int x, int y);
float randomf3d(unsigned int seed, int x, int y, int z);
float randomf4d(unsigned int seed, int x, int y, int z, int w);


int random_range(int min, int max);
float random_frange(float min, float max);


#endif // RANDOM_H
