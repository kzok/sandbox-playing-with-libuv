PROGRAM:=a.out
SRC_FILES:=$(shell find . -type f -name '*.c')
.DEFAULT_GOAL:=$(PROGRAM)

# Deps

$(PROGRAM): $(SRC_FILES)
	gcc -Wall -O2 -o $(PROGRAM) $(SRC_FILES)

# Tasks

.PHONY: run
run: $(PROGRAM)
	./$(PROGRAM)

.PHONY: clean
clean:
	rm -fr $(PROGRAM)
