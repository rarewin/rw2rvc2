int test_if_true() /* */ /* 1 */
{
	if (1) {
		return 1;
	}

	return 0;
}

int test_if_false() /* */ /* 0 */
{
	if (0) {
		return 1;
	}

	return 0;
}

int test_if_assign() /* */ /* 1 */
{
	int a;

	if (a = 2) {
		return 1;
	}

	return 0;
}


int test_if_else_true() /* */ /* 1 */
{
	int a;

	if (2) {
		a = 1;
	} else {
		a = 3;
	}

	return a;
}

int test_if_else_false() /* */ /* 3 */
{
	int a;

	if (0) {
		a = 1;
	} else {
		a = 3;
	}

	return a;
}
