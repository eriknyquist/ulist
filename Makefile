OBJ= $(patsubst %.c,%.o,$(wildcard *.c))
PROGNAME=ulist

.PHONY: clean

%: %.c

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(PROGNAME)
