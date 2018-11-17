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
