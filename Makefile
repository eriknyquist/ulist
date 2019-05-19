OBJ := $(patsubst %.c,%.o,$(wildcard *.c))
PROGNAME := ulist

CFLAGS := -Wall

.PHONY: clean

%: %.c

debug: CFLAGS += -g3 -O0
debug: $(PROGNAME)

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(PROGNAME)
