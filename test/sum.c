int sum(int a)
{
	if (a <= 0) {
		return 0;
	} else {
		return a + sum(a - 1);
	}
}

int test_sum_10() /* */ /* 55 */
{
	return sum(10);
}

int test_sum_1000() /* */ /* 500500 */
{
	return sum(1000);
}
