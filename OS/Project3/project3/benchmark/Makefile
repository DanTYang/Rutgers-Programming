
all : test
test: ../my_vm.h
	gcc test.c -L../ -lmy_vm -lm -m32 -lpthread -o test

clean:
	rm -rf test
