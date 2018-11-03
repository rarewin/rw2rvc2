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
 * @brief 文字 @p s をトークンタイプに変換する
 * @param[in]  s   文字
 * @return トークンタイプ(TK_XXX)。ただし、変換できなかった場合はTK_INVALIDを返す.
 */
static token_type_t get_token_type_of_symbol(char s)
{
	const struct symbol_t {
		char s;
		token_type_t tkval;
	} symbols[] = {
		{'+', TK_PLUS},
		{'-', TK_MINUS},
		{'*', TK_MUL},
		{'/', TK_DIV},
		{'%', TK_MOD},
		{';', TK_SEMICOLON},
		{':', TK_COLON},
		{'(', TK_LEFT_PAREN},
		{')', TK_RIGHT_PAREN},
		{'{', TK_LEFT_BRACE},
		{'}', TK_RIGHT_BRACE},
		{'\'', TK_SINGLE_QUOTE},
		{'"', TK_DOUBLE_QUOTE},
		{'=', TK_EQUAL},
		{'&', TK_AND},
		{'|', TK_OR},
		{'^', TK_XOR},
		{'!', TK_NOT},
		{'~', TK_INV},
		{'<', TK_LESS_OP},
		{'>', TK_GREATER_OP},
		{',', TK_COMMA},
	};
	unsigned int i;

	/* テーブル参照 */
	for (i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
		if (symbols[i].s == s)
			return symbols[i].tkval;
	}

	return TK_INVALID;
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
		{"int",    TK_INT},
	};
	const struct multibytes_operation_t {
		char *word;
		token_type_t tkval;
	} multibytes_operations[] = {
		{"||", TK_OR_OP},
		{"&&", TK_AND_OP},
		{"==", TK_EQ_OP},
		{"!=", TK_NE_OP},
		{"<=", TK_LE_OP},
		{">=", TK_GE_OP},
		{"*=", TK_MUL_ASSIGN},
		{"/=", TK_DIV_ASSIGN},
		{"%=", TK_MOD_ASSIGN},
		{"+=", TK_ADD_ASSIGN},
		{"-=", TK_SUB_ASSIGN},
		{">>", TK_RIGHT_OP},
		{"<<", TK_LEFT_OP},
	};
	unsigned int i;

	while (*p) {

		/* ignore spaces */
		if (isspace(*p)) {
			p++;
			continue;
		}

		/* symbols */
		if (strchr("+-*/%;(){}'\"=|&^!~<>,", *p) != NULL) {

			/* check if multibytes operations */
			for (i = 0; i < (sizeof(multibytes_operations) / sizeof(multibytes_operations[0])); i++) {
				size_t len = strlen(multibytes_operations[i].word);

				if (strncmp(multibytes_operations[i].word, p, len) == 0) {
					add_token(v, multibytes_operations[i].tkval, p);
					p += len;
					break;
				}
			}

			if (i != (sizeof(multibytes_operations) / sizeof(multibytes_operations[0])))
				continue;

			add_token(v, get_token_type_of_symbol(*p), p);
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
					t = add_token(v, keywords[i].tkval, p);	/* 予約語だった */
					t->name = keywords[i].word;
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
