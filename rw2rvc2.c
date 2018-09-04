#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");
	printf("	li	a0, %d\n", atoi(argv[1]));
	printf("	ret\n");

	return 0;
}
