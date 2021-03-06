#include "rw2rvc2.h"

#define TRANS_ELEMENT(e) [e] = #e

/**
 * @brief トークナイザーの出力を表示する
 */
void show_token(FILE *file, struct vector_t *tokens)
{
	unsigned int i;

	for (i = 0; i < tokens->len; i++) {
		token_type_t tt = ((struct token_t *)(tokens->data[i]))->type;
		fprintf(file, ASM_COMMENTOUT_STR "%02d: %s(%d)\n", i, get_token_str(tt), tt);
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
		TRANS_ELEMENT(TK_MOD),		/**< % */
		TRANS_ELEMENT(TK_EQUAL),	/**< = */
		TRANS_ELEMENT(TK_OR),		/**< | */
		TRANS_ELEMENT(TK_AND),		/**< & */
		TRANS_ELEMENT(TK_XOR),		/**< ^ */
		TRANS_ELEMENT(TK_NOT),		/**< ! */
		TRANS_ELEMENT(TK_INV),		/**< ~ */
		TRANS_ELEMENT(TK_MUL_ASSIGN),   /**< *= */
		TRANS_ELEMENT(TK_DIV_ASSIGN),   /**< /= */
		TRANS_ELEMENT(TK_MOD_ASSIGN),   /**< %= */
		TRANS_ELEMENT(TK_ADD_ASSIGN),   /**< += */
		TRANS_ELEMENT(TK_SUB_ASSIGN),   /**< -= */
		TRANS_ELEMENT(TK_LEFT_ASSIGN),  /**< <<= */
		TRANS_ELEMENT(TK_RIGHT_ASSIGN), /**< >>= */
		TRANS_ELEMENT(TK_OR_OP),	/**< || */
		TRANS_ELEMENT(TK_AND_OP),       /**< && */
		TRANS_ELEMENT(TK_EQ_OP),	/**< == */
		TRANS_ELEMENT(TK_NE_OP),	/**< != */
		TRANS_ELEMENT(TK_GREATER_OP),   /**< > */
		TRANS_ELEMENT(TK_LESS_OP),      /**< < */
		TRANS_ELEMENT(TK_GE_OP),	/** >= */
		TRANS_ELEMENT(TK_LE_OP),	/** <= */
		TRANS_ELEMENT(TK_LEFT_OP),      /** << */
		TRANS_ELEMENT(TK_RIGHT_OP),     /** >> */
		TRANS_ELEMENT(TK_NUM),		/**< 数値  */
		TRANS_ELEMENT(TK_STRING),       /**< 文字列 */
		TRANS_ELEMENT(TK_CHAR),		/**< 文字 */
		TRANS_ELEMENT(TK_SEMICOLON),    /**< ; */
		TRANS_ELEMENT(TK_LEFT_PAREN),   /**< ( */
		TRANS_ELEMENT(TK_RIGHT_PAREN),  /**< ) */
		TRANS_ELEMENT(TK_LEFT_BRACE),   /**< { */
		TRANS_ELEMENT(TK_RIGHT_BRACE),  /**< } */
		TRANS_ELEMENT(TK_DOUBLE_QUOTE), /**< " */
		TRANS_ELEMENT(TK_SINGLE_QUOTE), /**< ' */
		TRANS_ELEMENT(TK_COMMA),	/**< , */
		TRANS_ELEMENT(TK_IDENT),	/**< 識別子 (変数名等) */
		TRANS_ELEMENT(TK_RETURN),       /**< "return" */
		TRANS_ELEMENT(TK_IF),		/**< "if" */
		TRANS_ELEMENT(TK_ELSE),		/**< "else" */
		TRANS_ELEMENT(TK_GOTO),		/**< "goto" */
		TRANS_ELEMENT(TK_INT),		/**< "int" */
		TRANS_ELEMENT(TK_EOF),		/**< EOF */
		TRANS_ELEMENT(TK_INVALID),      /**< 不正な値(関数のエラー用) */
	};

	return (token_type < TK_INVALID) ? table[token_type] : "INVALID";
}

/**
 * @breif @p indent 文字だけ空白を標準出力する
 * @param[out] file    出力先
 * @param[in]  indent  インデント量
 */
static void print_indent(FILE *file, unsigned int indent)
{
	unsigned int i;

	for (i = 0; i < indent + 1; i++)
		fputc(' ', file);
}

/**
 * @brief パーサーの出力を表示する
 * @param[out] file   出力先
 * @param[in]  node   ノードデータ
 * @param[in]  indent インデント段数
 */
void show_node(FILE *file, struct node_t *node, unsigned int indent)
{
	const char *table[] = {
		TRANS_ELEMENT(ND_PLUS),		       /**< + */
		TRANS_ELEMENT(ND_MINUS),	       /**< - */
		TRANS_ELEMENT(ND_MUL),		       /**< * */
		TRANS_ELEMENT(ND_DIV),		       /**< / */
		TRANS_ELEMENT(ND_MOD),		       /**< % */
		TRANS_ELEMENT(ND_OR),		       /**< | */
		TRANS_ELEMENT(ND_AND),		       /**< & */
		TRANS_ELEMENT(ND_XOR),		       /**< ^ */
		TRANS_ELEMENT(ND_CONST),	       /**< 定数 */
		TRANS_ELEMENT(ND_IDENT),	       /**< 識別子 */
		TRANS_ELEMENT(ND_RETURN),	      /**< "return" */
		TRANS_ELEMENT(ND_IF),		       /**< "if" */
		TRANS_ELEMENT(ND_COMPOUND_STATEMENTS), /**< 複合文 */
		TRANS_ELEMENT(ND_EXPRESSION),	  /**< 式 */
		TRANS_ELEMENT(ND_ASSIGN),	      /**< 代入文 */
		TRANS_ELEMENT(ND_OR_OP),	       /**< OR */
		TRANS_ELEMENT(ND_AND_OP),	      /**< AND */
		TRANS_ELEMENT(ND_EQ_OP),	       /**< == */
		TRANS_ELEMENT(ND_NE_OP),	       /**< != */
		TRANS_ELEMENT(ND_GREATER_OP),	  /**< > */
		TRANS_ELEMENT(ND_LESS_OP),	     /**< < */
		TRANS_ELEMENT(ND_GE_OP),	       /**< >= */
		TRANS_ELEMENT(ND_LE_OP),	       /**< <= */
		TRANS_ELEMENT(ND_RIGHT_OP),	    /**< >> */
		TRANS_ELEMENT(ND_LEFT_OP),	     /**< << */
		TRANS_ELEMENT(ND_TYPE),		       /**< 型名 */
		TRANS_ELEMENT(ND_VAR_DEC),	     /**< 変数宣言 */
		TRANS_ELEMENT(ND_VAR_DEC_STATIC),      /**< 静的変数制限 */
		TRANS_ELEMENT(ND_VAR_INIT_DLIST),      /**< 初期宣言リスト  */
		TRANS_ELEMENT(ND_FUNC_DEF),	    /**< 関数定義 */
		TRANS_ELEMENT(ND_FUNC_CALL),	   /**< 関数コール */
		TRANS_ELEMENT(ND_FUNC_ARG),	    /**< 関数引数 */
		TRANS_ELEMENT(ND_FUNC_PARAM),	  /**< 関数パラメータ */
		TRANS_ELEMENT(ND_PROGRAM),	     /**< プログラム (スタートポイント) */
	};
	size_t i;

	if (node == NULL)
		return;

	fprintf(file, ASM_COMMENTOUT_STR);
	print_indent(file, indent);

	fprintf(file, "%s: %d\n", table[node->type], node->value);

	if (node->list != NULL) {
		for (i = 0; i < node->list->len; i++) {
			fprintf(file, ASM_COMMENTOUT_STR);
			print_indent(file, indent + 1);
			color_printf(file, COL_GREEN, "list%d:\n", i);
			show_node(file, node->list->data[i], indent + 1);
		}
	}

	if (node->parameter_list != NULL) {
		for (i = 0; i < node->parameter_list->len; i++) {
			fprintf(file, ASM_COMMENTOUT_STR);
			print_indent(file, indent + 1);
			color_printf(file, COL_GREEN, "parameter_list%d:\n", i);
			show_node(file, node->parameter_list->data[i], indent + 1);
		}
	}

	if (node->condition != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "condition:\n");
		show_node(file, node->condition, indent + 1);
	}

	if (node->consequence != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "consequence:\n");
		show_node(file, node->consequence, indent + 1);
	}

	if (node->alternative != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "alternative:\n");
		show_node(file, node->alternative, indent + 1);
	}

	if (node->name != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "name: ");
		fprintf(file, "%s\n", node->name);
	}

	if (node->lhs != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "lhs:\n");
		show_node(file, node->lhs, indent + 1);
	}

	if (node->rhs != NULL) {
		fprintf(file, ASM_COMMENTOUT_STR);
		print_indent(file, indent + 1);
		color_printf(file, COL_GREEN, "rhs:\n");
		show_node(file, node->rhs, indent + 1);
	}
}

/**
 * @brief IRの出力を表示する
 * @param[out] file  出力先
 * @param[in]  irv   IRベクター
 */
void show_ir(FILE *file, struct vector_t *irv)
{
	struct ir_t *ir;
	unsigned int i;
	int j;
	struct using_regs_list_t *using_regs;

	const char *OP2STR[] = {
		TRANS_ELEMENT(IR_PLUS),	//
		TRANS_ELEMENT(IR_MINUS),       //
		TRANS_ELEMENT(IR_MUL),	 //
		TRANS_ELEMENT(IR_DIV),	 //
		TRANS_ELEMENT(IR_MOD),	 /**< 剰余を求める a0 % a1 */
		TRANS_ELEMENT(IR_AND),	 /**< 論理積 */
		TRANS_ELEMENT(IR_OR),	  /**< 論理和 */
		TRANS_ELEMENT(IR_NOT),	 /**< 論理否定 */
		TRANS_ELEMENT(IR_XOR),	 /**< 排他的論理和 */
		TRANS_ELEMENT(IR_EQ_OP),       /**< == */
		TRANS_ELEMENT(IR_NE_OP),       /**< != */
		TRANS_ELEMENT(IR_SLT),	 /**< < */
		TRANS_ELEMENT(IR_SLET),	/**< <= */
		TRANS_ELEMENT(IR_LEFT_OP),     /**< << */
		TRANS_ELEMENT(IR_RIGHT_OP),    /**< >> */
		TRANS_ELEMENT(IR_RETURN),      //
		TRANS_ELEMENT(IR_IMM),	 //
		TRANS_ELEMENT(IR_MOV),	 //
		TRANS_ELEMENT(IR_KILL),	//
		TRANS_ELEMENT(IR_KILL_ARG),    /**< アーギュメントレジスタを解放する */
		TRANS_ELEMENT(IR_LOAD),	//
		TRANS_ELEMENT(IR_STORE),       //
		TRANS_ELEMENT(IR_LOADADDR),    //
		TRANS_ELEMENT(IR_BEQZ),	/**< lhs がゼロならブランチする */
		TRANS_ELEMENT(IR_JUMP),	/**< ジャンプする */
		TRANS_ELEMENT(IR_LABEL),       /**< ラベルを生成 */
		TRANS_ELEMENT(IR_FUNC_DEF),    /**< 関数定義 */
		TRANS_ELEMENT(IR_FUNC_CALL),   /**< 関数呼び出し */
		TRANS_ELEMENT(IR_FUNC_END),    /**< 関数定義終端 */
		TRANS_ELEMENT(IR_FUNC_ARG),    /**< 関数引数 */
		TRANS_ELEMENT(IR_FUNC_PARAM),  /**< 関数パラメータ */
		TRANS_ELEMENT(IR_NOP),
	};

	for (i = 0; i < irv->len; i++) {
		ir = irv->data[i];
		fprintf(file, ASM_COMMENTOUT_STR "%s(%d) %d %d %s\n", OP2STR[ir->op], ir->op, ir->lhs, ir->rhs,
			ir->name);

		if (ir->op == IR_FUNC_CALL && (using_regs = get_using_regs(ir->rhs)) != NULL) {
			fprintf(file, ASM_COMMENTOUT_STR "  regs: ");

			for (j = 0; j < using_regs->num; j++)
				fprintf(file, "%s ", get_temp_reg_str(using_regs->list[j]));

			fprintf(file, "\n");
		}
	}
}
