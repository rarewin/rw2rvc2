#include <stdbool.h>
#include <stdlib.h>

#include "rw2rvc2.h"

static char *TEMP_REGS[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
			    "a1", "a2", "a3", "a4", "a5", "a6", "a7", NULL};
#define NUM_OF_TEMP_REGS  14  // = sizeof(TEMP_REGS) / ziefof(TEMP_REGS[0]) - 1
static bool used_temp_regs[NUM_OF_TEMP_REGS];

/**
 * @brief 割り当て可能なスクラッチレジスタを探す
 * @param[in] ir_reg   register
 * @param[in] reg_map  register map
 */
int find_allocatable_reg(int ir_reg, int *reg_map)
{
	int i;

	if (reg_map[ir_reg] != -1)
		return reg_map[ir_reg];		// ?

	for (i = 0; i < NUM_OF_TEMP_REGS; i++) {
		if (!used_temp_regs[i]) {
			used_temp_regs[i] = true;
			reg_map[ir_reg] = i;
			return i;
		}
	}

	return -10;
}

/**
 * @brief release the register
 * @param[in] r  register index
 */
void release_reg(int r)
{
	used_temp_regs[r] = false;
}

/**
 * @brief allocate register
 */
void allocate_regs(struct vector_t *irv)
{
	struct ir_t *ir;
	int *reg_map = malloc(sizeof(int) * irv->len);
	unsigned int i;

	/* initialize */
	for (i = 0; i < irv->len; i++)
		reg_map[i] = -1;

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		if (ir->op == IR_IMM || ir->op == IR_LOADADDR) {
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
			continue;
		}

		if (ir->op == IR_MOV || ir->op == IR_PLUS || ir->op == IR_MINUS ||
		    ir->op == IR_MUL || ir->op == IR_DIV  || ir->op == IR_LOAD) {
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
			continue;
		}

		if (ir->op == IR_STORE) {
			ir->lhs = reg_map[ir->lhs];
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
		}

		if (ir->op == IR_RETURN) {
			ir->lhs = reg_map[ir->lhs];
			continue;
		}

		if (ir->op == IR_KILL) {
			release_reg(reg_map[ir->lhs]);
			ir->op = IR_NOP;
			continue;
		}
	}

	free(reg_map);
}

/**
 * @brief get a temporary register's index
 * @param[in] index for a temporary
 * @return string if the register
 */
char *get_temp_reg_str(int index)
{
	if (index < 0 || index >= NUM_OF_TEMP_REGS)
		return "";

	return TEMP_REGS[index];
}
