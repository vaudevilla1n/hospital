CFLAGS := -Wall -Wextra -Wpedantic -std=c99 -g3
LDFLAGS := -lcurses

BIN := ./hospital_management

.PHONY: all clean

all: $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BIN)
