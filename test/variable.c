int a;
int b;
int c;
int d;
int e;
int f;

int test_var() /* */ /* 2 */
{
	/* int; */
	a = 2;
	return a;
}

int test_only_int() /* */ /* 0 */
{
	/* int; */
	/* int; */
	/* int   ; */

	return 0;
}

int test_declarate_var() /* */ /* 0 */
{
	return 0;
}

int test_declarate_var2() /* */ /* 1 */
{
	c += 1;

	return c;
}


int test_declarate_var3() /* */ /* 3 */
{
	d = 2 + 1;

	return d;
}

int test_declarate_va4() /* */ /* 1 */
{
	e = 1;
	f = 1;

	return f;
}
