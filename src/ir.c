#include "rw2rvc2.h"

/**
 * @brief conversion table for node type to IR type
 */
static ir_type_t CONVERSION_NODE_TO_IR[] = {
	[ND_PLUS] = IR_PLUS,	//
	[ND_MINUS] = IR_MINUS,      //
	[ND_MUL] = IR_MUL,	  //
	[ND_DIV] = IR_DIV,	  //
	[ND_MOD] = IR_MOD,	  //
	[ND_RETURN] = IR_RETURN,    //
	[ND_OR_OP] = IR_OR,	 //
	[ND_AND] = IR_AND,	  //
	[ND_OR] = IR_OR,	    //
	[ND_XOR] = IR_XOR,
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

	/* initial allocation */
	if (ir_array == NULL || index >= ALLOCATE_SIZE) {
		if ((ir_array = (struct ir_t *)malloc(sizeof(struct ir_t) * ALLOCATE_SIZE)) == NULL) {
			error_printf("memmory allocation failed\n");
			exit(1);
		}
		index = 0;
	}

	index++;

	return &ir_array[index - 1];
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
 * @brief 新しい変数データにメモリを割り当てる
 * @param[in] node    変数ノード
 * @param[in] slevel  スコープレベル
 */
static struct variable_t *new_variable(struct node_t *node, int slevel)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct variable_t *var_array = NULL;
	static size_t index = 0;

	/* assert */
	if (node == NULL || node->type != ND_IDENT /* @TODO たぶんおかしい*/) {
		error_printf("unexpected error in %s() (unexpected node type: %d)\n", __FUNCTION__,
			     (node == NULL) ? -1 : node->type);
		exit(1);
	}

	if (var_array == NULL || index >= ALLOCATE_SIZE) {
		if ((var_array = (struct variable_t *)malloc(sizeof(struct variable_t) * ALLOCATE_SIZE)) == NULL) {
			error_printf("memory allocation error\n");
			exit(1);
		}
		index = 0;
	}

	var_array[index].node = node;
	var_array[index].scope_level = slevel;

	index++;

	return &var_array[index - 1];
}

/**
 * @brief IR生成 サブ関数
 * @param[in] v            IRのベクタ
 * @param[in] d            変数の辞書
 * @param[in] node         パースしたノード
 * @param[in] scode_level  スコープレベル
 * @return 上段に渡す結果レジスタ, もしくはそれに相当する値
 */
static int gen_ir_sub(struct vector_t *v, struct dict_t *d, struct node_t *node, int scope_level)
{
	static int regno = 0;
	static int label = 0;
	struct node_t *n = NULL;
	int lhs, rhs;
	int r = regno;
	int l = label;
	int i = 0;
	size_t j = 0;

	if (node == NULL)
		return -1;

	if (node->type == ND_PROGRAM || node->type == ND_COMPOUND_STATEMENTS) {

		for (j = 0; j < node->list->len; j++)
			gen_ir_sub(v, d, node->list->data[j], scope_level + 1);

		return -1;
	}

	/* 静的変数の宣言 */
	if (node->type == ND_VAR_DEC_STATIC) {
		/* node->lhs: 型, node->rhs: INIT_DLIST  */

		/* 型名だけの行は何もしない */
		if (node->rhs == NULL)
			return -1;

		if (node->rhs->type != ND_VAR_INIT_DLIST) {
			error_printf("unexpected error\n");
			exit(1);
		}

		for (j = 0; j < node->rhs->list->len; j++) {
			n = node->rhs->list->data[j];
			dict_append(d, n->name, new_variable(n, 0));

#if 0
			if (n->rhs != NULL) {
				rhs = gen_ir_sub(v, d, n->rhs, scope_level);
				vector_push(v, new_ir(IR_LOADADDR, regno++, -1, n->name));
				vector_push(v, new_ir(IR_STORE, regno - 1, rhs, NULL));
				vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
				vector_push(v, new_ir(IR_KILL, regno - 1, 0, NULL));
			}
#endif
		}

		return -1;
	}

	/* ローカル変数の宣言 */
	if (node->type == ND_VAR_DEC) {
		error_printf("local variable is not supported yet... %s\n", node->name);
		exit(1);
		/* NOTREACHED */
	}

	if (node->type == ND_RETURN) {
		lhs = gen_ir_sub(v, d, node->expression, scope_level);
		vector_push(v, new_ir(IR_RETURN, lhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, lhs, 0, NULL));
		return r;
	}

	if (node->type == ND_CONST) {
		vector_push(v, new_ir(IR_IMM, regno++, node->value, NULL));
		return r;
	}

	if (node->type == ND_ASSIGN) {
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);
		lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		vector_push(v, new_ir(IR_LOADADDR, regno++, -1, node->lhs->name));
		vector_push(v, new_ir(IR_STORE, regno - 1, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, lhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		vector_push(v, new_ir(IR_KILL, regno - 1, 0, NULL));
		return -1;
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
		lhs = gen_ir_sub(v, d, node->condition, scope_level);
		vector_push(v, new_ir(IR_BEQZ, lhs, label++, NULL));
		vector_push(v, new_ir(IR_KILL, lhs, 0, NULL));

		gen_ir_sub(v, d, node->consequence, scope_level);    // then

		if (node->alternative != NULL) {
			int l2 = label++;
			vector_push(v, new_ir(IR_JUMP, l2, 0, NULL));
			vector_push(v, new_ir(IR_LABEL, l, 0, NULL));
			gen_ir_sub(v, d, node->alternative, scope_level);    // else
			vector_push(v, new_ir(IR_LABEL, l2, 0, NULL));
		} else {
			vector_push(v, new_ir(IR_LABEL, l, 0, NULL));
		}
	}

	if (node->type == ND_PLUS || node->type == ND_MINUS || node->type == ND_MUL || node->type == ND_DIV ||
	    node->type == ND_MOD || node->type == ND_OR_OP || node->type == ND_AND || node->type == ND_OR ||
	    node->type == ND_XOR) {

		if (node->lhs != NULL) {
			lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		} else {
			if (node->type == ND_PLUS || node->type == ND_MINUS) {
				lhs = regno;
				vector_push(v, new_ir(IR_IMM, regno++, 0, NULL));
			} else {
				error_printf("unexpected error\n");
				exit(1);
			}
		}
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);

		vector_push(v, new_ir(CONVERSION_NODE_TO_IR[node->type], lhs, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		return lhs;
	}

	if (node->type == ND_AND_OP) {
		/* a && b = ~(~a || ~b) */
		lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);

		vector_push(v, new_ir(IR_NOT, lhs, 0, NULL));
		vector_push(v, new_ir(IR_NOT, rhs, 0, NULL));
		vector_push(v, new_ir(IR_OR, lhs, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		vector_push(v, new_ir(IR_NOT, lhs, 0, NULL));
		return lhs;
	}

	if (node->type == ND_EQ_OP || node->type == ND_NE_OP) {
		lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);

		vector_push(v, new_ir(IR_MINUS, lhs, rhs, NULL));

		if (node->type == ND_EQ_OP) {
			vector_push(v, new_ir(IR_NOT, lhs, 0, NULL));
		}

		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
		return lhs;
	}

	if (node->type == ND_LESS_OP || node->type == ND_GREATER_OP || node->type == ND_LE_OP ||
	    node->type == ND_GE_OP) {
		lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);

		if (node->type == ND_GREATER_OP || node->type == ND_LE_OP) {
			int tmp;
			tmp = lhs;
			lhs = rhs;
			rhs = tmp; /* swap */
		}

		if (node->type == ND_LESS_OP || node->type == ND_GREATER_OP)
			vector_push(v, new_ir(IR_SLT, lhs, rhs, NULL));
		else
			vector_push(v, new_ir(IR_SLET, lhs, rhs, NULL));

		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));

		return lhs;
	}

	if (node->type == ND_LEFT_OP || node->type == ND_RIGHT_OP) {
		lhs = gen_ir_sub(v, d, node->lhs, scope_level);
		rhs = gen_ir_sub(v, d, node->rhs, scope_level);

		vector_push(v, new_ir((node->type == ND_LEFT_OP) ? IR_LEFT_OP : IR_RIGHT_OP, lhs, rhs, NULL));
		vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));

		return lhs;
	}

	if (node->type == ND_EXPRESSION) {
		return gen_ir_sub(v, d, node->expression, scope_level);
	}

	if (node->type == ND_FUNC_DEF) {
		vector_push(v, new_ir(IR_FUNC_DEF, -1, -1, node->lhs->lhs->name));

		/* for parameters */
		if (node->lhs->lhs->parameter_list != NULL) {
			for (j = 0; j < node->lhs->lhs->parameter_list->len; j++) {
				n = node->lhs->lhs->parameter_list->data[j];
				dict_append(d, n->rhs->name, new_variable(n->rhs, scope_level));
				vector_push(v, new_ir(IR_LOADADDR, regno++, -1, n->rhs->name));
				vector_push(v, new_ir(IR_FUNC_PARAM, regno - 1, i, n->rhs->name));
				regno++; /* arg reg用の番号を確保……  */
				vector_push(v, new_ir(IR_KILL, regno - 2, -1, NULL));
				vector_push(v, new_ir(IR_KILL_ARG, i, -1, NULL));
				i++;
			}
		}

		vector_push(v, new_ir(IR_FUNC_END, gen_ir_sub(v, d, node->rhs, scope_level), -1, node->lhs->lhs->name));

		return -1;
	}

	if (node->type == ND_FUNC_CALL) {
		if (node->list != NULL) {
			for (j = 0; j < node->list->len; j++) {
				n = node->list->data[j];

				if (n->type == ND_FUNC_ARG) {
					rhs = gen_ir_sub(v, d, n->lhs, scope_level);
					vector_push(v, new_ir(IR_FUNC_ARG, n->value, rhs, NULL));
					vector_push(v, new_ir(IR_KILL, rhs, 0, NULL));
					vector_push(v, new_ir(IR_KILL_ARG, n->value, 0, NULL));
				} else {
					error_printf("unexpected node\n");
				}
			}
		}
		vector_push(v, new_ir(IR_FUNC_CALL, regno++, -1, node->name));
		return r;
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
	gen_ir_sub(v, d, node, 0);

	return v;
}
