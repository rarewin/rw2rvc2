#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief token array
 */
static struct token_t g_tokens[256];
static int g_token_index = 0;

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
 * @brief recursive descendant parser
 */
static int g_position = 0;

/**
 * @brief error with token
 * @param[in] message   a message to be printed out
 * @param[in] position  position of the token where the error occurred
 */
static void error_token(char *message, int position)
{
	fprintf(stderr, "%s: %s\n", message, g_tokens[position].input);
	exit(1);
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

	/* initial allocate */
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

struct ir_t *ins[1000];
int inp = 0;
int regno = 0;

/**
 * @brief new IR
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
 * @param[in] node node
 * @return LHS
 */
int gen_ir_sub(struct node_t *node)
{
	int lhs, rhs;
	int r = regno;

	if (node->type == ND_NUM) {
		ins[inp++] = new_ir(IR_IMM, regno++, node->value);
		return r;
	}

	lhs = gen_ir_sub(node->lhs);
	rhs = gen_ir_sub(node->rhs);

	ins[inp++] = new_ir(CONVERSION_NODE_TO_IR[node->type], lhs, rhs);
	ins[inp++] = new_ir(IR_KILL, rhs, 0);

	return lhs;
}

/**
 * @brief generate IR
 * @param[in] node  node
 * @return 0
 */
int gen_ir(struct node_t *node)
{
	int r;

	r = gen_ir_sub(node);
	ins[inp++] = new_ir(IR_RETURN, r, 0);

	return 0;
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
	static size_t size = 0;

	/* initial allocate */
	if (node_array == NULL) {
		size += ALLOCATE_SIZE;
		node_array = (struct node_t*)malloc(sizeof(struct node_t) * size);
	}

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
	if (g_tokens[g_position].type == TK_NUM) {
		return new_node_num(g_tokens[g_position++].value);
	}

	error_token("expected number", g_position);
	/* NOTREACHED */
	return NULL;
}

/**
 * @brief expression
 */
static struct node_t *expr(void)
{
	struct node_t *lhs = number();

	for (;;) {
		token_type_t op = g_tokens[g_position].type;

		if (op != TK_PLUS && op != TK_MINUS)
			break;

		g_position++;
		lhs = new_node(CONVERSION_TOKEN_TO_NODE[op], lhs, number());
	}

	if (g_tokens[g_position].type != TK_EOF)
		error_token("stray token", g_position);

	return lhs;
}

/**
 * @brief tokenizer
 */
static int tokenize(char *p)
{
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
				g_tokens[g_token_index].type = TK_PLUS;
				break;
			case '-':
				g_tokens[g_token_index].type =  TK_MINUS;
				break;
			default:
				break;
			}
			g_tokens[g_token_index].value = 0;	/* not used */
			g_tokens[g_token_index].input = p;

			g_token_index++;
			p++;

			continue;
		}

		/* number */
		if (isdigit(*p)) {
			g_tokens[g_token_index].type = TK_NUM;
			g_tokens[g_token_index].input = p;
			g_tokens[g_token_index].value = strtol(p, &p, 10);
			g_token_index++;

			continue;
		}

		fprintf(stderr, "tokenize error: %s", p);
		return -1;
	}

	g_tokens[g_token_index].type = TK_EOF;
	g_tokens[g_token_index].input = NULL;
	g_tokens[g_token_index].value = 0;

	return 0;
}

static char *TEMP_REGS[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
			    "a1", "a2", "a3", "a4", "a5", "a6", "a7", NULL};
#define NUM_OF_TEMP_REGS  14  // = sizeof(TEMP_REGS) / ziefof(TEMP_REGS[0]) - 1
static bool used_temp_regs[NUM_OF_TEMP_REGS];
int reg_map[1000];

int find_allocatable_reg(int ir_reg)
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
void allocate_regs(void)
{
	struct ir_t *ir;
	int i;

	/* initialize */
	for (i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++)
		reg_map[i] = -1;

	for (i = 0; i < inp; i++) {
		ir = ins[i];

		switch (ir->op) {
		case IR_IMM:
			ir->lhs = find_allocatable_reg(ir->lhs);
			break;
		case IR_MOV:
		case IR_PLUS:
		case IR_MINUS:
			ir->lhs = find_allocatable_reg(ir->lhs);
			ir->rhs = find_allocatable_reg(ir->rhs);
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
}

#if defined(DEBUG)
static void show_ir(void)
{
#define OP2STR_ELEMENT(e)    [e] = #e
	int i;

	const char *OP2STR[] = {
		OP2STR_ELEMENT(IR_PLUS),
		OP2STR_ELEMENT(IR_MINUS),
		OP2STR_ELEMENT(IR_IMM),
		OP2STR_ELEMENT(IR_MOV),
		OP2STR_ELEMENT(IR_RETURN),
		OP2STR_ELEMENT(IR_KILL),
		OP2STR_ELEMENT(IR_NOP),
	};

	for (i = 0; ins[i] != NULL; i++) {
		printf("%s(%d) %d %d\n",
		       OP2STR[ins[i]->op], ins[i]->op, ins[i]->lhs, ins[i]->rhs);
	}
}
#endif

/**
 * @brief generate assembly
 */
static void gen_riscv(void)
{
	int i;
	struct ir_t *ir;

	for (i = 0; i < inp; i++) {
		ir = ins[i];

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
	if (tokenize(argv[1]))
		return 1;

	node = expr();

	gen_ir(node);

#if defined(DEBUG)
	show_ir();
#endif

	printf(".section .text\n");
	printf(".global main\n");
	printf("main:\n");

	allocate_regs();
	gen_riscv();

	return 0;
}
