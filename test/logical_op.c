int test_or_1() /* */ /* 0 */
{
	if (1 || 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_or_2() /* */ /* 1 */
{
	if (0 || 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_1() /* */ /* 1 */
{
	if (1 && 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_2() /* */ /* 1 */
{
	if (0 && 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_3() /* */ /* 0 */
{
	if (1 && 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_or_1() /* */ /* 0 */
{
	if (1 && 1 || 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_or_2() /* */ /* 0 */
{
	if (0 || 1 && 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_and_or_3() /* */ /* 0 */
{
	if (1 && 0 || 1 && 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_logical_or_1() /* */ /* 0 */
{
	if (0 | 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_logical_or_2() /* */ /* 1 */
{
	if (0 | 0) {
		return 0;
	} else {
		return 1;
	}
}

int test_xor_1() /* */ /* 1 */
{
	if (1 ^ 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_xor_2() /* */ /* 0 */
{
	if (0 ^ 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_logical_and_1() /* */ /* 1 */
{
	if (0 & 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_logical_and_2() /* */ /* 1 */
{
	if (2 & 1) {
		return 0;
	} else {
		return 1;
	}
}

int test_logical_and_3() /* */ /* 0 */
{
	if (17 & 16) {
		return 0;
	} else {
		return 1;
	}
}
