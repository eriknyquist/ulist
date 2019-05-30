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
TEST_MAIN_CFLAGS := $(CFLAGS_BASE) -Itest -O3
UNIT_TEST_CFLAGS := $(CFLAGS_BASE) -O3 -I$(UNIT_TEST_DIR) -I$(UNITY_SRC)
UNIT_TEST_CFLAGS := $(CFLAGS_BASE) -g3 -O0 -I$(UNIT_TEST_DIR) -I$(UNITY_SRC)
DEBUG_CFLAGS := $(CFLAGS_BASE) -g3 -O0

UNIT_TEST_FILES := $(wildcard $(UNIT_TEST_DIR)/test_*.c)
UNIT_TEST_BINS := $(patsubst $(UNIT_TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%,$(UNIT_TEST_FILES))
UNIT_TEST_OBJS := $(patsubst $(UNIT_TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%.o,$(UNIT_TEST_FILES))
UNIT_TEST_RESULTS := $(patsubst $(UNIT_TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%.txt,$(UNIT_TEST_FILES))

UNITY_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNITY_SRC)/*.c))

.PHONY: clean run-tests test-build-dir debug test-build-dir unit-tests

all: $(TEST_MAIN)

test-build-dir:
	$(MKDIR) $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.o: $(UNIT_TEST_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TEST_BUILD_DIR)/%: $(TEST_BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< $(OBJ) $(UNITY_OBJ) -o $@

$(TEST_BUILD_DIR)/%.txt: $(TEST_BUILD_DIR)/%
	@echo "Running $<"
	-@./$< > $@ 2>&1

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(TEST_MAIN)

$(TEST_MAIN): CFLAGS = $(TEST_MAIN_CFLAGS)
$(TEST_MAIN): $(OBJ) $(TEST_MAIN_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

build-unit-tests: CFLAGS = $(UNIT_TEST_CFLAGS)
build-unit-tests: test-build-dir $(OBJ) $(UNITY_OBJ) $(UNIT_TEST_OBJS) $(UNIT_TEST_BINS)

unit-tests: CFLAGS = $(UNIT_TEST_CFLAGS)
unit-tests: build-unit-tests $(UNIT_TEST_RESULTS)
	@echo ""
	@echo "-------- RESULTS---------"
	@echo ""
	@echo "total tests: "`grep -s -h -e ':FAIL' -e ':PASS' $(TEST_BUILD_DIR)/*.txt | wc -l`
	@echo "passed:      "`grep -s -h -e ':PASS' $(TEST_BUILD_DIR)/*.txt | wc -l`
	@echo "failed:      "`grep -s -h -e ':FAIL'  $(TEST_BUILD_DIR)/*.txt | wc -l`
	@echo ""
	@echo "------- FAILURES ---------"
	@echo ""
	@grep -s -h -e ':FAIL' $(TEST_BUILD_DIR)/*.txt && echo "" || true
	@echo "--------------------------"
	@echo ""

clean:
	$(CLEANUP) $(OBJ) $(UNITY_OBJ) $(TEST_MAIN_OBJ) $(TEST_MAIN)
	$(CLEANUP) $(UNIT_TEST_OBJS) $(UNIT_TEST_BINS) $(UNIT_TEST_RESULTS)
