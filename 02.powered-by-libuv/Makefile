PROGRAM:=a.out
.DEFAULT_GOAL:=$(PROGRAM)

LIBUV_ROOT:=../deps/libuv
LIBUV_LIB_FILE:=$(LIBUV_ROOT)/build/libuv_a.a
SRC_FILES:=$(shell find . -type f \( -name '*.c' -o -name '*.h' \))
INCLUDE:=$(LIBUV_ROOT)/include

# Deps

$(LIBUV_ROOT):
	git submodule update --init --recursive
	touch $(LIBUV_ROOT)

$(LIBUV_LIB_FILE): $(LIBUV_ROOT)
	cd $(LIBUV_ROOT) && mkdir -p build && cd ./build && cmake .. && cd .. && cmake --build build

$(PROGRAM): $(SRC_FILES) $(LIBUV_LIB_FILE)
	gcc $(INCLUDE:%=-I%) -Wall -g3 -o $(PROGRAM) $(SRC_FILES) $(LIBUV_LIB_FILE)

# Tasks

.PHONY: gdb
gdb: $(PROGRAM)
	gdb ./$(PROGRAM)

.PHONY: run
run: $(PROGRAM)
	./$(PROGRAM)

.PHONY: clean
clean:
	rm -fr $(PROGRAM) $(LIBUV_LIB_FILE)
