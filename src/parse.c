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
static struct node_t *unary_expression(struct vector_t *tokens);

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
		error_printf("unexpect token: %s at line %d position %d\n", t->input, t->line, t->position);
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

	/* 新規のメモリプールを作成 */
	if (node_array == NULL || index >= ALLOCATE_SIZE) {
		if ((node_array = (struct node_t*)malloc(sizeof(struct node_t) * size)) == NULL) {
			color_printf(stderr, COL_RED, "memory allocation failed\n");
			exit(1);
		}
		index = 0;
	}

	return &node_array[index++];
}

/**
 * @brief 新規ノードをつくる
 * @param[in] op      ノードの種類
 * @param[in] lhs     左辺
 * @param[in] rhs     右辺
 * @param[in] list    リスト
 * @param[in] name    名前
 * @param[in] value   値 (整数値)
 */
static struct node_t *new_node(node_type_t op,
			       struct node_t *lhs,
			       struct node_t *rhs,
			       struct vector_t *list,
			       char *name,
			       int value)
{
	node_t *node = allocate_node();

	node->type = op;
	node->lhs = lhs;
	node->rhs = rhs;
	node->list = list;
	node->name = name;
	node->value = value;

	return node;
}

/**
 * @brief 演算子のトークンタイプからノードタイプに変換できるものを変換する.
 * @param[in] tt  トークンタイプ
 * @return 変換できればそのノードタイプを, 失敗したらND_ILLEGALを返す.
 */
static node_type_t convert_token_to_node(token_type_t tt)
{
	switch (tt) {
	case TK_PLUS:
		return ND_PLUS;
	case TK_MINUS:
		return ND_MINUS;
	case TK_MUL:
		return ND_MUL;
	case TK_DIV:
		return ND_DIV;
	case TK_MOD:
		return ND_MOD;
	case TK_NUM:
		return ND_CONST;
	case TK_RETURN:
		return ND_RETURN;
	default:
		return ND_ILLEGAL;
	}
}

/**
 * @brief primary expressionのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * primary_expression := IDENTIFIER
 *                     | CONSTANT
 *                     | STRING_LITERAL
 *                     | '(' expression ')'
 *                     ;
 *
 * @todo STRING_LITERAL
 */
static struct node_t *primary_expression(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];
	struct node_t *n;

	/* '(' expression ')' */
	if (t->type == TK_LEFT_PAREN) {
		g_position++;
		n = expression(tokens);
		consume_token(tokens, TK_RIGHT_PAREN);
		return n;
	}

	/* CONSTANT */
	if (t->type == TK_NUM) {
		g_position++;
		return new_node(ND_CONST, NULL, NULL, NULL, NULL, t->value);
	}

	/* IDENTIFIER */
	if (t->type == TK_IDENT) {
		g_position++;
		return new_node(ND_IDENT, NULL, NULL, NULL, t->name, -1);
	}

	return NULL;
}

/**
 * @brief argument_expression_list
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * argument_expression_list := assignment_expression
 *                           | argument_expression_list ',' assignment_expression
 *                           ;
 */
static struct node_t *argument_expression_list(struct vector_t *tokens)
{
	struct node_t *al;
	struct node_t *n;
	struct token_t *t;

	int num = 0;

	if ((n = assignment_expression(tokens)) == NULL)
		return NULL;

	al = new_node(ND_FUNC_ALIST, NULL, NULL, new_vector(), NULL, -1);

	for (;;) {
		vector_push(al->list, new_node(ND_FUNC_ARG, n, NULL, NULL, NULL, num));

		t = tokens->data[g_position];
		if (t->type != TK_COMMA)
			break;

		num++;
		if ((n = assignment_expression(tokens)) == NULL)
			break;
	}

	return al;
}

/**
 * @brief postfix_expressionのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
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
 *
 * @todo '[' expression ']', '.' IDENTIFIER, PTR_OP IDENTIFIER, INC_OP, DEC_OP
 */
static struct node_t *postfix_expression(struct vector_t *tokens)
{
	struct node_t *n;
	struct token_t *t;

	n = primary_expression(tokens);

	for (;;) {
		t = tokens->data[g_position];

		/* '(' ')' */
		/* '(' argument_expression_list ')' */
		if (t->type == TK_LEFT_PAREN) {
			consume_token(tokens, TK_LEFT_PAREN);

			if (t->type == TK_RIGHT_PAREN) { /* '(' ')' */
				expect_token(tokens, TK_RIGHT_PAREN);
				n = new_node(ND_FUNC_CALL, n, NULL, NULL, n->name, -1);
			} else { /* '(' argument_expression_list ')' */
				n = new_node(ND_FUNC_CALL, n, argument_expression_list(tokens), NULL, n->name, -1);
				expect_token(tokens, TK_RIGHT_PAREN);
			}
			continue;
		}

		break;
	}

	return n;
}

/**
 * @brief unary_operatorのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * unary_operator := '&'
 *                 | '*'
 *                 | '+'
 *                 | '-'
 *                 | '~'
 *                 | '!'
 *                 ;
 *
 * @todo: &, *, ~, ! の実装
 */
static struct node_t *unary_operator(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_MINUS) { /* - */
		g_position++;
		return new_node(ND_MINUS, NULL, NULL, NULL, NULL, -1);
	}

	if (t->type == TK_PLUS) { /* + */
		g_position++;
		return new_node(ND_PLUS, NULL, NULL, NULL, NULL, -1);
	}

	return NULL;
}

/**
 * @brief cast_expressionのパーサ
 *
 * cast_expression := unary_expression
 *                  | '(' type_name ')' cast_expression
 *                  ;
 *
 * @todo type_nameの実装
 */
static struct node_t *cast_expression(struct vector_t *tokens)
{
	return unary_expression(tokens);
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
	struct node_t *lhs;

	if ((lhs = unary_operator(tokens)) != NULL) {

		if ((lhs->rhs = cast_expression(tokens)) == NULL)
			parse_error();

		return lhs;
	}

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
		lhs = new_node(convert_token_to_node(op), lhs, unary_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(convert_token_to_node(op), lhs, multiplicative_expression(tokens), NULL, NULL, -1);
	}

	return lhs;
}

static struct node_t *identifier(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_IDENT) {
		g_position++;
		return new_node(ND_IDENT, NULL, NULL, NULL, t->name, -1);
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
		lhs = new_node(nd_type, lhs, additive_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(nd_type, lhs, shift_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(nd_type, lhs, relational_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(ND_AND, lhs, equality_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(ND_XOR, lhs, and_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(ND_OR, lhs, exclusive_or_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(ND_AND_OP, lhs, inclusive_or_expression(tokens), NULL, NULL, -1);
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
		lhs = new_node(ND_OR_OP, lhs, logical_and_expression(tokens), NULL, NULL, -1);
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
		 type == TK_ADD_ASSIGN || type == TK_SUB_ASSIGN ||
		 type == TK_LEFT_ASSIGN || type == TK_RIGHT_ASSIGN)) ? true : false;
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
				rhs = new_node(ND_MUL, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_DIV_ASSIGN) {
				rhs = new_node(ND_DIV, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_MOD_ASSIGN) {
				rhs = new_node(ND_MOD, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_ADD_ASSIGN) {
				rhs = new_node(ND_PLUS, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_SUB_ASSIGN) {
				rhs = new_node(ND_MINUS, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_LEFT_ASSIGN) {
				rhs = new_node(ND_LEFT_OP, lhs, expression(tokens), NULL, NULL, -1);
			} else if (t->type == TK_RIGHT_ASSIGN) {
				rhs = new_node(ND_RIGHT_OP, lhs, expression(tokens), NULL, NULL, -1);
			} else {
				rhs = expression(tokens);
			}

			lhs = new_node(ND_ASSIGN, lhs, rhs, NULL, NULL, -1);
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

	lhs = new_node(ND_EXPRESSION, lhs, NULL, NULL, NULL, -1);

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
		e = new_node(ND_RETURN, expression(tokens), NULL, NULL, NULL, -1);
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
		node = new_node(ND_IF, expression(tokens), NULL, NULL, NULL, -1);
		expect_token(tokens, TK_RIGHT_PAREN);

		node->rhs = new_node(ND_THEN_ELSE, statement(tokens), NULL, NULL, NULL, -1);

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
 * @brief initializerをパースする
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * initializer := assignment_expression
 *              | '{' initializer_list '}'
 *              | '{' initializer_list ',' '}'
 *              ;
 *
 * @todo assignment_expression以外の実装
 */
static struct node_t *initializer(struct vector_t *tokens)
{
	return assignment_expression(tokens);
}

/**
 * @brief init_declaratorをパースする
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * init_declarator := declarator
 *                  | declarator '=' initializer
 *                  ;
 */
static struct node_t *init_declarator(struct vector_t *tokens)
{
	struct node_t *n;
	struct token_t *t;

	n = declarator(tokens);
	t = tokens->data[g_position];

	if (t->type == TK_EQUAL) {
		consume_token(tokens, TK_EQUAL);
		n->rhs = initializer(tokens);
	}

	return n;
}

/**
 * @brief init_declarator_listをパースする
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * init_declarator_list := init_declarator
 *                       | init_declarator_list ',' init_declarator
 *                       ;
 */
static struct node_t *init_declarator_list(struct vector_t *tokens)
{
	struct node_t *idl = NULL, *id;
	struct token_t *t;
	int num = 0;

	if ((id = init_declarator(tokens)) == NULL)
		return NULL;

	idl = new_node(ND_VAR_INIT_DLIST, NULL, NULL, new_vector(), NULL, num++);

	do {
		vector_push(idl->list, id);
		t = tokens->data[g_position];

		if (t->type != TK_COMMA)
			break;

		id = init_declarator(tokens);
	} while (idl != NULL);

	return idl;
}

/**
 * @brief declarationをパースする
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * declaration := declaration_specifiers ';'
 *              | declaration_specifiers init_declarator_list ';'
 *              ;
 */
static struct node_t *declaration(struct vector_t *tokens)
{
	struct node_t *d = NULL, *ds;

	if ((ds = declaration_specifiers(tokens)) != NULL) {
		d = new_node(ND_VAR_DEC, ds, init_declarator_list(tokens), NULL, NULL, -1);
		expect_token(tokens, TK_SEMICOLON);
	}

	return d;
}

/**
 * @brief declaration_listをパースする
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * declaration_list := declaration
 *                   | declaration_list declaration
 *                   ;
 */
static struct node_t *declaration_list(struct vector_t *tokens)
{
	struct node_t *dl = NULL;
	struct node_t *dn = NULL;

	if ((dn = declaration(tokens)) == NULL)
		return NULL;

	dl = new_node(ND_VAR_DLIST, NULL, NULL, new_vector(), NULL, -1);

	do {
		vector_push(dl->list, dn);
		dn = declaration(tokens);
	} while (dn != NULL);

	return dl;
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
	struct node_t *sl = NULL, *dl = NULL, *n;

	if (t->type == TK_LEFT_BRACE) {
		g_position++;
		dl = declaration_list(tokens);
		sl = statement_list(tokens);
		expect_token(tokens, TK_RIGHT_BRACE);
	}

	if (dl == NULL && sl == NULL)
		return NULL;

	n = new_node(ND_COMPOUND_STATEMENTS, NULL, NULL, new_vector(), NULL, -1);

	if (dl != NULL)
		vector_push(n->list, dl);

	if (sl != NULL)
		vector_push(n->list, sl);

	return n;
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
 * @brief statement_listパーサ
 *
 * statement_list := statement
 *                 | statement_list statement
 *                 ;
 */
static struct node_t *statement_list(struct vector_t *tokens)
{
	struct node_t *sl = NULL;
	struct node_t *s;

	if ((s = statement(tokens)) == NULL)
		return NULL;

	/* 新規にSTATEMENTSノードを作成する  */
	sl = new_node(ND_STATEMENTS, NULL, NULL, new_vector(), NULL, -1);

	do {
		vector_push(sl->list, s);
		s = statement(tokens);
	} while (s != NULL);

	return sl;
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

	if ((lhs = declaration_specifiers(tokens)) == NULL)
		return NULL;

	return new_node(ND_FUNC_PARAM, lhs, declarator(tokens), NULL, NULL, -1);
}

/**
 * @brief parameter_listのパーサ
 * parameter_list := parameter_declaration
 *                 | parameter_list ',' parameter_declaration
 *                 ;
 */
static node_t *parameter_list(struct vector_t *tokens)
{
	struct node_t *pl = NULL;
	struct node_t *n;
	struct token_t *t;

	if ((n = parameter_declaration(tokens)) == NULL)
		return NULL;

	/* 新規にFUNC_PLISTノードを作成する */
	pl = new_node(ND_FUNC_PLIST, NULL, NULL, new_vector(), NULL, -1);

	for (;;) {
		t = tokens->data[g_position];

		if (n == NULL)
			break;

		vector_push(pl->list, n);

		if (t->type != TK_COMMA)
			break;

		consume_token(tokens, TK_COMMA);

		n = parameter_declaration(tokens);
	}

	return pl;
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
 * @param tokens  トークンベクタ
 * @return パース結果のノード
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
	struct node_t *n;

	if ((n = identifier(tokens)) != NULL) {
		t = tokens->data[g_position];

		/* parameter_type_list */
		if (t->type == TK_LEFT_PAREN) {
			consume_token(tokens, TK_LEFT_PAREN);
			n->lhs = parameter_type_list(tokens);
			expect_token(tokens, TK_RIGHT_PAREN);
		}

		return n;
	}

	return NULL;
}

/**
 * @brief declaratorのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * declarator := pointer direct_declarator
 *             | direct_declarator
 *             ;
 *
 * @todo direct_declarator以外への対応
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
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * @todo INT以外への対応
 */
static struct node_t *type_specifier(struct vector_t *tokens)
{
	struct token_t *t = tokens->data[g_position];

	if (t->type == TK_INT) {
		g_position++;
		return new_node(ND_TYPE, NULL, NULL, NULL, t->name, -1);
	}

	return NULL;
}

/**
 * @brief declaration_specifiersのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
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
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * function_definition := declaration_specifiers declarator declaration_list compound_statement
 *                      | declaration_specifiers declarator compound_statement
 *                      | declarator declaration_list compound_statement
 *                      | declarator compound_statement
 *                      ;
 *
 * @todo declarator compound_statement 以外への対応
 */
static struct node_t *function_definition(struct vector_t *tokens)
{
	struct node_t *dn, *sn;
	int stored_pos = g_position;

	if ((dn = declaration_specifiers(tokens)) == NULL)
		goto ERR;

	if ((dn->lhs = declarator(tokens)) == NULL)
		goto ERR;

	if ((sn = compound_statement(tokens)) == NULL)
		goto ERR;

	return new_node(ND_FUNC_DEF, dn, sn, NULL, NULL, -1);

ERR:
	g_position = stored_pos;
	return NULL;
}

/**
 * @brief external_declarationのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * external_declaration := function_definition
 *                       | declaration
 *                       ;
 *
 * @todo declarationの処理
 */
static struct node_t *external_declaration(struct vector_t *tokens)
{
	struct node_t *n;

	if ((n = function_definition(tokens)) != NULL)
		return n;

	if ((n = declaration(tokens)) == NULL)
		return NULL;

	/* グルーバル変数なので, ノードを書き換える. */
	n->type = ND_VAR_DEC_STATIC;

	return n;
}

/**
 * @brief translation_unitのパーサ
 *
 * @param tokens  トークンベクタ
 * @return パース結果のノード
 *
 * translation_unit := external_declaration
 *                   | translation_unit external_declaration
 *                   ;
 */
static struct node_t *translation_unit(struct vector_t *tokens)
{
	struct node_t *tu = NULL;
	struct node_t *n;

	if ((n = external_declaration(tokens)) == NULL)
		return NULL;

	/* 新規にPROGRAMノードを作成する */
	tu = new_node(ND_PROGRAM, NULL, NULL, new_vector(), NULL, -1);

	do {
		vector_push(tu->list, n);
		n = external_declaration(tokens);
	} while (n != NULL);

	return tu;
}

/**
 * @brief パーサーのメイン関数
 */
struct node_t *parse(struct vector_t *tokens)
{
	struct node_t *p = translation_unit(tokens); // start point

	if (p == NULL)
		parse_error();

	return p;
}
