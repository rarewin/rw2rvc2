#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief RISC-Vのアセンブラを生成する
 */
void gen_riscv(struct vector_t *irv)
{
	unsigned int i;
	struct ir_t *ir;

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		switch (ir->op) {
		case IR_IMM:
			printf("	li	%s, %d\n", get_temp_reg_str(ir->lhs), ir->rhs);
			break;
		case IR_MOV:
			printf("	mv	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;
		case IR_RETURN:
			if (ir->lhs != -1)
				printf("	mv	a0, %s\n", get_temp_reg_str(ir->lhs));
			printf("	ret\n");
			break;
		case IR_PLUS:
			printf("	add	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;
		case IR_MINUS:
			printf("	sub	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;
		case IR_MUL:
			printf("	mul	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;
		case IR_DIV:
			printf("	div	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;
		case IR_NOP:
		case IR_KILL:
		default:
			break;
		}
	}
}
