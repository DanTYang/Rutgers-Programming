# Make sure the name of the rule and the output matches so that it only rebuilds if file changes.

all: Program1

Program1: Program1.c
	gcc -Werror -Wall -fsanitize=address Program1.c -o Program1

clean:
	rm -rf Program1