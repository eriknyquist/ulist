ifeq ($(OSTYPE),cygwin)
	CLEANUP=rm -f
	MKDIR=mkdir -p
else ifeq ($(OS),Windows_NT)
	CLEANUP=del /F /Q
	MKDIR=mkdir
else
	CLEANUP=rm -f
	MKDIR=mkdir -p
endif

OBJ := $(patsubst %.c,%.o,$(wildcard src/*.c))
TEST_MAIN_OBJ += test/main.o

TEST_MAIN := test_main
UNITY_SRC := unity
UNIT_TEST_DIR := test/unit_tests
TEST_BUILD_DIR := $(UNIT_TEST_DIR)/build

CFLAGS_BASE := -Wall -Isrc
TEST_MAIN_CFLAGS := $(CFLAGS_BASE) -Itest
UNIT_TEST_CFLAGS := $(CFLAGS_BASE) -I$(UNIT_TEST_DIR) -I$(UNITY_SRC)
DEBUG_CFLAGS := $(CFLAGS_BASE) -g3 -O0

UNIT_TEST_FILES := $(wildcard $(UNIT_TEST_DIR)/test_*.c)
UNIT_TEST_BINS := $(patsubst $(UNIT_TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%,$(UNIT_TEST_FILES))
UNIT_TEST_OBJS := $(patsubst $(UNIT_TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%.o,$(UNIT_TEST_FILES))

UNITY_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNITY_SRC)/*.c))

.PHONY: clean run-tests test-build-dir

all: $(TEST_MAIN)

test-build-dir:
	$(MKDIR) $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.o: $(UNIT_TEST_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TEST_BUILD_DIR)/%: $(UNIT_TEST_DIR)/%.o
	$(CC) $(CFLAGS) $< $(OBJ) $(UNITY_OBJ) -o $@

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(TEST_MAIN)

$(TEST_MAIN): CFLAGS = $(TEST_MAIN_CFLAGS)
$(TEST_MAIN): $(OBJ) $(TEST_MAIN_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

unit-tests: CFLAGS = $(UNIT_TEST_CFLAGS)
unit-tests: test-build-dir $(OBJ) $(UNITY_OBJ) $(UNIT_TEST_OBJS) $(UNIT_TEST_BINS)

clean:
	$(CLEANUP) $(OBJ) $(UNITY_OBJ) $(TEST_MAIN_OBJ) $(TEST_MAIN)
	$(CLEANUP) $(UNIT_TEST_OBJS) $(UNIT_TEST_BINS)
