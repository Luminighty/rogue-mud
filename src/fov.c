#include "fov.h"

#include <stdbool.h>
#include <math.h>

// https://www.jordansavant.com/book/algorithms/shadowcasting.md

static inline void cast_light(
	int x, int y,
	FOV2D_IsOpaqueFn is_opaque, FOV2D_OnVisibleFn on_visible, void* data,
	int octant_id, int row, float start_slope, float end_slope,
	int xx, int xy, int yx, int yy, int max_distance
) {
	if (start_slope < end_slope)
		return;

	float next_start_slope = start_slope;
	for (int i = row; i <= max_distance; i++) {
		bool blocked = false;

		for (int dx = -i, dy = -i; dx <= 0; dx++) {
			float left_slope = ((float)dx - 0.5) / ((float)dy + 0.5);
			float right_slope = ((float)dx + 0.5) / ((float)dy - 0.5);

			if (start_slope < right_slope)
				continue;
			if (end_slope > left_slope)
				break;
		
			float sax = dx * xx + dy * xy;
			float say = dx * yx + dy * yy;
			if ((sax < 0 && fabs(sax) > x) || (say < 0 && fabs(say) > y))
				continue;

			float ax = x + sax;
			float ay = y + say;

			if ((dx * dx + dy * dy) < max_distance * max_distance)
				on_visible(ax, ay, data);

			if (blocked) {
				if (is_opaque(ax, ay, data)) {
					next_start_slope = right_slope;
				} else {
					blocked = false;
					start_slope = next_start_slope;
				}
				continue;
			}
			if (is_opaque(ax, ay, data)) {
				blocked = true;
				next_start_slope = right_slope;
				cast_light(
					x, y, is_opaque, on_visible, data,
					octant_id, i + 1, start_slope, left_slope,
					xx, xy, yx, yy, max_distance
				);
			}
		}
		if (blocked)
			break;
	}
}


static const float MULTIPLIERS[4][8] = {
	{1,  0,  0, -1, -1,  0,  0,  1},
	{0,  1, -1,  0,  0, -1,  1,  0},
	{0,  1,  1,  0,  0, -1, -1,  0},
	{1,  0,  0,  1, -1,  0,  0, -1}
};


void fov_2d(
	int x, int y, int max_distance,
	FOV2D_IsOpaqueFn is_opaque, FOV2D_OnVisibleFn on_visible,
	void* data
) {
	on_visible(x, y, data);
	for (int i = 0; i < 8; i++) {
		cast_light(
			x, y, is_opaque, on_visible, data,
			i, 1, 1.0, 0.0,
			MULTIPLIERS[0][i], MULTIPLIERS[1][i],
			MULTIPLIERS[2][i], MULTIPLIERS[3][i],
			max_distance
		);
	}
}

