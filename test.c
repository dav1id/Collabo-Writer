#include <stdio.h>
#include <string.h>
int main() {
	char* comp_one = "Hello, World!";
	comp_one = "Hello, World with a memory leak to spice it up!";
	printf("%d\n", (int) sizeof(comp_one));
	printf("%s\n", comp_one);
}

