#include <stdio.h>
#include <string.h>

int main() {
	char string_one[64] = "Hello World with a little memory leak!";
	char string_two[64];
	memcpy(string_two, string_one, 8); // why is it printing
	printf("%*.s\n", (int) strlen(string_two), string_two);
}