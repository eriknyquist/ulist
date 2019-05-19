OBJ := $(patsubst %.c,%.o,$(wildcard src/*.c))
TEST_MAIN_OBJ += test/main.o

TEST_MAIN := test_main
UNIT_TEST_BIN := unit_test_runner
UNITY_SRC := unity
UNIT_TEST_SRC := test/unit_tests

CFLAGS := -Wall -Itest -Isrc
UNIT_TEST_CFLAGS := -Wall -Isrc -I$(UNIT_TEST_SRC) -I$(UNITY_SRC)
UNIT_TEST_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNIT_TEST_SRC)/*.c))
UNITY_OBJ := $(patsubst %.c,%.o,$(wildcard $(UNITY_SRC)/*.c))

.PHONY: clean run-tests

%: %.c

debug: CFLAGS += -g3 -O0
debug: $(TEST_MAIN)

all: $(TEST_MAIN)

$(TEST_MAIN): $(OBJ) $(TEST_MAIN_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(UNIT_TEST_BIN): CFLAGS = $(UNIT_TEST_CFLAGS)

$(UNIT_TEST_BIN): $(OBJ) $(UNIT_TEST_OBJ) $(UNITY_OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

run-tests: $(UNIT_TEST_BIN)
	./$(UNIT_TEST_BIN)

clean:
	rm -f $(OBJ)
	rm -f $(UNITY_OBJ)
	rm -f $(UNIT_TEST_OBJ)
	rm -f $(TEST_MAIN_OBJ)
	rm -f $(UNIT_TEST_BIN)
	rm -f $(TEST_MAIN)
