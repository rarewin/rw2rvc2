/**
 * @brief トークナイザー
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "rw2rvc2.h"

/**
 * @brief allocate memory to the new token
 * @return a pointer to the new token
 */
static struct token_t *allocate_token(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct token_t *token_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	/* 初期化 */
	if (token_array == NULL)
		token_array = (struct token_t*)malloc(sizeof(struct token_t) * size);

	/* サイズを拡大する */
	if (index >= size) {
		size *= 2;
		token_array = (struct token_t*)realloc(token_array, sizeof(struct token_t) * size);
	}

	return &token_array[index++];
}

/**
 * @brief add token
 * @param[in]  v      トークンを詰めるvector_t
 * @param[in]  type   タイプ (TK_XXX)
 * @param[in]  input  入力文字列へのポインタ
 */
struct token_t *add_token(struct vector_t *v, token_type_t type, char *input)
{
	struct token_t *t = allocate_token();

	t->type = type;
	t->input = input;
	vector_push(v, t);

	return t;
}

/**
 * @brief トークナイザー メイン
 * @param[in] p  入力文字列へのポインタ
 */
struct vector_t *tokenize(char *p)
{
	struct vector_t *v = new_vector();
	struct token_t *t;
	const struct keyword_t {
		char *word;
		token_type_t tkval;
	} keywords[] = {
		{"return", TK_RETURN},
		{"if",     TK_IF},
		{"else",   TK_ELSE},
		{"goto",   TK_GOTO},
	};
	unsigned int i;

	while (*p) {

		/* ignore spaces */
		if (isspace(*p)) {
			p++;
			continue;
		}

		/* symbols */
		if (strchr("+-*/;()'\"=", *p) != NULL) {
			switch (*p) {
			case '+':
				add_token(v, TK_PLUS, p);
				break;
			case '-':
				add_token(v, TK_MINUS, p);
				break;
			case '*':
				add_token(v, TK_MUL, p);
				break;
			case '/':
				add_token(v, TK_DIV, p);
				break;
			case ';':
				add_token(v, TK_SEMICOLON, p);
				break;
			case ':':
				add_token(v, TK_COLON, p);
				break;
			case '(':
				add_token(v, TK_LEFT_PAREN, p);
				break;
			case ')':
				add_token(v, TK_RIGHT_PAREN, p);
				break;
			case '\'':
				add_token(v, TK_SINGLE_QUOTE, p);
				break;
			case '"':
				add_token(v, TK_DOUBLE_QUOTE, p);
				break;
			case '=':
				add_token(v, TK_EQUAL, p);
				break;
			default:
				break;
			}
			p++;
			continue;
		}

		/* number */
		if (isdigit(*p)) {
			t = add_token(v, TK_NUM, p);
			t->value = strtol(p, &p, 10);
			continue;
		}

		if (isalpha(*p) || *p == '_') {
			size_t len = 1;

			while (isalpha(p[len]) || isdigit(p[len]) || p[len] == '_')
				len++;

			/* 予約語かどうかの判定 */
			for (i = 0; i < (sizeof(keywords) / sizeof(keywords[0])); i++) {
				if (strncmp(keywords[i].word, p, len) == 0) {
					add_token(v, keywords[i].tkval, p);	/* 予約語だった */
					p += len;
					break;
				}
			}

			/* 予約語だったら次のトークンへ */
			if (i < (sizeof(keywords) / sizeof(keywords[0])))
				continue;

			/* 識別子 */
			t = add_token(v, TK_IDENT, p);
			t->name = strndup(p, len);
			p += len;
			continue;
		}

		color_printf(COL_RED, "tokenize error: %s\n", p);
		exit(1);
	}

	add_token(v, TK_EOF, "EOF");

	return v;
}
