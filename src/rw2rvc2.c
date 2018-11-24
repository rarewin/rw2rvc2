#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "rw2rvc2.h"

/**
 * @brief 使用方法を表示する
 * @param[in] prog  プログラム名文字列
 */
static void usage(char *prog)
{
	fprintf(stderr, "usage: %s [source file]  or  %s [code]\n\n", prog, prog);
	fprintf(stderr,
		"  Options:\n"
		"    -z  output debug info as comment\n");
}

/**
 * @brief main function
 */
int main(int argc, char **argv)
{
	struct vector_t *tokens;
	struct node_t *node = NULL;
	struct dict_t *d = NULL;

	FILE *fp;
	FILE *dbgout = stdout;
	char *buf;
	size_t s;

	int opt;
	bool flag_debug = false;

	setvbuf(dbgout, NULL, _IONBF, 0);

	/* オプションをパース */
	while ((opt = getopt(argc, argv, "z")) != -1) {
		switch (opt) {
		case 'z':
			flag_debug = true;
			break;
		case 'h':
		default:
			usage(argv[0]);
			exit(0);
			/* NOTREACHED */
			break;
		}
	}

	if (optind >= argc) {
		usage(argv[0]);
		exit(0);
		/* NOTREACHED */
	}

	if ((fp = fopen(argv[optind], "rt")) == NULL) {
		buf = argv[optind];
	} else {
		/* read file to buffer */
		fseek(fp, 0, SEEK_END);
		s = ftell(fp); /* ファイルサイズを取得 */
		fseek(fp, 0, SEEK_SET);

		if ((buf = malloc(s + 1)) == NULL) {
			fprintf(dbgout, "memory allocation error\n");
			return 3;
		}

		fread(buf, 1, s, fp);
		buf[s] = 0;
	}

	/* tokenize */
	tokens = tokenize(buf);

	if (flag_debug) {
		fprintf(dbgout, ASM_COMMENTOUT_STR);
		color_printf(dbgout, COL_YELLOW, "=====[token]=====\n");
		show_token(dbgout, tokens);
	}

	node = parse(tokens);

	if (flag_debug) {
		fprintf(dbgout, ASM_COMMENTOUT_STR);
		color_printf(dbgout, COL_YELLOW, "=====[node]=====\n");
		show_node(dbgout, node, 0);
	}

	d = new_dict();
	struct vector_t *irv = gen_ir(node, d);

	if (flag_debug) {
		fprintf(dbgout, ASM_COMMENTOUT_STR);
		color_printf(dbgout, COL_YELLOW, "=====[IR]=====\n");
		show_ir(dbgout, irv);
	}

	allocate_regs(irv);

	if (flag_debug) {
		fprintf(dbgout, ASM_COMMENTOUT_STR);
		color_printf(dbgout, COL_YELLOW, "=====[IR]=====\n");
		show_ir(dbgout, irv);
	}

	gen_riscv(irv, d);

	fflush(stdout);

	return 0;
}
