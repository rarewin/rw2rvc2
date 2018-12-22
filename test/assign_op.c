int a;
int b;

int test_mul_assign() /* */ /* 10 */
{
	a = 5;
	a *= 2;
	return a;
}

int test_div_assign() /* */ /* 3 */
{
	a = 15;
	a /= 4;
	return a;
}

int test_mod_assign() /* */ /* 3 */
{
	a = 15;
	a %= 4;
	return a;
}

int test_add_assign() /* */ /* 19 */
{
	a = 15;
	a += 4;

	return a;
}

int test_sub_assign() /* */ /* 11 */
{
	a = 15;
	a -= 4;
	return a;
}

int test_multi_assign_op() /* */ /* 32 */
{
	a = 15;
	b = 2;
	a *= b;
	b -= a;
	a += (a + b);
	return a;
}

int test_left_assign_op() /* */ /* 8 */
{
	a = 1;
	a <<= 3;

	return a;
}

int test_left_assign_op2() /* */ /* 0x1000 */
{
	a = 0x10;
	a <<= 8;

	return a;
}


int test_right_assign_op() /* */ /* 8 */
{
	a = 64;
	a >>= 3;

	return a;
}

int test_right_assign_op2() /* */ /* 0x100 */
{
	a = 0x40000;
	a >>= 10;

	return a;
}
