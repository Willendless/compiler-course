CFLAGS=-g -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=$(OPTLIBS) -ldl
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
DEST_OBJ=$(patsubst src/%,build/src/%,$(patsubst %.c,%.o,$(SOURCES)))

TEST_SRC=$(wildcard tests/*_tests.c)
TEST_OBJ=$(patsubst tests/%, build/tests/%, $(patsubst %.c,%.o,$(TEST_SRC)))
TESTS=$(patsubst %.c,%,$(TEST_SRC))

# specify build directory
TARGET=bin/toy.out
TEST_TARGET=build/libtest.a

all: build $(TARGET) $(TEST_TARGET) tests

$(TARGET): $(DEST_OBJ)
	$(CC) -o $@ $^

$(DEST_OBJ): $(OBJECTS)
	mv $(patsubst build/%.o,%.o,$@) $@

$(OBJECTS): CFLAGS += -c
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(patsubst %.o,%.c,$@)

# use -fPIC flag for target library
$(TEST_TARGET): CFLAGS += -fPIC
# prepare the directory first
# and then compile the objects
$(TEST_TARGET): $(DEST_OBJ)
	ar rcs $@ $(filter-out %main.o, $(DEST_OBJ))
	ranlib $@

build:
	@mkdir -p build/src/toy
	@mkdir -p build/tests
	@mkdir -p bin

.PHONY: tests
tests: $(TESTS)
	bash ./runtests.sh

$(TESTS): $(TEST_SRC) $(TEST_TARGET)
	$(CC) $(CFLAGS) $@.c $(TEST_TARGET) -o build/$@ $(LIBS)

# the cleaner
clean:
	rm -rf bin/ build/
	rm -f tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# the checker
# search for dangerous functions
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' \
	$(SOURCES) || true
