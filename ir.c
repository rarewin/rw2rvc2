#include "rw2rvc2.h"

/**
 * @brief conversion table for node type to IR type
 */
static node_type_t CONVERSION_NODE_TO_IR[] = {
	[ND_PLUS]  = IR_PLUS,
	[ND_MINUS] = IR_MINUS,
	[ND_MUL] = IR_MUL,
	[ND_DIV] = IR_DIV,
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
 * @brief new IR
 * @param[in] op   operation type of the new IR
 * @param[in] lhs  LHS
 * @param[in] rhs  RHS
 * @return a pointer to the new IR
 */
static struct ir_t *new_ir(ir_type_t op, int lhs, int rhs)
{
	struct ir_t *ir = allocate_ir();

	ir->op = op;
	ir->lhs = lhs;
	ir->rhs = rhs;

	return ir;
}

/**
 * @brief generate IR (sub function)
 * @param[in] v    vector
 * @param[in] node node
 * @return LHS
 */
int gen_ir_sub(struct vector_t *v, struct node_t *node)
{
	static int regno = 0;

	int lhs, rhs;
	int r = regno;

	if (node->type == ND_NUM) {
		vector_push(v, new_ir(IR_IMM, regno++, node->value));
		return r;
	}

	lhs = gen_ir_sub(v, node->lhs);
	rhs = gen_ir_sub(v, node->rhs);

	vector_push(v, new_ir(CONVERSION_NODE_TO_IR[node->type], lhs, rhs));
	vector_push(v, new_ir(IR_KILL, rhs, 0));

	return lhs;
}

/**
 * @brief generate IR
 */
struct vector_t *gen_ir(struct node_t *node)
{
	struct vector_t *v;
	int r;

	v = new_vector();
	r = gen_ir_sub(v, node);
	vector_push(v, new_ir(IR_RETURN, r, 0));

	return v;
}

