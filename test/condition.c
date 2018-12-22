int a;

int test_equal() /* */ /* 0 */
{
	a = 15;
	if (a == 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_not_equal() /* */ /* 1 */
{
	a = 15;
	if (a != 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_greater_equal() /* */ /* 0 */
{
	a = 15;
	if (a >= 15) {
		return 0;
	} else {
		return 1;}
}

int test_less_equal() /* */ /* 0 */
{
	a = 15;
	if (a <= 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_less() /* */ /* 1 */
{
	a = 15;
	if (a < 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_less2() /* */ /* 0 */
{
	a = 15;

	if (a < 16) {
		return 0;
	} else {
		return 1;
	}
}

int test_greater() /* */ /* 0 */
{
	a = 15;

	if (a > 2) {
		return 0;
	} else {
		return 1;
	}
}

int test_greater2() /* */ /* 1 */
{
	a = 15;

	if (a > 20) {
		return 0;
	} else {
		return 1;
	}
}

int test_greater_equal2() /* */ /* 1 */
{
	a = 15;

	if (a >= 20) {
		return 0;
	} else {
		return 1;
	}
}

int test_greater_equal3() /* */ /* 0 */
{
	a = 15;

	if (a >= 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_less_equal2() /* */ /* 0 */
{
	a = 5;

	if (a <= 15) {
		return 0;
	} else {
		return 1;
	}
}

int test_less_equal3() /* */ /* 1 */
{
	a = 35;

	if (a <= 15) {
		return 0;
	} else {
		return 1;
	}
}
