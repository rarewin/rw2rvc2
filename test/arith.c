int test_add_1_1() /* */ /* 2 */
{
	return 1 + 1;
}

int test_add_100_1() /* */ /* 101 */
{
	return 100 + 1;
}

int test_sub_3_1() /* */ /* 2 */
{
	return 3 - 1;
}

int test_sub_255_1() /* */ /* 254 */
{
	return (255 - 1);
}

int test_add_255_1() /* */ /* 256 */
{
	return 255 + 1;
}

int test_64_a_64_a_64() /* */ /* 192 */
{
	return 64 + 64 + 64;
}

int test_64_s_64_a_64_s_64() /* */ /* 0 */
{
	return (64 - 64 + (64 - (64)));
}

int test_sum_1_10() /* */ /* 55 */
{
	return 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;
}

int test_sum_1_10_s_sum_1_10_a_0() /* */ /* 0 */
{
	return (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0);
}

int test_mul_1_2() /* */ /* 2 */
{
	return 1 * 2;
}

int test_cal_1_a_1_m_2() /* */ /* 3 */
{
	return 1 + 1 * 2;
}

int test_cal_4_m_3_a_1_m_2() /* */ /* 14 */
{
	return 4 * 3 + 1 * 2;
}

int test_cal_4_mod_3_a_1_mod_2() /* */ /* 2 */
{
	return 4 % 3 + 1 % 2;
}

int test_cal_4_d_2() /* */ /* 2 */
{
	return 4 / 2;
}

int test_cal_5_d_2() /* */ /* 2 */
{
	return 5 / 2;
}

int test_cal_4_m_3_d_4() /* */ /* 3 */
{
	return 4 * 3 / 4;
}

int test_cal_11_mod_4() /* */ /* 3 */
{
	return 11 % 4;
}

int test_cal_10_d_3_m_3() /* */ /* 9 */
{
	return 10 / 3 * 3;
}

int test_cal_10_mod_3_m_3() /* */ /* 3 */
{
	return 10 % 3 * 3;
}

int test_cal_10_d_3_m_3_with_extra() /* */ /* 9 */
{
	1 + 2;
	return 10 / 3 * 3;
}

int test_cal_10_d_3_m_3_with_many_extras() /* */ /* 9 */
{
	1 + 2;
	1 + 2;
	1 + 2;
	1 + 2;
	1 + 2;
	1 + 2;
	1 + 2;
	return 10 / 3 * 3;
}

int test_cal_sum_1_10_with_extra() /* */ /* 55 */
{
	1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0; 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0;
	return 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;
}
