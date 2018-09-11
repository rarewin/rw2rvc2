#include <ctype.h>
#include <stdio.h>

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

	if (token_array == NULL)
		token_array = (struct token_t*)malloc(sizeof(struct token_t) * size);

	if (index >= size) {
		size += ALLOCATE_SIZE;
		token_array = (struct token_t*)realloc(token_array, sizeof(struct token_t) * size);
	}

	return &token_array[index++];
}

/**
 * @brief add token
 * @param[in]  v
 * @param[in]  type
 * @param[in]  input
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
 * @brief tokenizer
 */
struct vector_t *tokenize(char *p)
{
	struct vector_t *v = new_vector();

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
				add_token(v, TK_PLUS, p);
				break;
			case '-':
				add_token(v, TK_MINUS, p);
				break;
			default:
				break;
			}
			p++;
			continue;
		}

		/* number */
		if (isdigit(*p)) {
			struct token_t *t = add_token(v, TK_NUM, p);
			t->value = strtol(p, &p, 10);
			continue;
		}

		fprintf(stderr, "tokenize error: %s", p);
		exit(1);
	}

	add_token(v, TK_EOF, 0);

	return v;
}
