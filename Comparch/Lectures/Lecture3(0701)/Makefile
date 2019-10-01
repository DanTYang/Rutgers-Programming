all: Program1 Program2 Program4 Program5 Program6

Program1: Program1.c
	gcc -Werror -Wall -fsanitize=address Program1.c -o Program1

Program2: Program2.c
	gcc -Werror -Wall -fsanitize=address Program2.c -o Program2

Program4: Program4.c
	gcc -Werror -Wall -fsanitize=address Program4.c -o Program4

Program5: Program5.c
	gcc -Werror -Wall -fsanitize=address Program5.c -o Program5

Program6: Program6.c
	gcc -Werror -Wall -fsanitize=address Program6.c -o Program6

clean:
	rm -rf Program1 Program2 Program4 Program5 Program6