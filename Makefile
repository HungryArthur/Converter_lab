CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
NAME = converter
SRC = main.c integer.c fraction.c input.c output.c cli.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

.PHONY: all clean re test check

all: $(NAME)$(EXE)

$(NAME)$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c converter.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(NAME) $(NAME).exe

re:
	$(MAKE) clean
	$(MAKE) all

test: all
	sh tests.sh

check:
	cppcheck --enable=all --error-exitcode=1 --std=c11 --check-level=exhaustive --suppress=missingIncludeSystem $(SRC)
