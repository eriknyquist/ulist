OBJ := $(patsubst %.c,%.o,$(wildcard src/*.c))
TEST_MAIN_OBJ += test/main.o

TEST_MAIN := test_main
UNIT_TEST_BIN := unit_test_runner
UNITY_SRC := unity
UNIT_TEST_SRC := test/unit_tests

CFLAGS_BASE := -Wall -Isrc
TEST_MAIN_CFLAGS := $(CFLAGS_BASE) -Itest
UNIT_TEST_CFLAGS := $(CFLAGS_BASE) -I$(UNIT_TEST_SRC) -I$(UNITY_SRC)
DEBUG_CFLAGS := $(CFLAGS_BASE) -g3 -O0

UNIT_TEST_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNIT_TEST_SRC)/*.c))
UNITY_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNITY_SRC)/*.c))

.PHONY: clean run-tests

%: %.c

all: $(TEST_MAIN)

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(TEST_MAIN)

$(TEST_MAIN): CFLAGS = $(TEST_MAIN_CFLAGS)
$(TEST_MAIN): $(OBJ) $(TEST_MAIN_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)


$(UNIT_TEST_BIN): CFLAGS = $(UNIT_TEST_CFLAGS)
$(UNIT_TEST_BIN): $(OBJ) $(UNIT_TEST_OBJ) $(UNITY_OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

run-tests: $(UNIT_TEST_BIN)
	./$(UNIT_TEST_BIN)

clean:
	rm -f $(OBJ) $(UNITY_OBJ) $(UNIT_TEST_OBJ) $(TEST_MAIN_OBJ) \
		$(UNIT_TEST_BIN) $(TEST_MAIN)
