#include <stdio.h>

#include "rw2rvc2.h"

static int g_position = 0;

/**
 * @brief 期待値(トークン)
 * @param[in] tokens  パースするトークンへのポインタ
 * @param[in] type    期待値
 */
static void expect_token(struct vector_t *tokens, token_type_t type)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == type) {
		g_position++;	/* 期待値通りならインデックスを進めて戻る. */
	} else {
		/* 期待値と異なった場合, 止まる. */
		error_printf("unexpect token: %s\n", t->input);
		exit(1);
	}
}

/**
 * @brief 新規ノードにメモリを割り当てる
 * @return 割り当てたメモリへのポインタ
 */
struct node_t *allocate_node(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct node_t *node_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	/* 初期割り当て */
	if (node_array == NULL)
		node_array = (struct node_t*)malloc(sizeof(struct node_t) * size);

	/* 割り当て領域のリサイズを行なう */
	if (index >= size) {
		size += ALLOCATE_SIZE;
		node_array = (struct node_t*)realloc(node_array, sizeof(struct node_t) * size);
	}

	return &node_array[index++];
}

/**
 * @brief 新規ノードをつくる
 * @param[in] op           ノードの種類
 * @param[in] lhs          左辺
 * @param[in] rhs          右辺
 * @param[in] expression   式
 * @param[in] statement    文 (Vector型)
 */
static struct node_t *new_node(node_type_t op,
			       struct node_t *lhs,
			       struct node_t *rhs,
			       struct node_t *expression,
			       struct vector_t *statements)
{
	node_t *node = allocate_node();

	node->type = op;
	node->lhs = lhs;
	node->rhs = rhs;
	node->expression = expression;
	node->statements = statements;

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
 * @brief 乗除算
 * @param[in] tokens
 * @return
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
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, number(tokens), NULL, NULL);
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
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, muldiv(tokens), NULL, NULL);
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
	struct node_t *e = NULL;

	if (t->type == TK_RETURN) {
		g_position++;
		e = new_node(ND_RETURN, NULL, NULL, expression(tokens), NULL);
	} else {
		return NULL;
	}

	return e;
}

/**
 * @brief jump_statementをパースする
 *
 * jump_statement := GOTO IDENTIFIER ';'
 *                 | CONTINUE ';'
 *                 | BREAK ';'
 *                 | RETURN ';'
 *                 | RETURN expression ';'
 */
static struct node_t *jump_statement(struct vector_t *tokens)
{
	struct node_t *n = NULL;

	if ((n = keyword_return(tokens)) != NULL) {
		;
	} else {
		;
	}

	return n;
}

/**
 * statement := labeled_statement
 *            | compound_statement
 *            | expression_statement
 *            | selection_statement
 *            | iteration_statement
 *            | jump_statement
 *            ;
 */
struct node_t *statement(struct vector_t *tokens)
{
	struct node_t *node = NULL;

	if ((node = jump_statement(tokens)) != NULL)
		return node;

	return expression(tokens);
}

/**
 * statement_list := statement
 *                 | statement_list statement
 *                ;
 */
struct node_t *statement_list(struct vector_t *tokens)
{
	struct node_t *n = NULL;
	struct node_t *s;

	do {
		s = statement(tokens);

		if (s != NULL) {
			if (n == NULL)
				n = new_node(ND_STATEMENT_LIST, NULL, NULL, NULL, new_vector());
			vector_push(n->statements, s);
		}

		expect_token(tokens, TK_SEMICOLON);
	} while (((struct token_t *)(tokens->data[g_position]))->type != TK_EOF);

	return n;
}

/**
 * @brief main function of parser
 */
struct node_t *parse(struct vector_t *tokens)
{
	return statement_list(tokens);
}
