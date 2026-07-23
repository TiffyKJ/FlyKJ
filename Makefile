CC = gcc

CFLAGS = \
-Wall \
-Wextra \
-O2 \
-std=c99 \
-Isrc \
-MMD \
-MP


SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = \
$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS = $(OBJS:.o=.d)

all: power_export test_noise noise_export

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

power_export: tools/power_export.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

test_noise: tests/test_noise.c build/noise.o
	$(CC) $(CFLAGS) $^ -o $@

noise_export: tools/noise_export.c build/noise.o
	$(CC) $(CFLAGS) $^ -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f power_export
	rm -f test_noise
	rm -f noise_export


.PHONY: all clean
