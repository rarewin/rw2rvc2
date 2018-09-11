#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Vector type
 */
typedef struct vector_t {
	void **data;
	size_t capacity;
	size_t len;
} vector_t;

static struct vector_t *g_tokens;
static int g_position = 0;

/**
 * @brief types of tokens
 */
typedef enum {
	TK_PLUS,
	TK_MINUS,
	TK_NUM,
	TK_EOF,
} token_type_t;

/**
 * @brief Token type
 */
typedef struct token_t {
	token_type_t type;
	int value;
	char *input;
} token_t;

/**
 * @brief types of nodes
 */
typedef enum {
	ND_PLUS,
	ND_MINUS,
	ND_NUM,
} node_type_t;

/**
 * @brief conversion table for token type to node type
 */
static node_type_t CONVERSION_TOKEN_TO_NODE[] = {
	[TK_PLUS]  = ND_PLUS,
	[TK_MINUS] = ND_MINUS,
	[TK_NUM]   = ND_NUM,
};

/**
 * @brief Node
 */
typedef struct node_t {
	node_type_t type;
	struct node_t *lhs;
	struct node_t *rhs;
	int value;
} node_t;

/**
 * @brief types of IR
 */
typedef enum {
	IR_PLUS,
	IR_MINUS,
	IR_IMM,
	IR_MOV,
	IR_RETURN,
	IR_KILL,
	IR_NOP,
} ir_type_t;

/**
 * @brief conversion table for node type to IR type
 */
static node_type_t CONVERSION_NODE_TO_IR[] = {
	[ND_PLUS]  = IR_PLUS,
	[ND_MINUS] = IR_MINUS,
};

/**
 * @brief Intermediate Representation
 */
typedef struct ir_t {
	ir_type_t op;
	int lhs;
	int rhs;
} ir_t;

/**
 * @brief allocate memory to a new vector
 * @param[in] old  vector
 */
static struct vector_t *allocate_vector(void)
{
	const size_t ALLOCATE_SIZE = 256;

	static struct vector_t *vector_array = NULL;
	static size_t index = 0;
	static size_t size = 0;

	/* initial allocation */
	if (vector_array == NULL) {
		size += ALLOCATE_SIZE;
		vector_array = (struct vector_t*)malloc(sizeof(struct vector_t) * size);
	}

	if (index >= size) {
		size += ALLOCATE_SIZE;
		vector_array = (struct vector_t*)realloc(vector_array, sizeof(struct vector_t) * size);
	}

	return &vector_array[index++];
}

const size_t VECTOR_DATA_DEFAULT_CAPACITY = 16;

/**
 *
 */
struct vector_t *new_vector(void)
{
	const size_t ALLOCATE_SIZE = 256;

	static void *vector_data_array = NULL;
	static size_t index = 0;
	static size_t size = 0;

	struct vector_t *v = allocate_vector();

	/* initial allocation */
	if (vector_data_array == NULL) {
		size += ALLOCATE_SIZE;
		vector_data_array = (void*)malloc(sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * size);
	}

	if (index >= size) {
		size += ALLOCATE_SIZE;
		vector_data_array = (void*)realloc(vector_data_array, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * size);
	}

	v->capacity = VECTOR_DATA_DEFAULT_CAPACITY;
	v->len = 0;
	v->data = (vector_data_array + VECTOR_DATA_DEFAULT_CAPACITY * index);
	index++;

	return v;
}

/**
 *
 */
void vector_push(struct vector_t *v, void *element)
{
	if (v->len >= v->capacity) {
		if (v->capacity == VECTOR_DATA_DEFAULT_CAPACITY) {
			void *old = v->data;
			v->capacity *= 2;
			v->data = malloc(sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * v->capacity);
			memcpy(v->data, old, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY);
		} else {
			v->capacity *= 2;
			v->data = realloc(v->data, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * v->capacity);
		}
	}

	v->data[v->len++] = element;
}

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
 * @param[in] node  node
 * @return 0
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

/**
 * @brief allocate memory to a new node
 * @return allocated node_t, or NULL if failed
 */
static struct node_t *allocate_node(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct node_t *node_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	/* initial allocate */
	if (node_array == NULL)
		node_array = (struct node_t*)malloc(sizeof(struct node_t) * size);

	/* reallocate */
	if (index >= size) {
		size += ALLOCATE_SIZE;
		node_array = (struct node_t*)realloc(node_array, sizeof(struct node_t) * size);
	}

	return &node_array[index++];
}

/**
 * @brief allocate memory for a node
 * @param[in] op   operation
 * @param[in] lhs  left hand side
 * @param[in] rhs  right hand side
 */
static struct node_t *new_node(node_type_t op, struct node_t *lhs, struct node_t *rhs)
{
	node_t *node = allocate_node();

	node->type = op;
	node->lhs = lhs;
	node->rhs = rhs;

	return node;
}

/**
 * @brief allocate memory for a node of the number
 * @param[in] value  value of the number
 */
static struct node_t *new_node_num(int value)
{
	struct node_t *node = allocate_node();

	node->type = ND_NUM;
	node->lhs = NULL;
	node->rhs = NULL;
	node->value = value;

	return node;
}

/**
 * @brief number
 */
static struct node_t *number(void)
{
	struct token_t *t = g_tokens->data[g_position];

	if (t->type == TK_NUM) {
		g_position++;
		return new_node_num(t->value);
	}

	return NULL;
}

/**
 * @brief expression
 */
static struct node_t *expr(void)
{
	struct node_t *lhs = number();

	for (;;) {
		struct token_t *t = g_tokens->data[g_position];
		token_type_t op = t->type;

		if (op != TK_PLUS && op != TK_MINUS)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, number());
	}

	return lhs;
}

/**
 * @brief allocate memory to the new token
 * @return a pointer to the new token
 */
static struct token_t *allocate_token(void)
{
	const size_t ALLOCATE_SIZE = 256;
	static struct token_t *token_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	if (token_array == NULL)
		token_array = (struct token_t*)malloc(sizeof(struct token_t) * size);

	if (index >= size) {
		size += ALLOCATE_SIZE;
		token_array = (struct token_t*)realloc(token_array, sizeof(struct token_t) * size);
	}

	return &token_array[index++];
}

/**
 * @brief add token
 * @param[in]  v
 * @param[in]  type
 * @param[in]  input
 */
struct token_t *add_token(struct vector_t *v, token_type_t type, char *input)
{
	struct token_t *t = allocate_token();

	t->type = type;
	t->input = input;
	vector_push(v, t);

	return t;
}

/**
 * @brief tokenizer
 * @param[in] p  input stream
 */
static struct vector_t *tokenize(char *p)
{
	struct vector_t *v = new_vector();

	while (*p) {
		/* ignore spaces */
		if (isspace(*p)) {
			p++;
			continue;
		}

		/* arithmetic symbols */
		if (*p == '+' || *p == '-') {
			switch (*p) {
			case '+':
				add_token(v, TK_PLUS, p);
				break;
			case '-':
				add_token(v, TK_MINUS, p);
				break;
			default:
				break;
			}
			p++;
			continue;
		}

		/* number */
		if (isdigit(*p)) {
			struct token_t *t = add_token(v, TK_NUM, p);
			t->value = strtol(p, &p, 10);
			continue;
		}

		fprintf(stderr, "tokenize error: %s", p);
		exit(1);
	}

	add_token(v, TK_EOF, 0);

	return v;
}

/**
 * @brief debug function for tokenizer
 */
#if defined(DEBUG)
void show_token(void)
{
	const char *table[] = {
		[TK_PLUS] = "TK_PLUS",
		[TK_MINUS] = "TK_MINUS",
		[TK_NUM]   = "TK_NUM",
		[TK_EOF]   = "TK_EOF",
	};
	unsigned int i;

	for (i = 0; i < g_tokens->len; i++) {
		printf("%s\n", table[((struct token_t*)(g_tokens->data[i]))->type]);
	}
}
#endif

static char *TEMP_REGS[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
			    "a1", "a2", "a3", "a4", "a5", "a6", "a7", NULL};
#define NUM_OF_TEMP_REGS  14  // = sizeof(TEMP_REGS) / ziefof(TEMP_REGS[0]) - 1
static bool used_temp_regs[NUM_OF_TEMP_REGS];

/**
 * @brief find an allocatable temporary register
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
 * @param[in] irv
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

		switch (ir->op) {
		case IR_IMM:
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
			break;
		case IR_MOV:
		case IR_PLUS:
		case IR_MINUS:
			ir->lhs = find_allocatable_reg(ir->lhs, reg_map);
			ir->rhs = find_allocatable_reg(ir->rhs, reg_map);
			break;
		case IR_RETURN:
			release_reg(reg_map[ir->lhs]);
			break;
		case IR_KILL:
			release_reg(reg_map[ir->lhs]);
			ir->op = IR_NOP;
			break;
		case IR_NOP:
		default:
			break;
		}
	}

	free(reg_map);
}

#if defined(DEBUG)
static void show_ir(struct vector_t *irv)
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
#endif

/**
 * @brief generate assembly
 * @param[in] irv  vector of IR
 */
static void gen_riscv(struct vector_t *irv)
{
	unsigned int i;
	struct ir_t *ir;

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];

		switch (ir->op) {
		case IR_IMM:
			printf("	li	%s, %d\n", TEMP_REGS[ir->lhs], ir->rhs);
			break;
		case IR_MOV:
			printf("	mv	%s, %s\n", TEMP_REGS[ir->lhs], TEMP_REGS[ir->rhs]);
			break;
		case IR_RETURN:
			printf("	mv	a0, %s\n", TEMP_REGS[ir->lhs]);
			printf("	ret\n");
			break;
		case IR_PLUS:
			printf("	add	%s, %s, %s\n", TEMP_REGS[ir->lhs], TEMP_REGS[ir->lhs], TEMP_REGS[ir->rhs]);
			break;
		case IR_MINUS:
			printf("	sub	%s, %s, %s\n", TEMP_REGS[ir->lhs], TEMP_REGS[ir->lhs], TEMP_REGS[ir->rhs]);
			break;
		case IR_NOP:
		case IR_KILL:
		default:
			break;
		}
	}
}

/**
 * @brief main function
 */
int main(int argc, char **argv)
{
	struct node_t *node = NULL;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	/* tokenize */
	g_tokens = tokenize(argv[1]);

#if defined(DEBUG)
	show_token();
#endif

	node = expr();

	struct vector_t *irv = gen_ir(node);

#if defined(DEBUG)
	show_ir(irv);
#endif

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");

	allocate_regs(irv);
	gen_riscv(irv);

	return 0;
}
