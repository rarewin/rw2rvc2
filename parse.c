#include <stdio.h>

#include "rw2rvc2.h"

static int g_position = 0;

static void expect_token(struct vector_t *tokens, token_type_t type)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == type) {
		g_position++;
	} else {
		fprintf(stderr, "unexpect token\n");
		exit(1);
	}
}

/**
 * @brief allocate memory to a new node
 */
struct node_t *allocate_node(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct node_t *node_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	/* initial allocate */
	if (node_array == NULL)
		node_array = (struct node_t*)malloc(sizeof(struct node_t) * size);

	/* reallocate */
	if (index >= size) {
		size += ALLOCATE_SIZE;
		node_array = (struct node_t*)realloc(node_array, sizeof(struct node_t) * size);
	}

	return &node_array[index++];
}

/**
 * @brief allocate memory for a node
 * @param[in] op   operation
 * @param[in] lhs  left hand side
 * @param[in] rhs  right hand side
 */
static struct node_t *new_node(node_type_t op, struct node_t *lhs, struct node_t *rhs)
{
	node_t *node = allocate_node();

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
	struct node_t *node = allocate_node();

	node->type = ND_NUM;
	node->lhs = NULL;
	node->rhs = NULL;
	node->value = value;

	return node;
}

/**
 * @brief conversion table for token type to node type
 */
static node_type_t CONVERSION_TOKEN_TO_NODE[] = {
	[TK_PLUS]  = ND_PLUS,
	[TK_MINUS] = ND_MINUS,
	[TK_MUL]   = ND_MUL,
	[TK_DIV]   = ND_DIV,
	[TK_NUM]   = ND_NUM,
	[TK_RETURN] = ND_RETURN,
};

/**
 * @brief number
 */
static struct node_t *number(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_NUM) {
		g_position++;
		return new_node_num(t->value);
	}

	return NULL;
}

/**
 * @brief multiplication
 */
static struct node_t *muldiv(struct vector_t *tokens)
{
	struct node_t *lhs = number(tokens);

	for (;;) {
		struct token_t *t = tokens->data[g_position];
		token_type_t op = t->type;

		if (op != TK_MUL && op != TK_DIV)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, number(tokens));
	}

	return lhs;
}

/**
 * @brief expression
 * @param[in]  tokens  vector for tokens
 */
static struct node_t *expression(struct vector_t *tokens)
{
	struct node_t *lhs = muldiv(tokens);

	for (;;) {
		struct token_t *t = tokens->data[g_position];
		token_type_t op = t->type;

		if (op != TK_PLUS && op != TK_MINUS)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, muldiv(tokens));
	}

	return lhs;
}

/**
 * @brief "return"
 * @param[in] tokens  vector for tokens
 * @return
 */
static struct node_t *keyword_return(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];
	struct node_t *lhs = NULL;

	if (t->type == TK_RETURN) {
		g_position++;
		lhs = new_node(ND_RETURN, expression(tokens), NULL);
	} else {
		fprintf(stderr, "no return keywords\n");
		exit(1);
	}

	expect_token(tokens, TK_SEMICOLON);

	return lhs;
}

/**
 * jump_statement := GOTO IDENTIFIER ';'
 *                 | CONTINUE ';'
 *                 | BREAK ';'
 *                 | RETURN ';'
 *                 | RETURN expression ';'
 */
static struct node_t *jump_statement(struct vector_t *tokens)
{
	struct node_t *lhs = keyword_return(tokens);

	return lhs;
}

/**
 * @brief main function of parser
 */
struct node_t *parse(struct vector_t *tokens)
{
	return jump_statement(tokens);
}
