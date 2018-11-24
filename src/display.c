#include <stdio.h>
#include <stdarg.h>

#include "rw2rvc2.h"

/**
 * @brief 文字を色付きで標準出力する
 */
int color_printf(FILE *file, dprint_color_t color, const char *format, ...)
{
	va_list ap;
	int ret = 0;

	/* FIXME: アセンブラファイルに色を出力するととエラーになるので一旦無効に. */
	(void)color;

	va_start(ap, format);
	// ret += fprintf(file, "\e[1;%dm", color);
	ret += vfprintf(file, format, ap);
	// ret += fprintf(file, "\e[m");
	va_end(ap);

	return ret;
}

/**
 * @brief エラー表示をエラー出力に吐き出す
 */
int error_printf(const char *format, ...)
{
	va_list ap;
	int ret = 0;

	va_start(ap, format);
	ret += fprintf(stderr, "\e[1;%dm", COL_RED);
	ret += vfprintf(stderr, format, ap);
	ret += fprintf(stderr, "\e[m");
	va_end(ap);

	return ret;
}
