/**
 * @brief header file for rw2rvc
 */
#if !defined(RW2RVC2_H_INCLUDED)
#define RW2RVC2_H_INCLUDED
#include <stdlib.h>

/**
 * @brief Vector type
 */
typedef struct vector_t {
	void **data;		/**< pointer to data */
	size_t capacity;	/**< vector capacity */
	size_t len;		/**< current length */
} vector_t;

/**
 * @brief types of tokens
 */
typedef enum {
	TK_PLUS,		/**< + */
	TK_MINUS,		/**< - */
	TK_MUL,			/**< * */
	TK_DIV,			/**< / */
	TK_NUM,			/**< 数値  */
	TK_STRING,		/**< 文字列 */
	TK_CHAR,		/**< 文字 */
	TK_SEMICOLON,		/**< ; */
	TK_COLON,		/**< : */
	TK_OPEN_PAREN,		/**< ( */
	TK_CLOSE_PAREN,		/**< ) */
	TK_DOUBLE_QUOTE,	/**< " */
	TK_SINGLE_QUOTE,	/**< ' */
	TK_RETURN,		/**< "return" */
	TK_GOTO,		/**< "goto" */
	TK_EOF,			/**< EOF */
} token_type_t;

/**
 * @brief Token type
 */
typedef struct token_t {
	token_type_t type;	/**< token type */
	int value;		/**< token value */
	char *input;		/**< input string */
} token_t;

/**
 * @brief types of nodes
 */
typedef enum {
	ND_PLUS,		/**< + */
	ND_MINUS,		/**< - */
	ND_MUL,			/**< * */
	ND_DIV,			/**< / */
	ND_NUM,			/**< numbers */
	ND_SEMICOLON,		/**< ; */
	ND_RETURN,		/**< "return" */
	ND_STATEMENT_LIST,	/**< statement list */
} node_type_t;

/**
 * @brief ノード構造体
 */
typedef struct node_t {
	node_type_t type;		/**< タイプ (ND_XXXX) */
	struct node_t *lhs;		/**< 左辺値 */
	struct node_t *rhs;		/**< 右辺値 */
	struct node_t *expression;	/**< */
	struct vector_t *statements;	/**< 文 */
	int value;
} node_t;

/**
 * @brief types of IR
 */
typedef enum {
	IR_PLUS,
	IR_MINUS,
	IR_MUL,
	IR_DIV,
	IR_RETURN,
	IR_IMM,
	IR_MOV,
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

/**
 * @brief 表示色
 */
typedef enum {
	COL_RED    = 31,
	COL_GREEN  = 32,
	COL_YELLOW = 33,
	COL_BLUE   = 34,
} dprint_color_t;


/* parser.c */
/**
 * @brief main function of parser
 */
struct node_t *parse(struct vector_t *token);

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
 * @brief RISC-Vのアセンブラを生成する
 * @param[in] irv  中間表現(IR)のVector
 */
void gen_riscv(struct vector_t *irv);


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
 * @brief 中間表現(IR)を生成する
 * @param[in] node  ノードへのポインタ
 * @return 生成されたIRへのポインタ
 */
struct vector_t *gen_ir(struct node_t *node);


/* display.c */
/**
 * @brief 文字を色付きで標準出力する
 * @param[in] color   表示色
 * @param[in] format  表示フォーマット
 * @return 出力した文字数
 */
int color_printf(dprint_color_t color, const char *format, ...);

/**
 * @brief エラー表示をエラー出力に吐き出す
 * @param[in] format  表示フォーマット
 * @return 出力した文字数
 */
int error_printf(const char *format, ...);

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

/**
 * @brief debug function for parser
 */
void show_node(struct node_t *node, unsigned int indent);
#endif
