#include "pathfind.h"
#include "darray.h"
#include "linalg.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct path_node;
typedef struct path_node {
	// NOTE: Estimated final cost of the path
	PathCost h_cost;
	// NOTE: Current cost of the path
	PathCost c_cost;
	Vec2i pos;
	int prev;
} PathNode;

typedef DynamicArray(PathNode) PathNodes;

static PathNodes queue = {0};
static PathNodes visited = {0};

static inline PathNode *pathnodes_find(PathNodes *nodes, Vec2i pos) {
	foreach_da(*nodes, i) {
		if (vec2i_eq(nodes->items[i].pos, pos))
			return &nodes->items[i];
	}
	return NULL;
}

#define max(a, b) (a > b) ? a : b
static inline PathCost calculate_h_cost(Vec2i current, Vec2i target, PathCost current_cost) {
	return max(abs(current.x - target.x), abs(current.y - target.y)) + current_cost;
}


static inline void try_add(int prev, Vec2i delta, Vec2i target, PathCost current_cost, Pathfind2D_CostFn cost_fn, void *data) {
	Vec2i pos = vec2i_add(visited.items[prev].pos, delta);
	PathCost cost_delta = cost_fn(pos, data);
	if (cost_delta == PATHFIND_BLOCK)
		return;
	// NOTE: In case of an overflow, we skip
	if (cost_delta + current_cost < current_cost)
		return;
	if (pathnodes_find(&visited, pos))
		return;
	if (pathnodes_find(&queue, pos))
		return;

	PathCost new_cost = current_cost + cost_delta;
	PathNode node = {
		.h_cost = calculate_h_cost(pos, target, new_cost),
		.c_cost = new_cost,
		.prev = prev,
		.pos = pos,
	};
	da_push(queue, (PathNode){0});
	int i = queue.count - 2;
	for (; i >= 0; i--) {
		PathNode *q_node = &queue.items[i];
		if (q_node->h_cost >= node.h_cost)
			break;
		queue.items[i + 1] = *q_node;
	}
	queue.items[i + 1] = node;
}


bool pathfind_2d(
	Path *result,
	Vec2i from, Vec2i to,
	Pathfind2D_CostFn cost_fn, void *data
) {
	da_clear(queue);
	da_clear(visited);

	PathCost cost = cost_fn(from, data);
	da_push(queue, ((PathNode){
		.h_cost = calculate_h_cost(from, to, cost),
		.c_cost = cost,
		.pos = from,
		.prev = 0,
	}));
	bool found = false;
	while (queue.count > 0) {
		da_push(visited, da_pop(queue));
		int node = visited.count - 1;
		if (vec2i_eq(visited.items[node].pos, to)) {
			found = true;
			break;
		}
		
		PathCost cost = visited.items[node].c_cost;
		try_add(node, VEC2I_RIGHT, to, cost, cost_fn, data);
		try_add(node, VEC2I_LEFT , to, cost, cost_fn, data);
		try_add(node, VEC2I_UP   , to, cost, cost_fn, data);
		try_add(node, VEC2I_DOWN , to, cost, cost_fn, data);
	}
	if (!found)
		return false;

	da_clear(*result);
	PathNode *current = pathnodes_find(&visited, to);
	assert(current != NULL);
	while (!vec2i_eq(current->pos, from)) {
		da_push(*result, current->pos);
		current = &visited.items[current->prev];
		assert(current != NULL);
	}
	return true;
}

