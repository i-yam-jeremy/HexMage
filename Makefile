CC = gcc
TARGET = hexmage

all: src/main.c src/hexmage.c src/lexer.c
	$(CC) -o $(TARGET) src/main.c src/hexmage.c src/lexer.c

clean:
	$(RM) $(TARGET)