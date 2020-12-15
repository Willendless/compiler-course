CFLAGS=-g -Wall -Wextra -Isrc -rdynamic $(OPTFLAGS)
LIBS=$(OPTLIBS) -ldl
PREFIX?=/usr/local

SOURCES=$(wildcard *.c ../lib/*.c ../utils/utils.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=llgen

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mv ../lib/*.o .
	mv ../utils/*.o .
	$(CC) $(CFLAGS) -o $@ *.o

$(OBJECTS): CFLAGS += -c
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(patsubst %.o,%.c,$@)

# the cleaner
clean:
	rm -rf ./*.o llgen
