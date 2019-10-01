all: first

first: first.c
	gcc -std=c99 -Werror -Wall -fsanitize=address first.c -o first

clean:
	rm -rf first
