CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -I./include -O2
LDFLAGS = -lm

SRC = src/main.c src/common.c src/lexer.c src/parser.c src/ast.c src/types.c src/semantic.c src/diagnostics.c src/codegen.c src/compiler.c src/util.c
RUNTIME_SRC = runtime/white_runtime.c
OBJ = $(SRC:.c=.o) $(RUNTIME_SRC:.c=.o)
BIN = build/white

all: $(BIN)

$(BIN): $(OBJ) | build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build:
	mkdir -p build

clean:
	rm -rf $(OBJ) $(BIN) build/

test: $(BIN)
	./$(BIN) check examples/hello.white

verbose: CFLAGS += -g -DDEBUG
verbose: all

.PHONY: all clean test verbose
