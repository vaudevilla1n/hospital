CFLAGS := -Wall -Wextra -Wpedantic -std=c99 -g3
LDFLAGS := -lcurses

BIN := ./hospital_management
SRC := $(wildcard src/*.c)
INC := $(wildcard src/*.h)

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC) $(INC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(BIN)

clean:
	rm -f $(BIN)
