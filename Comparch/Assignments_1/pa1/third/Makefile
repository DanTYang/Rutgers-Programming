all: third

third: third.c
	gcc -std=c99 -Werror -Wall -fsanitize=address third.c -o third

clean:
	rm -rf third
