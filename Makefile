CC = gcc
RM = rm
ECHO = echo
TARGET = hexmage

all: src/*.c src/resources.h
	@$(ECHO) "[Building...]"
	@$(CC) -o $(TARGET) src/*.c
	@$(ECHO) "[Done building.]"
	@$(ECHO) "[Deleting packaged resources...]"
	@$(RM) src/resources.h
	@$(ECHO) "[Done deleting packaged resources.]"
	@$(ECHO) "[Done.]"

src/resources.h:
	@$(ECHO) "[Packaging resources...]"
	@for resource in resources/*; \
	do \
		$(ECHO) `xxd -i $$resource` >> src/resources.h; \
	done
	@$(ECHO) "[Done packaging resources.]"
	

clean:
	$(RM) $(TARGET)