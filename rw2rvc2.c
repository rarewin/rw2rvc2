#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *p;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	p = argv[1];

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");

	printf("	li	a0, %ld\n", strtol(p, &p, 10));

	while (*p) {
		switch (*p++) {
		case '+':
			printf("	addi	a0, a0, %ld\n", strtol(p, &p, 10));
			continue;
		case '-':
			printf("	addi	a0, a0, %ld\n", -1 * strtol(p, &p, 10));
			continue;
		default:
			fprintf(stderr, "unexpected character: %c\n", *p);
			return 1;
		}
	}

	printf("	ret\n");

	return 0;
}
