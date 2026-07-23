CC=gcc

CFLAGS=\
-Wall \
-Wextra \
-O2 \
-std=c99 \
-Isrc \
-MMD \
-MP


SRC_DIR=src
BUILD_DIR=build

SRCS=$(wildcard $(SRC_DIR)/*.c)

OBJS=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS=$(OBJS:.o=.d)

all: power_export

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)

	$(CC) $(CFLAGS) \
	-c $< \
	-o $@

power_export: tools/power_export.c $(OBJS)

	$(CC) $(CFLAGS) \
	$< \
	$(OBJS) \
	-o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f power_export
	rm -f power.bin

.PHONY: all clean
