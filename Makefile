CC      = gcc
CFLAGS  = -Wall -Wextra -I./include -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

SRC     = src/main.c src/lista.c src/logica.c
TARGET  = baque-virado

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
