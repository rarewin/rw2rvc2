#include <stdio.h>

#include "rw2rvc2.h"

#define TRANS_ELEMENT(e)    [e] = #e



/**
 * @brief debug function for tokenizer
 */
void show_token(struct vector_t *tokens)
{
	unsigned int i;

	for (i = 0; i < tokens->len; i++) {
		token_type_t tt = ((struct token_t*)(tokens->data[i]))->type;
		printf("%02d: %s(%d)\n", i, get_token_str(tt), tt);
	}
}

/**
 * @brief トークンタイプからトークンを表す文字列を取得する.
 *
 * @todo 引数チェック.
 */
const char *get_token_str(token_type_t token_type)
{
	const char *table[] = {
		TRANS_ELEMENT(TK_PLUS),		/**< + */
		TRANS_ELEMENT(TK_MINUS),	/**< - */
		TRANS_ELEMENT(TK_MUL),		/**< * */
		TRANS_ELEMENT(TK_DIV),		/**< / */
		TRANS_ELEMENT(TK_NUM),		/**< 数値  */
		TRANS_ELEMENT(TK_EQUAL),	/**< = */
		TRANS_ELEMENT(TK_STRING),	/**< 文字列 */
		TRANS_ELEMENT(TK_CHAR),		/**< 文字 */
		TRANS_ELEMENT(TK_SEMICOLON),	/**< ; */
		TRANS_ELEMENT(TK_LEFT_PAREN),	/**< ( */
		TRANS_ELEMENT(TK_RIGHT_PAREN),	/**< ) */
		TRANS_ELEMENT(TK_DOUBLE_QUOTE),	/**< " */
		TRANS_ELEMENT(TK_SINGLE_QUOTE),	/**< ' */
		TRANS_ELEMENT(TK_IDENT),	/**< 識別子 (変数名等) */
		TRANS_ELEMENT(TK_RETURN),	/**< "return" */
		TRANS_ELEMENT(TK_IF),		/**< "if" */
		TRANS_ELEMENT(TK_ELSE),		/**< "else" */
		TRANS_ELEMENT(TK_GOTO),		/**< "goto" */
		TRANS_ELEMENT(TK_EOF),		/**< EOF */
	};

	return table[token_type];
}

/**
 * @breif @p indent 文字だけ空白を標準出力する
 * @param[in] indent  インデント量
 */
static void print_indent(unsigned int indent)
{
	unsigned int i;

	for (i = 0; i < indent + 1; i++)
		putchar(' ');
}

/**
 * @brief debug function for parser
 */
void show_node(struct node_t *node, unsigned int indent)
{
	const char *table[] = {
		TRANS_ELEMENT(ND_PLUS),			/**< + */
		TRANS_ELEMENT(ND_MINUS),		/**< - */
		TRANS_ELEMENT(ND_MUL),			/**< * */
		TRANS_ELEMENT(ND_DIV),			/**< / */
		TRANS_ELEMENT(ND_IDENT),		/**< 識別子 */
		TRANS_ELEMENT(ND_CONST),		/**< numbers */
		TRANS_ELEMENT(ND_SEMICOLON),		/**< ; */
		TRANS_ELEMENT(ND_RETURN),		/**< "return" */
		TRANS_ELEMENT(ND_IF),			/**< "if" */
		TRANS_ELEMENT(ND_STATEMENT_LIST),	/**< statement list */
		TRANS_ELEMENT(ND_ASSIGN),		/**< 代入文 */
	};

	size_t i;

	if (node == NULL)
		return;

	print_indent(indent);

	printf("%s: %d\n", table[node->type], node->value);

	if (node->statements != NULL) {
		print_indent(indent + 1);
		color_printf(COL_GREEN, "statements:\n");
		for (i = 0; i < node->statements->len; i++)
			show_node(node->statements->data[i], indent + 2);
	}

	if (node->expression != NULL) {
		print_indent(indent + 1);
		color_printf(COL_GREEN, "expression:\n");
		show_node(node->expression, indent + 2);
	}

	if (node->name != NULL) {
		print_indent(indent + 1);
		color_printf(COL_GREEN, "name: ");
		printf("%s\n", node->name);
	}

	if (node->lhs != NULL) {
		print_indent(indent + 1);
		color_printf(COL_GREEN, "lhs:\n");
		show_node(node->lhs, indent + 1);
	}

	if (node->rhs != NULL) {
		print_indent(indent + 1);
		color_printf(COL_GREEN, "rhs:\n");
		show_node(node->rhs, indent + 1);
	}
}

/**
 * @brief debug function for IR
 */
void show_ir(struct vector_t *irv)
{
	struct ir_t *ir;
	unsigned int i;

	const char *OP2STR[] = {
		TRANS_ELEMENT(IR_PLUS),
		TRANS_ELEMENT(IR_MINUS),
		TRANS_ELEMENT(IR_MUL),
		TRANS_ELEMENT(IR_DIV),
		TRANS_ELEMENT(IR_IMM),
		TRANS_ELEMENT(IR_MOV),
		TRANS_ELEMENT(IR_RETURN),
		TRANS_ELEMENT(IR_KILL),
		TRANS_ELEMENT(IR_LOAD),
		TRANS_ELEMENT(IR_STORE),
		TRANS_ELEMENT(IR_LOADADDR),
		TRANS_ELEMENT(IR_NOP),
		TRANS_ELEMENT(IR_BEQZ),		/**< lhs がゼロならブランチする */
		TRANS_ELEMENT(IR_JUMP),		/**< ジャンプする */
		TRANS_ELEMENT(IR_LABEL),	/**< ラベルを生成 */
	};

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];
		printf("%s(%d) %d %d %s\n",
		       OP2STR[ir->op], ir->op, ir->lhs, ir->rhs, ir->name);
	}
}
