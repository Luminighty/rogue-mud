#ifndef FOV_H
#define FOV_H

#include <stdbool.h>


typedef bool (*FOV2D_IsOpaqueFn)(int, int, void*);
typedef void (*FOV2D_OnVisibleFn)(int, int, void*);


void fov_2d(
	int x, int y, int max_distance,
	FOV2D_IsOpaqueFn is_opaque,
	FOV2D_OnVisibleFn on_visible,
	void* data
);


#endif // FOV_H
