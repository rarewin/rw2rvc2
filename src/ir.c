#include "rw2rvc2.h"

/**
 * @brief conversion table for node type to IR type
 */
static node_type_t CONVERSION_NODE_TO_IR[] = {
	[ND_PLUS]  = IR_PLUS,
	[ND_MINUS] = IR_MINUS,
	[ND_MUL] = IR_MUL,
	[ND_DIV] = IR_DIV,
	[ND_RETURN] = IR_RETURN,
};

/**
 * @brief allocate memory to a new IR
 * @return allocated ir_t, or NULL if failed
 */
static struct ir_t *allocate_ir(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct ir_t *ir_array = NULL;
	static size_t index = 0;
	static size_t size = 0;

	/* initial allocation */
	if (ir_array == NULL) {
		size += ALLOCATE_SIZE;
		ir_array = (struct ir_t*)malloc(sizeof(struct ir_t) * size);
	}

	/* reallocate */
	if (index >= size) {
		size += ALLOCATE_SIZE;
		ir_array = (struct ir_t*)realloc(ir_array, sizeof(struct ir_t) * size);
	}

	return &ir_array[index++];
}

/**
 * @brief 新しいIR行を作成つくる
 * @param[in] op    IRのタイプ
 * @param[in] lhs   LHS
 * @param[in] rhs   RHS
 * @param[in] name  identifier名
 * @return 作成したIRへのポインタ
 */
static struct ir_t *new_ir(ir_type_t op, int lhs, int rhs, char *name)
{
	struct ir_t *ir = allocate_ir();

	ir->op = op;
	ir->lhs = lhs;
	ir->rhs = rhs;
	ir->name = name;

	return ir;
}

/**
 * @brief generate IR (sub function)
 * @param[in] v    vector
 * @param[in] node node
 * @return LHS
 */
static int gen_ir_sub(struct vector_t *v, struct dict_t *d, struct node_t *node)
{
	static int regno = 0;
	static int label = 0;
	int lhs, rhs;
	int r = regno;
	int l = label;

	if (node == NULL)
		return -1;

	if (node->type == ND_RETURN) {
		lhs = gen_ir_sub(v, d, node->expression);
		vector_push(v, new_ir(IR_RETURN, lhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, lhs, 0, NULL));
	}

	if (node->type == ND_CONST) {
		vector_push(v, new_ir(IR_IMM, regno++, node->value, NULL));
	}

	if (node->type == ND_ASSIGN) {
		rhs = gen_ir_sub(v, d, node->rhs);
		dict_append(d, node->lhs->name, 0);
		lhs = gen_ir_sub(v, d, node->lhs);
		vector_push(v, new_ir(IR_LOADADDR, regno++, -1, node->lhs->name));
		vector_push(v, new_ir(IR_STORE, regno - 1, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, lhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, regno - 1, 0, NULL));
	}

	if (node->type == ND_IDENT) {
		if (dict_lookup(d, node->name) == NULL) {
			error_printf("uninitialized identifier: %s\n", node->name);
			exit(1);
		}
		vector_push(v, new_ir(IR_LOADADDR, regno++, 0, node->name));
		vector_push(v, new_ir(IR_LOAD, regno, regno - 1, NULL));
		vector_push(v, new_ir(IR_KILL, regno - 1, 0, NULL));
		regno++;
		return (regno - 1);
	}

	if (node->type == ND_IF) {
		vector_push(v, new_ir(IR_BEQZ, gen_ir_sub(v, d, node->expression), label++, NULL));
		gen_ir_sub(v, d, node->lhs);	// then

		if (node->rhs != NULL) {
			int l2 = label++;
			vector_push(v, new_ir(IR_JUMP, l2, 0, NULL));
			vector_push(v, new_ir(IR_LABEL, l, 0, NULL));
			gen_ir_sub(v, d, node->rhs);	// else
			vector_push(v, new_ir(IR_LABEL, l2, 0, NULL));
		} else {
			vector_push(v, new_ir(IR_LABEL, l, 0, NULL));
		}
	}

	if (node->type == ND_PLUS || node->type == ND_MINUS ||
	    node->type == ND_MUL  || node->type == ND_DIV) {
		lhs = gen_ir_sub(v, d, node->lhs);
		rhs = gen_ir_sub(v, d, node->rhs);

		vector_push(v, new_ir(CONVERSION_NODE_TO_IR[node->type], lhs, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		return lhs;
	}

	if (node->type == ND_STATEMENT) {
		gen_ir_sub(v, d, node->lhs);
		if (node->rhs != NULL)
			gen_ir_sub(v, d, node->rhs);
	}

	return r;
}

/**
 * @brief 中間表現(IR)を生成する
 */
struct vector_t *gen_ir(struct node_t *node, struct dict_t *d)
{
	struct vector_t *v = NULL;

	v = new_vector();
	gen_ir_sub(v, d, node);

	return v;
}

