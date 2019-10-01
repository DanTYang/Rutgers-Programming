# Make sure the name of the rule and the output matches so that it only rebuilds if file changes.

all: Program1 Program2 Program3 Program3a Program4 Program5 Program6 Program7 Program8 Program9

Program1: Program1.c
	gcc -Werror -Wall -fsanitize=address Program1.c -o Program1

Program2: Program2.c
	gcc -Werror -Wall -fsanitize=address Program2.c -o Program2

Program3: Program3.c
	gcc -Werror -Wall -fsanitize=address Program3.c -o Program3

Program3a: Program3a.c
	gcc -Werror -Wall -fsanitize=address Program3a.c -o Program3a

Program4: Program4.c
	gcc -Werror -Wall -fsanitize=address Program4.c -o Program4

Program5: Program5.c
	gcc -Werror -Wall -fsanitize=address Program5.c -o Program5

Program6: Program6.c
	gcc -Werror -Wall -fsanitize=address Program6.c -o Program6

Program7: Program7.c
	gcc -Werror -Wall -fsanitize=address Program7.c -o Program7

Program8: Program8.c
	gcc -Werror -Wall -fsanitize=address Program8.c -o Program8

Program9: Program9.c
	gcc -Werror -Wall -fsanitize=address Program9.c -o Program9

clean:
	rm -rf Program1 Program2 Program3 Program4 Program5 Program6 Program7 Program8 Program9