#ifndef GOBLIN_H
#define GOBLIN_H


#include "display.h"
#include "gcd.h"
#include <stdbool.h>


typedef struct {
	bool active;
	GCD gcd;
	int x;
	int y;
	int hp;
} Goblin;

void goblin_init(int x, int y);
void goblin_tick(Goblin *goblin, double dt);
void goblin_render(Display *display, Goblin *goblin);

#endif // GOBLIN_H
