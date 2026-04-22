#ifndef PATHFIND_H
#define PATHFIND_H

#include "darray.h"
#include "linalg.h"
#include <stdint.h>


typedef uint16_t PathCost;

typedef DynamicArray(Vec2i) Path;
typedef PathCost (*Pathfind2D_CostFn)(Vec2i, void*);
bool pathfind_2d(
	Path *result, 
	Vec2i from, Vec2i to, 
	Pathfind2D_CostFn cost_fn, void *data
);

#define PATHFIND_BLOCK 0


#endif // PATHFIND_H
