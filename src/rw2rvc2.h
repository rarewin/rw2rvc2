/**
 * @brief header file for rw2rvc
 */
#if !defined(RW2RVC2_H_INCLUDED)
#define RW2RVC2_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>

#define ASM_COMMENTOUT_STR	"# "

/**
 * @brief ベクター タイプ
 */
typedef struct vector_t {
	void **data;		/**< データポインタ(void*)へのポインタ */
	size_t capacity;	/**< ベクターの容量  */
	size_t len;		/**< ベクターの現在の長さ */
} vector_t;


/**
 * @brief 辞書要素用 構造体
 */
typedef struct dict_element_t {
	char *key;	/**< キー */
	void *value;	/**< 値 */
} dict_element_t;


/**
 * @brief 辞書用 構造体
 */
typedef struct dict_t {
	struct dict_element_t *dict;	/**< 辞書要素へのポインタ */
	size_t len;			/**< 辞書の現在の長さ */
	size_t capacity;		/**< ベクターの容量 */
} dict_t;


/**
 * @brief トークンタイプ
 */
typedef enum {
	TK_PLUS,		/**< + */
	TK_MINUS,		/**< - */
	TK_MUL,			/**< * */
	TK_DIV,			/**< / */
	TK_MOD,			/**< % */
	TK_EQUAL,		/**< = */
	TK_OR,			/**< | */
	TK_AND,			/**< & */
	TK_XOR,			/**< ^ */
	TK_NOT,			/**< ! */
	TK_INV,			/**< ~ */
	TK_MUL_ASSIGN,		/**< *= */
	TK_DIV_ASSIGN,		/**< /= */
	TK_MOD_ASSIGN,		/**< %= */
	TK_ADD_ASSIGN,		/**< += */
	TK_SUB_ASSIGN,		/**< -= */
	TK_LEFT_ASSIGN,		/**< <<= */
	TK_RIGHT_ASSIGN,	/**< >>= */
	TK_OR_OP,		/**< || */
	TK_AND_OP,		/**< && */
	TK_EQ_OP,		/**< == */
	TK_NE_OP,		/**< != */
	TK_GREATER_OP,		/**< > */
	TK_LESS_OP,		/**< < */
	TK_GE_OP,		/** >= */
	TK_LE_OP,		/** <= */
	TK_LEFT_OP,		/** << */
	TK_RIGHT_OP,		/** >> */
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
	TK_COMMA,		/**< , */
	TK_IDENT,		/**< 識別子 (変数名等) */
	TK_RETURN,		/**< "return" */
	TK_IF,			/**< "if" */
	TK_ELSE,		/**< "else" */
	TK_GOTO,		/**< "goto" */
	TK_INT,			/**< "int" */
	TK_EOF,			/**< EOF */
	TK_INVALID,		/**< 不正な値(関数のエラー用) */
} token_type_t;

/**
 * @brief トークン
 */
typedef struct token_t {
	token_type_t type;	/**< タイプ */
	int value;		/**< 値  */
	char *input;		/**< 入力文字列 */
	char *name;		/**< 識別子等の名前 */
	int line;		/**< 行番号 */
	int position;		/**< その行での位置 */
} token_t;

/**
 * @brief ノードタイプ
 */
typedef enum {
	ND_PLUS,	/**< + */
	ND_MINUS,	/**< - */
	ND_MUL,		/**< * */
	ND_DIV,		/**< / */
	ND_MOD,		/**< % */
	ND_OR,		/**< | */
	ND_AND,		/**< & */
	ND_XOR,		/**< ^ */
	ND_CONST,	/**< 定数 */
	ND_IDENT,	/**< 識別子 */
	ND_SEMICOLON,	/**< ; */
	ND_RETURN,	/**< "return" */
	ND_IF,		/**< "if" */
	ND_THEN_ELSE,	/**< then-else */
	ND_STATEMENT,	/**< 文 */
	ND_EXPRESSION,	/**< 式 */
	ND_ASSIGN,	/**< 代入文 */
	ND_OR_OP,	/**< OR */
	ND_AND_OP,	/**< AND */
	ND_EQ_OP,	/**< == */
	ND_NE_OP,	/**< != */
	ND_GREATER_OP,	/**< > */
	ND_LESS_OP,	/**< < */
	ND_GE_OP,	/**< >= */
	ND_LE_OP,	/**< <= */
	ND_RIGHT_OP,	/**< >> */
	ND_LEFT_OP,	/**< << */
	ND_TYPE,	/**< 型名 */
	ND_FUNC_DEF,	/**< 関数定義 */
	ND_FUNC_CALL,	/**< 関数コール */
	ND_DECLARATION,	/**< 宣言文 */
	ND_FUNC_ARG,	/**< 関数引数 */
	ND_FUNC_PLIST,	/**< 関数パラメータリスト */
	ND_FUNC_PARAM,	/**< 関数パラメータ */
} node_type_t;

/**
 * @brief ノード構造体
 */
typedef struct node_t {
	node_type_t type;	/**< タイプ (ND_XXXX) */
	struct node_t *lhs;	/**< 左辺値 */
	struct node_t *rhs;	/**< 右辺値 */
	char *name;		/**< 識別子等の名前 */
	int value;		/**< 値 */
} node_t;

/**
 * @brief 中間表現(IR)タイプ
 */
typedef enum {
	IR_PLUS,	/**< 加算: lhs + rhs */
	IR_MINUS,	/**< 減算: lhs - rhs */
	IR_MUL,		/**< 乗算: lhs * rhs*/
	IR_DIV,		/**< 除算: lhs / rhs */
	IR_MOD,		/**< 剰余: lhs % rhs */
	IR_AND,		/**< 論理積: lhs & rhs */
	IR_OR,		/**< 論理和: lhs | rhs  */
	IR_NOT,		/**< 論理否定: ~lhs  */
	IR_XOR,		/**< 排他的論理和: lhs ^ rhs  */
	IR_EQ_OP,	/**< 等号: lhs == rhs  */
	IR_NE_OP,	/**< 否定等号: lhs != rhs */
	IR_SLT,		/**< 不等号: lhs < rhs */
	IR_SLET,	/**< 不等号: lhs <= rhs */
	IR_LEFT_OP,	/**< 不等号: lhs << rhs */
	IR_RIGHT_OP,	/**< 不等号: lhs >> rhs */
	IR_RETURN,	/**< return */
	IR_IMM,
	IR_MOV,
	IR_KILL,
	IR_KILL_ARG,	/**< アーギュメントレジスタを解放する */
	IR_LOAD,
	IR_STORE,
	IR_LOADADDR,
	IR_BEQZ,	/**< lhs がゼロならブランチする */
	IR_JUMP,	/**< ジャンプする */
	IR_LABEL,	/**< ラベルを生成 */
	IR_FUNC_DEF,	/**< 関数定義 */
	IR_FUNC_CALL,	/**< 関数呼び出し */
	IR_FUNC_END,	/**< 関数定義終端 */
	IR_FUNC_ARG,	/**< 関数引数 */
	IR_FUNC_PLIST,	/**< 関数パラメータリスト */
	IR_FUNC_PARAM,	/**< 関数パラメータ */
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
#define NUM_OF_TEMP_REGS  15  // = sizeof(TEMP_REGS) / sizeof(TEMP_REGS[0]) - 1

typedef struct using_regs_list_t {
	int num;
	int list[NUM_OF_TEMP_REGS];
} using_regs_list_t;

/**
 * @brief get a temporary register's index
 * @param[in] index for a temporary
 * @return string if the register
 */
char *get_temp_reg_str(int index);

/**
 * @brief 使用中レジスタリストを取得する
 * @note 複数回コールした場合, リストは最後の結果で上書かれます
 * @return 使用中のレジスタの番号のリスト
 */
struct using_regs_list_t *get_using_regs(int index);

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
 * @param[out] file    出力先
 * @param[in]  color   表示色
 * @param[in]  format  表示フォーマット
 * @return 出力した文字数
 */
int color_printf(FILE *file, dprint_color_t color, const char *format, ...);

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
 * @brief トークナイザーの出力を表示する
 * @param[out] file   出力先
 * @param[in]  tokens トークンベクター
 */
void show_token(FILE *file, struct vector_t *tokens);

/**
 * @brief トークンタイプからトークンを表す文字列を取得する.
 * @param[in] token_type  トークンタイプ.
 * @return トークンを表わす文字列
 */
const char *get_token_str(token_type_t token_type);

/**
 * @brief IRの出力を表示する
 * @param[out] file  出力先
 * @param[in]  irv   IRベクター
 */
void show_ir(FILE *file, struct vector_t *irv);

/**
 * @brief パーサーの出力を表示する
 * @param[out] file   出力先
 * @param[in]  node   ノードデータ
 * @param[in]  indent インデント段数
 */
void show_node(FILE *file, struct node_t *node, unsigned int indent);
#endif
