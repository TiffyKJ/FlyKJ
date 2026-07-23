CC = gcc

#
#not final version
#

CFLAGS = -Wall -Wextra -O2 -std=c99 -Isrc -MMD -MP


SRC_DIR = src
BUILD_DIR = build


SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS = $(OBJS:.o=.d)


all: \
	power_export \
	noise_export \
	test_noise \
	test_preamble \
	test_phase

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


#
# Tools
#

power_export: tools/power_export.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@



noise_export: tools/noise_export.c $(BUILD_DIR)/noise.o
	$(CC) $(CFLAGS) $^ -o $@



#
# Tests
#

test_noise: tests/test_noise.c $(BUILD_DIR)/noise.o
	$(CC) $(CFLAGS) $^ -o $@

test_preamble: tests/test_preamble.c $(BUILD_DIR)/preamble.o
	$(CC) $(CFLAGS) $^ -o $@

test_phase: tests/test_phase.c $(BUILD_DIR)/phase.o
	$(CC) $(CFLAGS) $^ -o $@


#
# Automatic dependency files
#

-include $(DEPS)


clean:
	rm -rf $(BUILD_DIR)

	rm -f \
	power_export \
	noise_export \
	test_noise \
	test_preamble \
	test_phase

	rm -rf output/

.PHONY: all clean
