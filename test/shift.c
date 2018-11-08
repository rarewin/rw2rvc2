int test_lshift() /* */ /* 8 */
{
	return (1 << 3);
}

int test_rshift() /* */ /* 1 */
{
	return (8 >> 3);
}

int test_rshift2() /* */ /* 0 */
{
	return 1 >> 1;
}

int test_lshift2() /* */ /* 0 */
{
	return 0 << 4;
}

int test_lshift3() /* */ /* 0x400 */
{
	return (1 << 10);	/* 64-bit */ /* 64以上の演算は未定義なので注意する */
}

int test_priority_shift_and_or() /* */ /* 17 */
{
	return (1 | 1 << 4);
}
