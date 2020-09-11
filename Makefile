CC = gcc
BIN = bin
SRC = src

<<<<<<< HEAD
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
=======
FLAGS = -Wall -Wextra -g
VPATH = src:./$(SRC)

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(SOURCES))
TARGET = main.exe

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $^ -o $@ -lws2_32
>>>>>>> Added winServerReadFile & winServerCreateResponse

$(BIN)/%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@ -I"$(SRC)"

<<<<<<< HEAD
$(BIN):
	mkdir $(BIN)

clean:
	rm -r $(BIN)
	rm $(TARGET)
=======
clean:
	rm $(TARGET)
	rm $(BIN)/*.o
>>>>>>> Added winServerReadFile & winServerCreateResponse

.PHONY: clean
