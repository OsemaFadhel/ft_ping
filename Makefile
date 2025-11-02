NAME = ft_ping

SRCS = $(shell find srcs -name '*.c')

OBJS = $(SRCS:%.c=objs/%.o)

FLAGS = -Wall -Wextra -Werror

CC = gcc

objs/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME) -lm

clean:
	rm -rf  $(NAME) $(OBJS)

fclean: clean
	rm -rf objs

re: fclean all

.PHONY: all clean fclean re
