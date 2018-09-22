#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief main function
 */
int main(int argc, char **argv)
{
	struct vector_t *tokens;
	struct node_t *node = NULL;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [num]\n", argv[0]);
		return 1;
	}

	/* tokenize */
	tokens = tokenize(argv[1]);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[token]=====\n");
	show_token(tokens);
#endif

	node = parse(tokens);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[node]=====\n");
	show_node(node, 0);
#endif

	struct vector_t *irv = gen_ir(node);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[IR]=====\n");
	show_ir(irv);
#endif

	allocate_regs(irv);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[IR]=====\n");
	show_ir(irv);
#endif

	gen_riscv(irv);

	return 0;
}
