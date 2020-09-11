CC = gcc
BIN = bin
SRC = src

FLAGS = -Wall -Wextra
VPATH = src:./$(SRC)

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/*.c, $(BIN)/*.o, $(SOURCES))
TARGET = main.exe

all: $(TARGET) $(BIN)

debug: FLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $^ -o $@

$(BIN)/%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@ -I"$(SRC)"

$(BIN):
	mkdir $(BIN)

clean:
	rm -r $(BIN)
	rm $(TARGET)

.PHONY: clean
