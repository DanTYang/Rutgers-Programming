/*
    Struct examples
 */
#include <stdlib.h>
#include <stdio.h>


struct MyStruct {
    int num1;
    int num2;
    int num3;
    char a;
    char b;
    char c;
};

void ExerciseOne() {
    struct MyStruct s;
    s.num1 = 2;
    s.num2 = 3;
    s.num3 = 25;
    s.a = 'C';
    s.b = 'A';
    s.c = 'T';
    
    printf("%d, %d, %d, %c, %c, %c\n", s.num1, s.num2, s.num3, s.a, s.b, s.c);
    return;
}

void ExerciseOnePointOne() {
    struct MyStruct *s = (struct MyStruct *)malloc(sizeof(struct MyStruct));
    s->num1 = 2;
    s->num2 = 3;
    s->num3 = 25;
    s->a = 'C';
    s->b = 'A';
    s->c = 'T';
    
    printf("%d, %d, %d, %c, %c, %c\n", s->num1, s->num2, s->num3, s->a, s->b, s->c);
    return;
}

// Only to showcase that Struct packs all the data as close as possible.
struct StringStruct {
    char a;
    char b;
    char c;
    char d;
    char e;
    char f;
    char g;
    char h;
    char i;
    char j;
};

void ExerciseTwo() {
    struct StringStruct ss;
    ss.a = 'H';
    ss.b = 'e';
    ss.c = 'l';
    ss.d = 'l';
    ss.e = 'o';
    ss.f = ' ';
    ss.g = 'J';
    ss.h = 'a';
    ss.i = 'y';
    ss.j = '\0';
    
    printf("%s\n", (char*)&ss);
}

// Size of Struct can be different for differently organized structs
struct OneStruct {
    int * p;
    int num;
    char s;
};

struct TwoStruct {
    char s;
    int * p;
    int num;
};

struct ThreeStruct {
    char s;
    char s2;
    char s3;
};

struct FourStruct {
    int * p1;
    int * p2;
    int * p3;
};

struct FiveStruct {
    char s1;
    int * p1;
    char s2;
    int * p2;
    char s3;
    int * p3;
};

void ExerciseThree() {
    printf("Size of OneStruct: %lu\n", sizeof(struct OneStruct));
    printf("Size of TwoSTruct: %lu\n", sizeof(struct TwoStruct));
    printf("Size of TwoSTruct: %lu\n", sizeof(struct ThreeStruct));
    printf("Size of TwoSTruct: %lu\n", sizeof(struct FourStruct));
    printf("Size of TwoSTruct: %lu\n", sizeof(struct FiveStruct));
    return;
}

int main(int argc, char** argv) {
    return 0;
}
