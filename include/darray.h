#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>
#include <stdlib.h>

#define DynamicArray(Item) \
	struct { \
		Item *items; \
		size_t count; \
		size_t capacity; \
	}


#define da_push(vec, value)\
	do {\
	if ((vec).count >= (vec).capacity) {\
		if ((vec).capacity == 0) (vec).capacity = 256;\
		else (vec).capacity *= 2;\
		vec.items = realloc((vec).items, (vec).capacity * sizeof(*(vec).items));\
	}\
	(vec).items[(vec).count++] = value;\
	} while (0)

#define da_pop(vec) (vec).items[--(vec).count]
#define da_clear(vec) (vec).count = 0
#define foreach_da(vec, i) for (size_t i = 0; i < (vec).count; i++)

#define da_reserve(vec, amount)\
	do {\
	if ((vec).count + (amount) > (vec).capacity) {\
		if ((vec).capacity == 0) (vec).capacity = (amount);\
		else (vec).capacity = ((vec).count + (amount)) * 2;\
		(vec).items = realloc((vec).items, (vec).capacity * sizeof(*(vec).items));\
	}\
	} while (0)

#endif // DARRAY_H
