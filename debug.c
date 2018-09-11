#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief debug function for tokenizer
 */
void show_token(struct vector_t *tokens)
{
	const char *table[] = {
		[TK_PLUS] = "TK_PLUS",
		[TK_MINUS] = "TK_MINUS",
		[TK_NUM]   = "TK_NUM",
		[TK_EOF]   = "TK_EOF",
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
#define OP2STR_ELEMENT(e)    [e] = #e
	struct ir_t *ir;
	unsigned int i;

	const char *OP2STR[] = {
		OP2STR_ELEMENT(IR_PLUS),
		OP2STR_ELEMENT(IR_MINUS),
		OP2STR_ELEMENT(IR_IMM),
		OP2STR_ELEMENT(IR_MOV),
		OP2STR_ELEMENT(IR_RETURN),
		OP2STR_ELEMENT(IR_KILL),
		OP2STR_ELEMENT(IR_NOP),
	};

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];
		printf("%s(%d) %d %d\n",
		       OP2STR[ir->op], ir->op, ir->lhs, ir->rhs);
	}
}
