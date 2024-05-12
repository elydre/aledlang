.PHONY:	all opti clean re

CC		= gcc
RM		= rm -f
CFLAGS	= -Wall -Wextra -Werror -g

NAME	= aledlang

all:
	$(CC) $(CFLAGS) -o $(NAME) src/*.c

opti:
	$(CC) $(CFLAGS) -O3 -o $(NAME) src/*.c

clean:
	$(RM) $(NAME)
