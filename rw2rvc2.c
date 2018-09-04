#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief types of tokens
 */
typedef enum {
	TK_PLUS,
	TK_MINUS,
	TK_NUM,
	TK_EOF,
} token_type_t;

/**
 * @brief Token type
 */
typedef struct {
	token_type_t type;
	int value;
	char *input;
} token_t;

/**
 * @brief token array
 */
token_t g_tokens[256];
int g_index = 0;

/**
 * @brief tokenizer
 */
static int tokenize(char *p)
{
	while (*p) {
		/* ignore spaces */
		if (isspace(*p)) {
			p++;
			continue;
		}

		/* arithmetic symbols */
		if (*p == '+' || *p == '-') {
			switch (*p) {
			case '+':
				g_tokens[g_index].type = TK_PLUS;
				break;
			case '-':
				g_tokens[g_index].type =  TK_MINUS;
				break;
			default:
				break;
			}
			g_tokens[g_index].value = 0;	/* not used */
			g_tokens[g_index].input = p;

			g_index++;
			p++;

			continue;
		}

		/* number */
		if (isdigit(*p)) {
			g_tokens[g_index].type = TK_NUM;
			g_tokens[g_index].input = p;
			g_tokens[g_index].value = strtol(p, &p, 10);
			g_index++;

			continue;
		}

		fprintf(stderr, "tokenize error: %s", p);
		return -1;
	}

	g_tokens[g_index].type = TK_EOF;
	g_tokens[g_index].input = NULL;
	g_tokens[g_index].value = 0;

	return 0;
}

/**
 * @brief error with token
 * @param[in] message   a message to be printed out
 * @param[in] position  position of the token where the error occurred
 */
static void error(char *message, int position)
{
	fprintf(stderr, "%s: %s\n", message, g_tokens[position].input);
	exit(1);
}

/**
 * @brief main function
 */
int main(int argc, char **argv)
{
	int i = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	/* tokenize */
	if (tokenize(argv[1]))
		return 1;

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");

	/* error if first token is not a number */
	if (g_tokens[i].type != TK_NUM){
		error("unexpected token", i);
		return 1;
	}

	printf("	li	a0, %d\n", g_tokens[i].value);
	i++;

	while (g_tokens[i].type != TK_EOF) {

		switch (g_tokens[i].type) {
		case TK_PLUS:
			i++;
			if (g_tokens[i].type != TK_NUM)
				error("unexpected token", i);
			printf("	addi	a0, a0, %d\n", g_tokens[i].value);
			i++;
			continue;
		case TK_MINUS:
			i++;
			if (g_tokens[i].type != TK_NUM)
				error("unexpected token", i);
			printf("	addi	a0, a0, %d\n", -1 * g_tokens[i].value);
			i++;
			continue;
		default:
			error("unexpected token", i);
		}
	}

	printf("	ret\n");

	return 0;
}
