#include <stdio.h>

#include "rw2rvc2.h"

/**
 * @brief main function
 */
int main(int argc, char **argv)
{
	struct vector_t *tokens;
	struct node_t *node = NULL;
	struct dict_t *d = NULL;
	FILE *fp;
	char *buf;
	size_t s;

	if (argc != 2) {
		fprintf(stderr, "usage: %s [source file]  or  %s [code]\n", argv[0], argv[0]);
		return 1;
	}

	if ((fp = fopen(argv[1], "rt")) == NULL) {
		buf = argv[1];
	} else {
		/* read file to buffer */
		fseek(fp, 0, SEEK_END);
		s = ftell(fp); /* ファイルサイズを取得 */
		fseek(fp, 0, SEEK_SET);

		if ((buf = malloc(s + 1)) == NULL) {
			fprintf(stderr, "memory allocation error\n");
			return 3;
		}

		fread(buf, 1, s, fp);
		buf[s] = 0;
	}

	/* tokenize */
	tokens = tokenize(buf);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[token]=====\n");
	show_token(tokens);
#endif

	node = parse(tokens);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[node]=====\n");
	show_node(node, 0);
#endif

	d = new_dict();
	struct vector_t *irv = gen_ir(node, d);

#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[IR]=====\n");
	show_ir(irv);
#endif

	allocate_regs(irv);
#if defined(DEBUG)
	color_printf(COL_YELLOW, "=====[IR]=====\n");
	show_ir(irv);
#endif
	gen_riscv(irv, d);

	fflush(stdout);

	return 0;
}
