# ==== CONFIGURATION ====
CC = gcc
CFLAGS = -Wall -std=c99 -Isource -Iinclude
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lGL -lX11

# ==== AUTO-DETECT FILES (RECURSIVE) ====
SRC = $(shell find source -name "*.c")
OBJ = $(SRC:.c=.o)
TARGET = game

# ==== RULES ====
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
