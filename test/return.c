int test_return0() /* */ /* 0 */
{
	return 0;
}

int test_return100() /* */ /* 100 */
{
	return 100;
}

int test_return10000() /*  */ /* 10000 */
{
	return 10000;
}

int test_return_arg(int a) /* 0 */ /* 0 */
{
	return a;
}

int test_return_arg2(int a) /* 10 */ /* 10 */
{
	return +a;
}

int test_return_minus_value() /* */ /* -1  */
{
	return -1;
}

int test_return_minus_arg(int a) /* 1 */ /* -1 */
{
	return -a;
}

int test_return_minus_arg2(int a) /* -10 */ /* 10 */
{
	return -a;
}

int test_return_minus_arg3(int a) /* -10 */ /* -10 */
{
	return -(-a);
}

int test_return_hex_value() /* */ /* 0x10 */
{
	return 0x10;
}

int test_return_oct_value() /* */ /* 020 */
{
	return 020;
}

int test_return_sum_dec_hex_oct_value() /* */ /* 10 + 0x10 + 010 */
{
	return 10 + 0x10 + 010;
}
