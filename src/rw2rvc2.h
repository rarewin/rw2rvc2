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
 * @brief 辞書要素用 構造体
 */
typedef struct dict_element_t {
	char *key;
	void *value;
} dict_element_t;


/**
 * @brief 辞書用 構造体
 */
typedef struct dict_t {
	struct dict_element_t *dict;
	size_t len;
	size_t capacity;
} dict_t;


/**
 * @brief types of tokens
 */
typedef enum {
	TK_PLUS,		/**< + */
	TK_MINUS,		/**< - */
	TK_MUL,			/**< * */
	TK_DIV,			/**< / */
	TK_EQUAL,		/**< = */
	TK_NUM,			/**< 数値  */
	TK_STRING,		/**< 文字列 */
	TK_CHAR,		/**< 文字 */
	TK_SEMICOLON,		/**< ; */
	TK_COLON,		/**< : */
	TK_LEFT_PAREN,		/**< ( */
	TK_RIGHT_PAREN,		/**< ) */
	TK_LEFT_BRACE,		/**< { */
	TK_RIGHT_BRACE,		/**< } */
	TK_DOUBLE_QUOTE,	/**< " */
	TK_SINGLE_QUOTE,	/**< ' */
	TK_IDENT,		/**< 識別子 (変数名等) */
	TK_RETURN,		/**< "return" */
	TK_IF,			/**< "if" */
	TK_ELSE,		/**< "else" */
	TK_GOTO,		/**< "goto" */
	TK_INT,			/**< "int" */
	TK_EOF,			/**< EOF */
} token_type_t;

/**
 * @brief Token type
 */
typedef struct token_t {
	token_type_t type;	/**< token type */
	int value;		/**< token value */
	char *input;		/**< input string */
	char *name;		/**< 識別子等の名前 */
} token_t;

/**
 * @brief types of nodes
 */
typedef enum {
	ND_PLUS,	/**< + */
	ND_MINUS,	/**< - */
	ND_MUL,		/**< * */
	ND_DIV,		/**< / */
	ND_CONST,	/**< 定数 */
	ND_IDENT,	/**< 識別子 */
	ND_SEMICOLON,	/**< ; */
	ND_RETURN,	/**< "return" */
	ND_IF,		/**< "if" */
	ND_STATEMENT,	/**< 文 */
	ND_ASSIGN,	/**< 代入文 */
	ND_TYPE,	/**< 型名 */
	ND_FUNC_DEF,	/**< 関数定義 */
	ND_FUNC_CALL,	/**< 関数コール */
	ND_PARAM,	/**< 関数パラメータ */
} node_type_t;

/**
 * @brief ノード構造体
 */
typedef struct node_t {
	node_type_t type;		/**< タイプ (ND_XXXX) */
	struct node_t *lhs;		/**< 左辺値 */
	struct node_t *rhs;		/**< 右辺値 */
	struct node_t *expression;	/**< 式 */
	char *name;			/**< 識別子等の名前 */
	int value;			/**< 値 */
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
	IR_LOAD,
	IR_STORE,
	IR_LOADADDR,
	IR_BEQZ,	/**< lhs がゼロならブランチする */
	IR_JUMP,	/**< ジャンプする */
	IR_LABEL,	/**< ラベルを生成 */
	IR_FUNC_DEF,	/**< 関数定義 */
	IR_FUNC_END,	/**< 関数定義終端 */
	IR_NOP,
} ir_type_t;

/**
 * @brief Intermediate Representation
 */
typedef struct ir_t {
	ir_type_t op;
	int lhs;
	int rhs;
	char *name;
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


/* codegen.c */

/**
 * @brief RISC-Vのアセンブラを生成する
 * @param[in] irv  中間表現(IR)のVector
 * @param[in] d    辞書
 */
void gen_riscv(struct vector_t *irv, struct dict_t *d);


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
 * @param[in] d     辞書へのポインタ
 * @return 生成されたIRへのポインタ
 */
struct vector_t *gen_ir(struct node_t *node, struct dict_t *d);


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

/* util.c */

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

/**
 * @brief 辞書を新規に作成する
 */
struct dict_t *new_dict(void);

/**
 * @brief 辞書にデータを追加する
 * @param[in] d     辞書
 * @param[in] key   キー
 * @param[in] value 値
 */
void dict_append(struct dict_t *d, char *key, void *value);

/**
 * @brief 辞書からデータを参照する
 * @param[in] d     辞書
 * @param[in] key   キー
 * @return データが存在したら値へのポインタを、存在しなければNULLを返す
 */
struct dict_element_t *dict_lookup(struct dict_t *d, char *key);

/* debug.c */
/**
 * @brief debug function for tokenizer
 * @param[in] vector of tokens
 */
void show_token(struct vector_t *tokens);

/**
 * @brief トークンタイプからトークンを表す文字列を取得する.
 * @param[in] token_type  トークンタイプ.
 * @return トークンを表わす文字列
 */
const char *get_token_str(token_type_t token_type);

/**
 * @brief debug function for IR
 */
void show_ir(struct vector_t *irv);

/**
 * @brief debug function for parser
 */
void show_node(struct node_t *node, unsigned int indent);
#endif
