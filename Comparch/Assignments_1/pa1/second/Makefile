all: second

second: second.c
	gcc -std=c99 -Werror -Wall -fsanitize=address second.c -o second

clean:
	rm -rf second
