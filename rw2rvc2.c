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
typedef struct token_t {
	token_type_t type;
	int value;
	char *input;
} token_t;

/**
 * @brief types of nodes
 */
typedef enum {
	ND_PLUS,
	ND_MINUS,
	ND_NUM,
} node_type_t;

/**
 * @brief conversion table for token type to node type
 */
static node_type_t CONVERSION_TOKEN_TO_NODE[] = {
	[TK_PLUS]  = ND_PLUS,
	[TK_MINUS] = ND_MINUS,
	[TK_NUM]   = ND_NUM,
};

/**
 * @brief Node
 */
typedef struct node_t {
	node_type_t type;
	struct node_t *lhs;
	struct node_t *rhs;
	int value;
} node_t;

/**
 * @brief token array
 */
static struct token_t g_tokens[256];
static int g_index = 0;

/**
 * @brief recursive descendant parser
 */
static int g_position = 0;

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
 * @brief allocate memory for a node
 * @param[in] op   operation
 * @param[in] lhs  left hand side
 * @param[in] rhs  right hand side
 */
static struct node_t *new_node(node_type_t op, struct node_t *lhs, struct node_t *rhs)
{
	node_t *node = malloc(sizeof(struct node_t));

	node->type = op;
	node->lhs = lhs;
	node->rhs = rhs;

	return node;
}

/**
 * @brief allocate memory for a node of the number
 * @param[in] value  value of the number
 */
static struct node_t *new_node_num(int value)
{
	struct node_t *node = malloc(sizeof(struct node_t));

	node->type = ND_NUM;
	node->lhs = NULL;
	node->rhs = NULL;
	node->value = value;

	return node;
}

/**
 * @brief number
 */
static struct node_t *number(void)
{
	if (g_tokens[g_position].type == TK_NUM) {
		return new_node_num(g_tokens[g_position++].value);
	}

	error("expected number", g_position);
	/* NOTREACHED */
	return NULL;
}

/**
 * @brief expression
 */
static struct node_t *expr(void)
{
	struct node_t *lhs = number();

	for (;;) {
		token_type_t op = g_tokens[g_position].type;

		if (op != TK_PLUS && op != TK_MINUS)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, number());
	}

	if (g_tokens[g_position].type != TK_EOF)
		error("stray token", g_position);

	return lhs;
}

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
 * @brief main function
 */
int main(int argc, char **argv)
{
	int i = 0;
	struct node_t *node = NULL;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	/* tokenize */
	if (tokenize(argv[1]))
		return 1;

	node = expr();

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
