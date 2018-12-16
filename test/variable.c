int test_var() /* */ /* 2 */
{
	int;
	int a;
	a = 2;
	return a;
}

int test_only_int() /* */ /* 0 */
{
	int;
	int;
	int   ;

	return 0;
}

int test_declarate_var() /* */ /* 0 */
{
	int b;

	return 0;
}

int test_declarate_var2() /* */ /* 3 */
{
	int c = 2;

	c += 1;

	return c;
}


int test_declarate_var3() /* */ /* 3 */
{
	int c = 2 + 1;

	return c;
}
