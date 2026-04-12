IDIR=./../../include
PLAT_IDIR=./include
TARGET=./bin/main

CC=gcc
CFLAGS=-I$(IDIR) -I$(PLAT_IDIR)
LDFLAGS=-lm
LDFLAGS_WIN=
LDFLAGS_UNIX=

ifeq ($(OS),Windows_NT)
	LDFLAGS += $(LDFLAGS_WIN)
else
	LDFLAGS += $(LDFLAGS_UNIX)
endif


DEPS=$(wildcard $(IDIR)/*.h) $(wildcard $(PLAT_IDIR)/*.h)

PLAT_SRC_DIR=./src
CORE_SRC_DIR=../../src

CORE_SRCS := $(shell find $(CORE_SRC_DIR) -name '*.c')
PLAT_SRCS := $(shell find $(PLAT_SRC_DIR) -name '*.c')

CORE_OBJS=$(patsubst $(CORE_SRC_DIR)/%.c, build/core/%.o, $(CORE_SRCS))
PLAT_OBJS=$(patsubst $(PLAT_SRC_DIR)/%.c, build/platform/%.o, $(PLAT_SRCS))
OBJS=$(CORE_OBJS) $(PLAT_OBJS)

.PHONY: debug release clean

debug: CFLAGS += -g -O0 -Wall -Wextra -DDEV_MODE
debug: $(TARGET)

release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)


build/core/%.o: $(CORE_SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


build/platform/%.o: $(PLAT_SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


$(TARGET): $(OBJS)
	mkdir -p bin
	# cp -r assets bin/
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)


clean:
	rm -rf build bin

