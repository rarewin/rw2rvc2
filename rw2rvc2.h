#if !defined(RW2RVC2_H_INCLUDED)
#define RW2RVC2_H_INCLUDED
#include <stdlib.h>

/**
 * @brief Vector type
 */
typedef struct vector_t {
	void **data;
	size_t capacity;
	size_t len;
} vector_t;

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
 * @brief Intermediate Representation
 */
typedef struct ir_t {
	ir_type_t op;
	int lhs;
	int rhs;
} ir_t;


/* parser.c */
/**
 * @brief expression
 * @param[in]  tokens  vector for tokens
 */
struct node_t *expr(struct vector_t *tokens);

/**
 * @brief tokenizer
 * @param[in] p  input stream
 */
struct vector_t *tokenize(char *p);

/**
 * @brief push an element to a vector
 * @param[in] v        vector storing an element
 * @param[in] element  an element to be pushed
 */
void vector_push(struct vector_t *v, void *element);

/**
 * @brief create a new vector
 * @return a created vector
 */
struct vector_t *new_vector(void);


/* codegen.c */

/**
 * @brief generate assembly
 * @param[in] irv  vector of IR
 */
void gen_riscv(struct vector_t *irv);


/* debug.c */

/**
 * @brief debug function for tokenizer
 * @param[in] vector of tokens
 */
void show_token(struct vector_t *tokens);

/**
 * @brief debug function for IR
 */
void show_ir(struct vector_t *irv);


/* regalloc.c */
/**
 * @brief get a temporary register's index
 * @param[in] index for a temporary
 * @return string if the register
 */
char *get_temp_reg_str(int index);

/**
 * @brief allocate register
 * @param[in] irv
 */
void allocate_regs(struct vector_t *irv);

/* ir.c */
/**
 * @brief generate IR
 * @param[in] node  node
 * @return 0
 */
struct vector_t *gen_ir(struct node_t *node);
#endif
