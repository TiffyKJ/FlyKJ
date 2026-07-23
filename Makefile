CC = gcc

CFLAGS = -Wall -Wextra -O2 -std=c99 -Isrc -MMD -MP


SRC_DIR = src
BUILD_DIR = build


SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS = $(OBJS:.o=.d)



ALL_TESTS = \
test_noise \
test_preamble \
test_phase \
test_ppm



all: $(ALL_TESTS)



$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)



$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



test_noise: tests/test_noise.c $(BUILD_DIR)/noise.o
	$(CC) $(CFLAGS) $^ -o $@



test_preamble: tests/test_preamble.c $(BUILD_DIR)/preamble.o
	$(CC) $(CFLAGS) $^ -o $@



test_phase: tests/test_phase.c $(BUILD_DIR)/phase.o
	$(CC) $(CFLAGS) $^ -o $@



test_ppm: tests/test_ppm.c $(BUILD_DIR)/ppm.o
	$(CC) $(CFLAGS) $^ -o $@



-include $(DEPS)



clean:
	rm -rf $(BUILD_DIR)
	rm -f $(ALL_TESTS)

.PHONY: all clean
