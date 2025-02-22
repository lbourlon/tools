LIBS    = -lm -lc
CC		= gcc
CFLAGS	=  -O3 -Wall -Wextra -g

OPTI_FLAGS		= -march=native -mavx2 -mfpmath=sse -msse4 -fopt-info-vec-all=gcc.optrpt - 03

SRC		= src
SRCS	= $(wildcard $(SRC)/*.c)
INLHEADERS = $(wildcard $(SRC)/*.h)

TEST		= tests
TESTS		= $(wildcard $(TEST)/*.c)
TESTBINS	= $(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(TESTS))

messiah: $(SRCS) $(INLHEADERS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./messiah

# Compiles tests
$(TEST)/bin/%: $(TEST)/%.c 
	$(CC) $< $(SRCS_MINUS_MAIN) -o $@ -lcriterion  $(LIBS)

# Runs tests, 
test: clean_tests $(TESTBINS)
	@echo "Messiah Tests"
	for test in $(TESTBINS) ; do ./$$test ; done

clean_tests: 
	rm -f $(TEST)/bin/* 2>/dev/null

clean: clean_tests
	rm -f messiah

.PHONY: messiah clean test $(TEST)/bin/%
