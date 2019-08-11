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
TEST_DIR := test
TEST_BUILD_DIR := $(TEST_DIR)/build

CFLAGS_BASE := -Wall -Isrc
TEST_MAIN_CFLAGS := $(CFLAGS_BASE) -Itest -O3
TEST_CFLAGS := $(CFLAGS_BASE) -O3 -I$(TEST_DIR) -I$(UNITY_SRC)
TEST_CFLAGS := $(CFLAGS_BASE) -g3 -O0 -I$(TEST_DIR) -I$(UNITY_SRC)
DEBUG_CFLAGS := $(CFLAGS_BASE) -g3 -O0

TEST_FILES := $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%,$(TEST_FILES))
TEST_OBJS := $(patsubst $(TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%.o,$(TEST_FILES))
TEST_RESULTS := $(patsubst $(TEST_DIR)/test_%.c,$(TEST_BUILD_DIR)/test_%.txt,$(TEST_FILES))

UNITY_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNITY_SRC)/*.c))

.PHONY: clean run-tests test-build-dir debug test-build-dir tests

all: $(TEST_MAIN)

test-build-dir:
	$(MKDIR) $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TEST_BUILD_DIR)/%: $(TEST_BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< $(OBJ) $(UNITY_OBJ) -o $@

$(TEST_BUILD_DIR)/%.txt: $(TEST_BUILD_DIR)/%
	@echo "Running $<"
	-@./$< > $@ 2>&1
	@cat $@
	@echo ""

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(TEST_MAIN)

$(TEST_MAIN): CFLAGS = $(TEST_MAIN_CFLAGS)
$(TEST_MAIN): $(OBJ) $(TEST_MAIN_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

build-tests: CFLAGS = $(TEST_CFLAGS)
build-tests: test-build-dir $(OBJ) $(UNITY_OBJ) $(TEST_OBJS) $(TEST_BINS)

tests: CFLAGS = $(TEST_CFLAGS)
tests: build-tests $(TEST_RESULTS)
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
	$(CLEANUP) $(TEST_OBJS) $(TEST_BINS) $(TEST_RESULTS)
