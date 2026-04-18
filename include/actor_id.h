#ifndef ACTOR_ID_H
#define ACTOR_ID_H

#include <stdint.h>


typedef union {
	uint64_t id;
	struct {
		uint32_t index;
		uint32_t gen;
	};
} ActorId;


#endif // ACTOR_ID_H
