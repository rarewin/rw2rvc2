#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief RISC-Vのアセンブラを生成する
 *
 * @param[in]  irv IRのベクタ
 * @param[in]  d   辞書
 */
void gen_riscv(struct vector_t *irv, struct dict_t *d)
{
	struct ir_t *ir;
	unsigned int i;
	int j;

	for (i = 0; i < d->len; i++)
		printf("	.comm %s, 4, 4\n", (d->dict)[i].key);

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		if (ir->op == IR_FUNC_DEF) {
			printf("	.section .text\n");
			printf("	.global %s\n", ir->name);
			printf("	.type %s, @function\n", ir->name);
			printf("%s:\n", ir->name);
		}

		if (ir->op == IR_FUNC_CALL) {
			struct using_regs_list_t *using_regs;

			using_regs = get_using_regs(ir->rhs);

			printf("	addi	sp, sp, -%d\n", using_regs->num * 8 + 8);
			printf("	sd	ra, 0(sp)\n");

			for (j = 0; j < using_regs->num; j++)
				printf("	sd	%s, %d(sp)\n", get_temp_reg_str(using_regs->list[j]), j * 8 + 8);

			printf("	call	%s\n", ir->name);

			for (j = using_regs->num - 1; j >= 0 ; j--)
				printf("	ld	%s, %d(sp)\n", get_temp_reg_str(using_regs->list[j]), j * 8 + 8);

			printf("	ld	ra, 0(sp)\n");
			printf("	addi	sp, sp, %d\n", using_regs->num * 8 + 8);
			printf("	mv	%s, a0\n", get_temp_reg_str(ir->lhs));
		}

		if (ir->op == IR_FUNC_ARG)
			printf("	mv	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_FUNC_END)
			printf("	.size %s, . - %s\n\n", ir->name, ir->name);

		if (ir->op == IR_IMM)
			printf("	li	%s, %d\n", get_temp_reg_str(ir->lhs), ir->rhs);

		if (ir->op == IR_MOV)
			printf("	mv	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_LOADADDR)
			printf("	la	%s, %s\n", get_temp_reg_str(ir->lhs), ir->name);

		if (ir->op == IR_RETURN) {
			if (ir->lhs != -1)
				printf("	mv	a0, %s\n", get_temp_reg_str(ir->lhs));
			printf("	ret\n");
		}

		if (ir->op == IR_PLUS)
			printf("	add	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_MINUS)
			printf("	sub	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_MUL)
			printf("	mul	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_DIV)
			printf("	div	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_MOD)
			printf("	rem	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_AND)
			printf("	and	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_OR)
			printf("	or	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_XOR)
			printf("	xor	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_NOT)
			printf("	not	%s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs));

		if (ir->op == IR_STORE)
			printf("	sw	%s, 0(%s)\n", get_temp_reg_str(ir->rhs), get_temp_reg_str(ir->lhs));

		if (ir->op == IR_LOAD)
			printf("	lw	%s, 0(%s)\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_BEQZ)
			printf("	beqz	%s, .L%d\n", get_temp_reg_str(ir->lhs), ir->rhs);

		if (ir->op == IR_SLT)
			printf("	slt	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_SLET) {
			printf("	slt	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));
			printf("	xori	%s, %s, 1\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs));
		}

		if (ir->op == IR_LEFT_OP)
			printf("	sll	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_RIGHT_OP)
			printf("	srl	%s, %s, %s\n", get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->lhs), get_temp_reg_str(ir->rhs));

		if (ir->op == IR_JUMP)
			printf("	j	.L%d\n", ir->lhs);

		if (ir->op == IR_LABEL)
			printf(".L%d:\n", ir->lhs);
	}
}
