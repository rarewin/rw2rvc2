#include <stdio.h>
#include <stdbool.h>

#include "rw2rvc2.h"

/* static関数のプロトタイプ宣言. (循環コールのため) */
static struct node_t *expression(struct vector_t *tokens);
static struct node_t *statement(struct vector_t *tokens);
static struct node_t *statement_list(struct vector_t *tokens);
static struct node_t *declarator(struct vector_t *tokens);
static struct node_t *declaration_specifiers(struct vector_t *tokens);
static struct node_t *assignment_expression(struct vector_t *tokens);

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
		error_printf("unexpect token: %s at %d\n", t->input, g_position);
		error_printf("expect token: %s (%d)\n", get_token_str(type), type);
		exit(1);
	}
}

/**
 * @brief 消費(トークン)
 * @param[in] tokens  パースするトークンへのポインタ
 * @param[in] type    期待値
 */
static void consume_token(struct vector_t *tokens, token_type_t type)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == type)
		g_position++;	/* 期待値通りならインデックスを進めて戻る. */
}

/**
 * @brief パースエラー
 */
static void parse_error(void)
{
	error_printf("parse error at index %d\n", g_position);
	exit(1);
}

/**
 * @brief 新規ノードにメモリを割り当てる
 * @return 割り当てたメモリへのポインタ
 */
static struct node_t *allocate_node(void)
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
 * @param[in] name         名前
 */
static struct node_t *new_node(node_type_t op,
			       struct node_t *lhs,
			       struct node_t *rhs,
			       char *name,
			       int value)
{
	node_t *node = allocate_node();

	node->type = op;
	node->lhs = lhs;
	node->rhs = rhs;
	node->name = name;
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
	[TK_MOD]   = ND_MOD,
	[TK_NUM]   = ND_CONST,
	[TK_RETURN] = ND_RETURN,
};

/**
 * @brief primary expression
 *
 * primary_expression := IDENTIFIER
 *                     | CONSTANT
 *                     | STRING_LITERAL
 *                     | '(' expression ')'
 *                     ;
 */
static struct node_t *primary_expression(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];
	struct node_t *node;

	if (t->type == TK_LEFT_PAREN) {
		g_position++;
		node = expression(tokens);
		consume_token(tokens, TK_RIGHT_PAREN);
		return node;
	}

	if (t->type == TK_NUM) {
		g_position++;
		return new_node(ND_CONST, NULL, NULL, NULL, t->value);
	}

	if (t->type == TK_IDENT) {
		g_position++;
		return new_node(ND_IDENT, NULL, NULL, t->name, -1);
	}

	return NULL;
}


/**
 * @brief argument_expression_list
 *
 * argument_expression_list := assignment_expression
 *                           | argument_expression_list ',' assignment_expression
 *                           ;
 */
static struct node_t *argument_expression_list(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct node_t *n1, *n2;

	if ((lhs = assignment_expression(tokens)) ==NULL)
		return NULL;

	lhs = new_node(ND_FUNC_ARG, lhs, NULL, NULL, -1);
	n1 = lhs;

	while (((struct token_t*)tokens->data[g_position])->type == TK_COMMA) {
		consume_token(tokens, TK_COMMA);
		n2 = new_node(ND_FUNC_ARG, assignment_expression(tokens), NULL, NULL, -1);
		n1->rhs = n2;
		n1 = n2;
	}

	return lhs;
}

/**
 * @brief postfix_expression
 *
 * postfix_expression := primary_expression
 *                     | postfix_expression '[' expression ']'
 *                     | postfix_expression '(' ')'
 *                     | postfix_expression '(' argument_expression_list ')'
 *                     | postfix_expression '.' IDENTIFIER
 *                     | postfix_expression PTR_OP IDENTIFIER
 *                     | postfix_expression INC_OP
 *                     | postfix_expression DEC_OP
 *                     ;
 */
static struct node_t *postfix_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = primary_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type == TK_LEFT_PAREN) {
			consume_token(tokens, TK_LEFT_PAREN);

			if (t->type == TK_RIGHT_PAREN) {
				consume_token(tokens, TK_RIGHT_PAREN);
				lhs = new_node(ND_FUNC_CALL, lhs, NULL, lhs->name, -1);
			} else {
				lhs = new_node(ND_FUNC_CALL, lhs, argument_expression_list(tokens), lhs->name, -1);
				expect_token(tokens, TK_RIGHT_PAREN);
			}
			continue;
		}

		break;
	}

	return lhs;
}

/**
 * @brief unary_expression
 *
 * unary_expression := postfix_expression
 *                   | INC_OP unary_expression
 *                   | DEC_OP unary_expression
 *                   | unary_operator cast_expression
 *                   | SIZEOF unary_expression
 *                   | SIZEOF '(' type_name ')'
 *                   ;
 */
static struct node_t *unary_expression(struct vector_t *tokens)
{
	return postfix_expression(tokens);
}

/**
 * @brief 乗除算
 * @param[in] tokens
 * @return
 *
 * multiplicative_expression := unary_expression
 *                           | multiplicative_expression '*' unary_expression
 *                           | multiplicative_expression '/' unary_expression
 *                           | multiplicative_expression '%' unary_expression
 *                           ;
 */
static struct node_t *multiplicative_expression(struct vector_t *tokens)
{
	struct node_t *lhs;

	if ((lhs = unary_expression(tokens)) == NULL)
		return NULL;

	for (;;) {
		struct token_t *t = tokens->data[g_position];
		token_type_t op = t->type;

		if (op != TK_MUL && op != TK_DIV && op != TK_MOD)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, unary_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief additive_expression
 *
 * additive_expression := multiplicative_expression
 *                      | additive_expression '+' multiplicative_expression
 *                      | additive_expression '-' multiplicative_expression
 *                      ;
 */
static struct node_t *additive_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	if ((lhs = multiplicative_expression(tokens)) == NULL)
		return NULL;

	for (;;) {
		t = tokens->data[g_position];
		token_type_t op = t->type;

		if (op != TK_PLUS && op != TK_MINUS)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, multiplicative_expression(tokens), NULL, -1);
	}

	return lhs;
}

static struct node_t *identifier(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_IDENT) {
		g_position++;
		return new_node(ND_IDENT, NULL, NULL, t->name, -1);
	}

	return NULL;
}

/**
 * @brief shift_expression
 *
 * shift_expression := additive_expression
 *                   | shift_expression LEFT_OP additive_expression
 *                   | shift_expression RIGHT_OP additive_expression
 *                   ;
 */
static struct node_t *shift_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;
	int nd_type;

	lhs = additive_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_RIGHT_OP && t->type != TK_LEFT_OP)
			break;

		nd_type = (t->type == TK_RIGHT_OP) ? ND_RIGHT_OP : ND_LEFT_OP;
		consume_token(tokens, t->type);
		lhs = new_node(nd_type, lhs, additive_expression(tokens), NULL, -1);
	}

	return lhs;
}


/**
 * @brief relational_expression
 *
 * relational_expression := shift_expression
 *                        | relational_expression '<' shift_expression
 *                        | relational_expression '>' shift_expression
 *                        | relational_expression LE_OP shift_expression
 *                        | relational_expression GE_OP shift_expression
 *                        ;
 */
static struct node_t *relational_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;
	int nd_type;

	lhs = shift_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_LESS_OP && t->type != TK_GREATER_OP &&
		    t->type != TK_LE_OP && t->type != TK_GE_OP)
			break;

		if (t->type == TK_LESS_OP)
			nd_type = ND_LESS_OP;
		else if (t->type == TK_GREATER_OP)
			nd_type = ND_GREATER_OP;
		else if (t->type == TK_LE_OP)
			nd_type = ND_LE_OP;
		else
			nd_type = ND_GE_OP;

		consume_token(tokens, t->type);
		lhs = new_node(nd_type, lhs, shift_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief equality_expression
 *
 * equality_expression := relational_expression
 *                      | equality_expression EQ_OP relational_expression
 *                      | equality_expression NE_OP relational_expression
 *                      ;
 */
static struct node_t *equality_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;
	int nd_type;

	lhs = relational_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_EQ_OP && t->type != TK_NE_OP)
			break;

		consume_token(tokens, t->type);

		nd_type = (t->type == TK_EQ_OP) ? ND_EQ_OP : ND_NE_OP;
		lhs = new_node(nd_type, lhs, relational_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief and_expression
 *
 * and_expression := equality_expression
 *                 | and_expression '&' equality_expression
 *                 ;
 */
static struct node_t *and_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = equality_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_AND)
			break;

		consume_token(tokens, TK_AND);
		lhs = new_node(ND_AND, lhs, equality_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief exclusive_or_expression
 *
 * exclusive_or_expression := and_expression
 *                          | exclusive_or_expression '^' and_expression
 *                          ;
 */
static struct node_t *exclusive_or_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = and_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_XOR)
			break;

		consume_token(tokens, TK_XOR);
		lhs = new_node(ND_XOR, lhs, and_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief inclusive_or_expression
 *
 * inclusive_or_expression := exclusive_or_expression
 *                          | inclusive_or_expression '|' exclusive_or_expression
 *                          ;
 */
static struct node_t *inclusive_or_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = exclusive_or_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_OR)
			break;

		consume_token(tokens, TK_OR);
		lhs = new_node(ND_OR, lhs, exclusive_or_expression(tokens), NULL, -1);
	}

	return lhs;
}


/**
 * @brief logical_and_expression
 *
 * logical_and_expression := inclusive_or_expression
 *                         | logical_and_expression AND_OP inclusive_or_expression
 *                         ;
 */
static struct node_t *logical_and_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = inclusive_or_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_AND_OP)
			break;

		consume_token(tokens, TK_AND_OP);
		lhs = new_node(ND_AND_OP, lhs, inclusive_or_expression(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief logical_or_expression
 *
 * logical_or_expression := logical_and_expression
 *                        | logical_or_expression OR_OP logical_and_expression
 *                        ;
 */
static struct node_t *logical_or_expression(struct vector_t *tokens)
{
	struct node_t *lhs;
	struct token_t *t;

	lhs = logical_and_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		if (t->type != TK_OR_OP)
			break;

		consume_token(tokens, TK_OR_OP);
		lhs = new_node(ND_OR_OP, lhs, logical_and_expression(tokens), NULL, -1);
	}

	return lhs;
}


/**
 * @brief conditional_expression
 *
 * conditional_expression := logical_or_expression
 *                         | logical_or_expression '?' expression ':' conditional_expression
 *                         ;
 */
static struct node_t *conditional_expression(struct vector_t *tokens)
{
	return logical_or_expression(tokens);
}


/**
 * @brief assignment_operatorかどうか判断する
 * @param[in]  type  トークンタイプ
 * @return   true or false
 */
static inline bool is_assignment_operator(token_type_t type)
{
	return ((type == TK_EQUAL || type == TK_MUL_ASSIGN ||
		 type == TK_DIV_ASSIGN || type == TK_MOD_ASSIGN ||
		 type == TK_ADD_ASSIGN || type == TK_SUB_ASSIGN)) ? true : false;
}

/**
 * @brief assignment expression
 * assignment_expression := conditional_expression
 *                        | unary_expression assignment_operator assignment_expression
 *                        ;
 */
static struct node_t *assignment_expression(struct vector_t *tokens)
{
	struct node_t *lhs, *rhs;
	struct token_t *t;
	int pos = g_position;

	if ((lhs = unary_expression(tokens)) != NULL) {

		t = tokens->data[g_position];

		/* assgienment operator */
		if (!is_assignment_operator(t->type)) {
			g_position = pos;
			lhs = NULL;
		} else {
			consume_token(tokens, t->type);

			if (t->type == TK_MUL_ASSIGN) {
				rhs = new_node(ND_MUL, lhs, expression(tokens), NULL, -1);
			} else if (t->type == TK_DIV_ASSIGN) {
				rhs = new_node(ND_DIV, lhs, expression(tokens), NULL, -1);
			} else if (t->type == TK_MOD_ASSIGN) {
				rhs = new_node(ND_MOD, lhs, expression(tokens), NULL, -1);
			} else if (t->type == TK_ADD_ASSIGN) {
				rhs = new_node(ND_PLUS, lhs, expression(tokens), NULL, -1);
			} else if (t->type == TK_SUB_ASSIGN) {
				rhs = new_node(ND_MINUS, lhs, expression(tokens), NULL, -1);
			} else {
				rhs = expression(tokens);
			}

			lhs = new_node(ND_ASSIGN, lhs, rhs, NULL, -1);
		}
	}

	if (lhs == NULL)
		lhs = conditional_expression(tokens);

	return lhs;
}

/**
 * @brief expression
 * @param[in]  tokens  vector for tokens
 *
 * expression := assignment_expression
 *             | expression ',' assignment_expression
 *             ;
 */
static struct node_t *expression(struct vector_t *tokens)
{
	struct node_t *lhs;

	if ((lhs = assignment_expression(tokens)) == NULL)
		return NULL;

	lhs = new_node(ND_EXPRESSION, lhs, NULL, NULL, -1);

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
		e = new_node(ND_RETURN, expression(tokens), NULL, NULL, -1);
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
		expect_token(tokens, TK_SEMICOLON);
	} else {
		;
	}

	return n;
}

/**
 * @brief selection_statementをパースする
 *
 * selection_statement := IF '(' expression ')' statement
 *                      | IF '(' expression ')' statement ELSE statement
 *                      | SWITCH '(' expression ')' statement
 *                      ;
 */
static struct node_t *selection_statement(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];
	struct node_t *node = NULL;

	if (t->type == TK_IF) {
		g_position++;
		expect_token(tokens, TK_LEFT_PAREN);
		node = new_node(ND_IF, expression(tokens), NULL, NULL, -1);
		expect_token(tokens, TK_RIGHT_PAREN);

		node->rhs = new_node(ND_THEN_ELSE, statement(tokens), NULL, NULL, -1);

		if (node->rhs == NULL) {
			parse_error();
			/* NOTREACHED */
		}

		t = tokens->data[g_position];
		if (t->type == TK_ELSE) {
			g_position++;
			if ((node->rhs->rhs = statement(tokens)) == NULL) {
				parse_error();
				/* NOTREACHED */
			}
		}
	}

	return node;
}

/**
 * @brief expression_statementをパースする
 * expression_statement := ';'
 *                       | expression ';'
 *                       ;
 */
static struct node_t *expression_statement(struct vector_t *tokens)
{
	struct node_t *node = NULL;
	struct token_t *t = tokens->data[g_position];

	if ((node = expression(tokens)) != NULL)
		expect_token(tokens, TK_SEMICOLON);
	else if (t->type == TK_SEMICOLON)
		consume_token(tokens, TK_SEMICOLON);

	return node;
}

/**
 * @brief compound_statementをパースする
 *
 * compound_statement := '{' '}'
 *                     | '{' statement_list '}'
 *                     | '{' declaration_list '}'
 *                     | '{' declaration_list statement_list '}'
 *                     ;
 */
static struct node_t *compound_statement(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];
	struct node_t *node = NULL;

	if (t->type == TK_LEFT_BRACE) {
		g_position++;
		node = statement_list(tokens);
		expect_token(tokens, TK_RIGHT_BRACE);
	}

	return node;
}

/**
 * @brief statementをパースする
 *
 * statement := labeled_statement
 *            | compound_statement
 *            | expression_statement
 *            | selection_statement
 *            | iteration_statement
 *            | jump_statement
 *            ;
 */
static struct node_t *statement(struct vector_t *tokens)
{
	struct node_t *node = NULL;

	if ((node = compound_statement(tokens)) != NULL)
		return node;

	if ((node = expression_statement(tokens)) != NULL)
		return node;

	if ((node = selection_statement(tokens)) != NULL)
		return node;

	if ((node = jump_statement(tokens)) != NULL)
		return node;

	return NULL;
}

/**
 * statement_list := statement
 *                 | statement_list statement
 *                 ;
 */
static struct node_t *statement_list(struct vector_t *tokens)
{
	struct node_t *n = NULL;
	struct node_t *s;

	if ((s = statement(tokens)) != NULL)
		n = new_node(ND_STATEMENT, s, statement_list(tokens), NULL, -1);

	return n;
}

/**
 * @brief parameter_declarationのパーサ
 *
 * parameter_declaration := declaration_specifiers declarator
 *                        | declaration_specifiers abstract_declarator
 *                        | declaration_specifiers
 *                        ;
 */
static node_t *parameter_declaration(struct vector_t *tokens)
{
	struct node_t *lhs;

	lhs = declaration_specifiers(tokens);
	return new_node(ND_FUNC_PARAM, lhs, declarator(tokens), NULL, -1);
}

/**
 * @brief parameter_listのパーサ
 * parameter_list := parameter_declaration
 *                 | parameter_list ',' parameter_declaration
 *                 ;
 */
static node_t *parameter_list(struct vector_t *tokens)
{
	return parameter_declaration(tokens);
}

/**
 * @brief parameter_type_listのパーサ
 *
 * parameter_type_list := parameter_list
 *                      | parameter_list ',' ELLIPSIS
 */
static struct node_t *parameter_type_list(struct vector_t *tokens)
{
	return parameter_list(tokens);
}

/**
 * @brief direct_declaratorのパーサ
 *
 * direct_declarator := IDENTIFIER
 *                    | '(' declarator ')'
 *                    | direct_declarator '[' constant_expression ']'
 *                    | direct_declarator '[' ']'
 *                    | direct_declarator '(' parameter_type_list ')'
 *                    | direct_declarator '(' identifier_list ')'
 *                    | direct_declarator '(' ')'
 *                    ;
 */
static struct node_t *direct_declarator(struct vector_t *tokens)
{
	struct token_t *t;
	struct node_t *lhs;

	if ((lhs = identifier(tokens)) != NULL) {
		t = tokens->data[g_position];

		/* parameter_type_list */
		if (t->type == TK_LEFT_PAREN) {
			consume_token(tokens, TK_LEFT_PAREN);
			lhs->lhs = new_node(ND_FUNC_PLIST, parameter_type_list(tokens), 0, NULL, -1);
			expect_token(tokens, TK_RIGHT_PAREN);
		}

		return lhs;
	}

	return NULL;
}

/**
 * @brief declaratorのパーサ
 *
 * declarator := pointer direct_declarator
 *             | direct_declarator
 *             ;
 */
static struct node_t *declarator(struct vector_t *tokens)
{
	return direct_declarator(tokens);
}

/**
 * @brief type_specifierのパーサ
 *
 * type_specifier := VOID
 *                 | CHAR
 *                 | SHORT
 *                 | INT
 *                 | LONG
 *                 | FLOAT
 *                 | DOUBLE
 *                 | SIGNED
 *                 | UNSIGNED
 *                 | struct_or_union_specifier
 *                 | enum_specifier
 *                 | TYPE_NAME
 *                 ;
 */
static struct node_t *type_specifier(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_INT) {
		g_position++;
		return new_node(ND_TYPE, NULL, NULL, t->name, -1);
	}

	return NULL;
}

/**
 * @brief declaration_specifiersのパーサ
 *
 * declaration_specifiers := storage_class_specifier
 *                         | storage_class_specifier declaration_specifiers
 *                         | type_specifier
 *                         | type_specifier declaration_specifiers
 *                         | type_qualifier
 *                         | type_qualifier declaration_specifiers
 *                         ;
 */
static struct node_t *declaration_specifiers(struct vector_t *tokens)
{
	return type_specifier(tokens);
}

/**
 * @brief function_definitionのパーサ
 *
 * function_definition := declaration_specifiers declarator declaration_list compound_statement
 *                      | declaration_specifiers declarator compound_statement
 *                      | declarator declaration_list compound_statement
 *                      | declarator compound_statement
 *                      ;
 */
static struct node_t *function_definition(struct vector_t *tokens)
{
	struct node_t *lhs;

	if ((lhs = declaration_specifiers(tokens)) != NULL) {
		lhs->lhs = declarator(tokens);
		return new_node(ND_FUNC_DEF, lhs, compound_statement(tokens), NULL, -1);
	}

	return lhs;
}

/**
 * @brief external_declarationのパーサ
 *
 * external_declaration := function_definition
 *                       | declaration
 *                       ;
 */
static struct node_t *external_declaration(struct vector_t *tokens)
{
	return function_definition(tokens);
}

/**
 * @brief translation_unitのパーサ
 *
 * translation_unit := external_declaration
 *                   | translation_unit external_declaration
 *                   ;
 */
static struct node_t *translation_unit(struct vector_t *tokens)
{
	struct node_t *lhs;

	if ((lhs = external_declaration(tokens)) != NULL)
		return new_node(ND_DECLARATION, lhs, translation_unit(tokens), NULL, -1);

	return NULL;
}

/**
 * @brief main function of parser
 */
struct node_t *parse(struct vector_t *tokens)
{
	struct node_t *lhs = translation_unit(tokens); // start point

	if (lhs == NULL)
		parse_error();

	return lhs;
}
