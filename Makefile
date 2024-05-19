.PHONY:	all opti clean re

CC		= gcc
RM		= rm -f
CFLAGS	= -Wall -Wextra -Werror -g -D ENABLE_BIN

NAME	= aledlang

all:
	@ echo "\e[90m[make] Compiling $(NAME)\e[0m"
	@ $(CC) $(CFLAGS) -o $(NAME) src/*.c

opti:
	@ echo "\e[90m[make] Compiling $(NAME) with optimization\e[0m"
	@ $(CC) $(CFLAGS) -O3 -o $(NAME) src/*.c

clean:
	$(RM) $(NAME)
