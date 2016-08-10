CC = gcc
TARGET = hexmage

all: src/main.c src/*.c
	$(CC) -o $(TARGET) src/*.c

clean:
	$(RM) $(TARGET)