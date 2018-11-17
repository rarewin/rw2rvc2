int factorial(int a)
{
	if (a <= 0) {
		return 1;
	} else {
		return (a * factorial(a - 1));
	}
}

int test_factorial_4() /* */ /*  24 */
{
	return factorial(4);
}

int test_factorial_5() /* */ /* 120 */
{
	return factorial(5);
}
