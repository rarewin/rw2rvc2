#include <stdio.h>

#include "rw2rvc2.h"

#define TRANS_ELEMENT(e)    [e] = #e

/**
 * @brief debug function for tokenizer
 */
void show_token(struct vector_t *tokens)
{
	const char *table[] = {
		TRANS_ELEMENT(TK_PLUS),
		TRANS_ELEMENT(TK_MINUS),
		TRANS_ELEMENT(TK_MUL),
		TRANS_ELEMENT(TK_DIV),
		TRANS_ELEMENT(TK_NUM),
		TRANS_ELEMENT(TK_EOF),
	};
	unsigned int i;

	for (i = 0; i < tokens->len; i++) {
		printf("%s\n", table[((struct token_t*)(tokens->data[i]))->type]);
	}
}

/**
 * @brief debug function for IR
 */
void show_ir(struct vector_t *irv)
{
	struct ir_t *ir;
	unsigned int i;

	const char *OP2STR[] = {
		TRANS_ELEMENT(IR_PLUS),
		TRANS_ELEMENT(IR_MINUS),
		TRANS_ELEMENT(IR_MUL),
		TRANS_ELEMENT(IR_DIV),
		TRANS_ELEMENT(IR_IMM),
		TRANS_ELEMENT(IR_MOV),
		TRANS_ELEMENT(IR_RETURN),
		TRANS_ELEMENT(IR_KILL),
		TRANS_ELEMENT(IR_NOP),
	};

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];
		printf("%s(%d) %d %d\n",
		       OP2STR[ir->op], ir->op, ir->lhs, ir->rhs);
	}
}
