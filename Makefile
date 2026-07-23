CC=gcc

#not final version

CFLAGS=-Wall -Wextra -O2 -std=c99 -Isrc -MMD -MP


SRC_DIR=src
BUILD_DIR=build


SRCS=$(wildcard $(SRC_DIR)/*.c)

OBJS=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS=$(OBJS:.o=.d)



all: test_phase



$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)



$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



test_phase: tests/test_phase.c $(BUILD_DIR)/phase.o
	$(CC) $(CFLAGS) $^ -o $@



-include $(DEPS)



clean:
	rm -rf $(BUILD_DIR)
	rm -f test_phase



.PHONY: all clean
