#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rw2rvc2.h"

static char *TEMP_REGS[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
			    "a7", "a6", "a5", "a4", "a3", "a2", "a1", "a0", NULL};
static bool used_temp_regs[NUM_OF_TEMP_REGS];

// static char *SAVED_REGS[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6",
// 			     "s7", "s8", "s9", "s10", "s11",  NULL};
// #define NUM_OF_SAVED_REGS 12 // = sizeof(SAVED_REGS) / sizeof(SAVED_REGS[0]) - 1


/**
 * @brief 割り当て可能なスクラッチレジスタを探す
 * @param[in] ir_reg   register
 * @param[in] reg_map  register map
 */
static int find_allocatable_reg(int ir_reg, int *reg_map)
{
	int i;

	if (reg_map[ir_reg] != -2)
		return reg_map[ir_reg];		// ?

	for (i = 0; i < NUM_OF_TEMP_REGS; i++) {
		if (!used_temp_regs[i]) {
			used_temp_regs[i] = true;
			reg_map[ir_reg] = i;
			return i;
		}
	}

	return -10;	/* 割り当て不可 */
}

/**
 * @brief アーギュメントレジスタを指定して割り当てる
 *
 * @param[in] ir_reg   IR中のレジスタ番号
 * @param[in] reg_map  レジスタマップ
 * @param[in] arg      引数番号 (0オリジン)
 *
 * @return レジスタ
 */
static int allocate_argument_reg(int ir_reg, int *reg_map, int arg)
{
	int index = NUM_OF_TEMP_REGS - 1 - arg;
	if (used_temp_regs[index])
		return -10;	/* 割り当て不可 */

	used_temp_regs[index] = true;
	reg_map[ir_reg] = index;

	return index;
}

/**
 * @brief release the register
 * @param[in] r  register index
 */
static void release_reg(int r)
{
	used_temp_regs[r] = false;
}

static bool (*using_regs)[NUM_OF_TEMP_REGS] = NULL;

/**
 * @brief 現在使用中のレジスタを記録する
 * @return 記録した配列のインデックス
 */
static int record_using_regs(void)
{
	const size_t ALLOCATE_SIZE = 64;
	static int index = 0;
	static int size = 0;

	if (using_regs == NULL) {
		size = ALLOCATE_SIZE;
		using_regs = (bool(*)[NUM_OF_TEMP_REGS])malloc(sizeof(bool) * NUM_OF_TEMP_REGS * size);
	}

	if (index >= size) {
		size *= 2;
		using_regs = (bool(*)[NUM_OF_TEMP_REGS])realloc(using_regs, sizeof(bool) * NUM_OF_TEMP_REGS * size);
	}

	memcpy(&using_regs[index++], used_temp_regs, sizeof(used_temp_regs));

	return (index - 1);
}

/**
 * @brief 使用中レジスタリストを取得する
 */
using_regs_list_t *get_using_regs(int index)
{
	static using_regs_list_t list;
	int i;
	int j = 0;

	if (using_regs == NULL)
		return NULL;

	for (i = 0; i < NUM_OF_TEMP_REGS; i++) {
		if (using_regs[index][i])
			list.list[j++] = i;
	}

	list.num = j;

	return &list;
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
		reg_map[i] = -2;

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		if (ir->op == IR_IMM || ir->op == IR_LOADADDR)
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);

		if (ir->op == IR_FUNC_ARG) {
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
			ir->lhs = allocate_argument_reg(ir->rhs, reg_map, ir->lhs);
		}

		if (ir->op == IR_FUNC_CALL) {
			ir->rhs = record_using_regs();
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
		}

		if (ir->op == IR_MOV || ir->op == IR_PLUS || ir->op == IR_MINUS ||
		    ir->op == IR_MUL || ir->op == IR_DIV  || ir->op == IR_MOD   ||
		    ir->op == IR_EQ_OP || ir->op == IR_NE_OP ||
		    ir->op == IR_SLT || ir->op == IR_SLET ||
		    ir->op == IR_LEFT_OP || ir->op == IR_RIGHT_OP ||
		    ir->op == IR_OR ||
		    ir->op == IR_LOAD) {
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
		}

		if (ir->op == IR_STORE) {
			ir->lhs = reg_map[ir->lhs];
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
		}

		if (ir->op == IR_RETURN || ir->op == IR_BEQZ || ir->op == IR_NOT)
			ir->lhs = reg_map[ir->lhs];

		if (ir->op == IR_KILL) {
			release_reg(reg_map[ir->lhs]);
			ir->op = IR_NOP;
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
