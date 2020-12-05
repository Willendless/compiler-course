CFLAGS=-g -Wall -Wextra -Isrc -rdynamic $(OPTFLAGS)
LIBS=$(OPTLIBS) -ldl
PREFIX?=/usr/local

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=llgen

all: $(TARGET)

$(TARGET): $(OBJECTS)

$(OBJECTS): CFLAGS += -c
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(patsubst %.o,%.c,$@)

# the cleaner
clean:
	rm -rf ./*.o llgen
