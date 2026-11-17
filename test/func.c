int func_return_10()
{
	return 10;
}

int test_func_call_return_10()  /* */ /* 10 */
{
	return func_return_10();
}

int test_func_call_return_10_twice() /* */ /* 100 */
{
	return func_return_10() * func_return_10();
}

int test_func_call_return_10_three_times() /* */ /* 110 */
{
	return func_return_10() + func_return_10() * func_return_10();

}

int func_return_8()
{
	return 8;
}

int test_two_func_call() /* */ /* 180 */
{
	return func_return_8() * func_return_10() + func_return_10() * func_return_10();
}

int func_return_3_plus_func_call()
{
	return func_return_8() + 3;
}

int func_return_3()
{
	return 3;
}

int test_many_func_calls() /* */ /* 57 */
{
	return func_return_8() * func_return_3() + func_return_3() * func_return_3_plus_func_call();
}

int func_echo(int a)
{
	return a;
}

int test_func_arg() /* */ /* 17 */
{
	return func_echo(17);
}

int func_zero_args()
{
	return 100;
}

int test_func_zero_args() /* */ /* 100 */
{
	return func_zero_args();
}

int add_two(int x, int y)
{
	return x + y;
}

int test_func_two_args() /* */ /* 7 */
{
	return add_two(3, 4);
}

int add_three(int x, int y, int z)
{
	return x + y + z;
}

int test_func_three_args() /* */ /* 15 */
{
	return add_three(2, 5, 8);
}
