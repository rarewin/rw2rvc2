/**
 * @brief アセンブラ生成
 *
 * @author Katsuki Kobayashi<rare@tirasweel.org>
 * @copyright 2018- Katsuki Kobayashi. All rights reserved.
 */
#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief RISC-Vのアセンブラを生成する
 */
void gen_riscv(struct vector_t *irv, struct dict_t *d)
{
	struct ir_t *ir;
	unsigned int i;
	int j;

	printf("	.section .data\n");
	for (i = 0; i < d->len; i++) {
		struct variable_t *v = (d->dict)[i].value;
		if (v->scope_level == 0) {
			if (v->node->rhs != NULL && (v->node->rhs->type == ND_CONST && v->node->rhs->value != 0)) {
				printf("%s:\n", (d->dict)[i].key);
				printf("	.word	%d\n", v->node->rhs->value);
			}
		}
	}

	printf("\n");

	for (i = 0; i < d->len; i++) {
		struct variable_t *v = (d->dict)[i].value;
		if (v->scope_level == 0) {
			if (v->node->rhs == NULL || (v->node->rhs->type == ND_CONST && v->node->rhs->value == 0))
				printf("	.comm %s, 4, 4\n", (d->dict)[i].key);
		} else {
			/* @todo スタック上への割り当て  */
			printf("	.comm %s, 4, 4\n", (d->dict)[i].key);
		}
	}

	printf("\n");

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		switch (ir->op) {
		case IR_FUNC_DEF:
			printf("	.section .text\n");
			printf("	.global %s\n", ir->name);
			printf("	.type %s, @function\n", ir->name);
			printf("%s:\n", ir->name);
			printf("	sd	ra, -%d(sp)\n", COMPILE_WORD_SIZE);
			printf("	sd	s0, -%d(sp)\n", COMPILE_WORD_SIZE * 2);
			printf("	mv	s0, sp\n");
			printf("	addi	sp, sp, -%d\n", COMPILE_WORD_SIZE * 2);
			break;

		case IR_FUNC_CALL:
		{
			struct using_regs_list_t *using_regs;

			using_regs = get_using_regs(ir->rhs);

			printf("	addi	sp, sp, -%d\n", using_regs->num * COMPILE_WORD_SIZE + COMPILE_WORD_SIZE);
			printf("	sd	ra, 0(sp)\n");

			for (j = 0; j < using_regs->num; j++)
				printf("	sd	%s, %d(sp)\n", get_temp_reg_str(using_regs->list[j]), j * COMPILE_WORD_SIZE + COMPILE_WORD_SIZE);

			printf("	call	%s\n", ir->name);

			for (j = using_regs->num - 1; j >= 0 ; j--)
				printf("	ld	%s, %d(sp)\n", get_temp_reg_str(using_regs->list[j]), j * COMPILE_WORD_SIZE + COMPILE_WORD_SIZE);

			printf("	ld	ra, 0(sp)\n");
			printf("	addi	sp, sp, %d\n", using_regs->num * COMPILE_WORD_SIZE + COMPILE_WORD_SIZE);
			printf("	mv	%s, a0\n", get_temp_reg_str(ir->lhs));
			break;
		}

		case IR_FUNC_PARAM:
			printf("	sw	%s, 0(%s)\n", get_temp_reg_str(ir->rhs), get_temp_reg_str(ir->lhs));
			break;

		case IR_FUNC_ARG:
			printf("	mv	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_FUNC_END:
			printf("	.size %s, . - %s\n\n", ir->name, ir->name);
			break;

		case IR_IMM:
			printf("	li	%s, %d\n", get_temp_reg_str(ir->lhs), ir->rhs);
			break;

		case IR_MOV:
			printf("	mv	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_LOADADDR:
			printf("	la	%s, %s\n", get_temp_reg_str(ir->lhs), ir->name);
			break;

		case IR_RETURN:
			if (ir->lhs != -1)
				printf("	mv	a0, %s\n", get_temp_reg_str(ir->lhs));
			printf("	ld	ra, -%d(s0)\n", COMPILE_WORD_SIZE);
			printf("	ld	s0, -%d(s0)\n", COMPILE_WORD_SIZE * 2);
			printf("	addi	sp, sp, %d\n", COMPILE_WORD_SIZE * 2);
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

		case IR_MOD:
			printf("	rem	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_AND:
			printf("	and	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_OR:
			printf("	or	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_XOR:
			printf("	xor	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_NOT:
			printf("	not	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs));
			break;

		case IR_STORE:
			printf("	sw	%s, 0(%s)\n", get_temp_reg_str(ir->rhs), get_temp_reg_str(ir->lhs));
			break;

		case IR_LOAD:
			printf("	lw	%s, 0(%s)\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_BEQZ:
			printf("	beqz	%s, .L%d\n", get_temp_reg_str(ir->lhs), ir->rhs);
			break;

		case IR_SLT:
			printf("	slt	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_SLET:
			printf("	slt	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			printf("	xori	%s, %s, 1\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs));
			break;

		case IR_LEFT_OP:
			printf("	sllw	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_RIGHT_OP:
			printf("	srl	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			break;

		case IR_JUMP:
			printf("	j	.L%d\n", ir->lhs);
			break;

		case IR_LABEL:
			printf(".L%d:\n", ir->lhs);
			break;

		case IR_EQ_OP:
		case IR_NE_OP:
		case IR_KILL:
		case IR_KILL_ARG:
		case IR_NOP:
			break;
		}
	}
}
