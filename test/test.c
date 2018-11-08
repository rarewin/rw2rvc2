#include <stdio.h>
#include <stdbool.h>

static inline void assert(bool exp, char *func_name)
{
	printf("%s => ", func_name);
	if (exp) {
		printf("\e[1;32mOK\e[m\n");
	} else {
		printf("\e[1;31mNG\e[m\n");
	}
}

int test_if_true();
int test_if_false();
int test_if_assign();
int test_if_else_true();
int test_if_else_false();
int test_lshift();
int test_rshift();
int test_rshift2();
int test_lshift2();
int test_lshift3();
int test_priority_shift_and_or();
int test_mul_assign();
int test_div_assign();
int test_mod_assign();
int test_add_assign();
int test_sub_assign();
int test_multi_assign_op();
int test_var();
int test_factorial_4();
int test_factorial_5();
int test_return0();
int test_return100();
int test_return10000();
int test_return_arg(int a);
int test_add_1_1();
int test_add_100_1();
int test_sub_3_1();
int test_sub_255_1();
int test_add_255_1();
int test_64_a_64_a_64();
int test_64_s_64_a_64_s_64();
int test_sum_1_10();
int test_sum_1_10_s_sum_1_10_a_0();
int test_mul_1_2();
int test_cal_1_a_1_m_2();
int test_cal_4_m_3_a_1_m_2();
int test_cal_4_mod_3_a_1_mod_2();
int test_cal_4_d_2();
int test_cal_5_d_2();
int test_cal_4_m_3_d_4();
int test_cal_11_mod_4();
int test_cal_10_d_3_m_3();
int test_cal_10_mod_3_m_3();
int test_cal_10_d_3_m_3_with_extra();
int test_cal_10_d_3_m_3_with_many_extras();
int test_cal_sum_1_10_with_extra();
int test_func_call_return_10();
int test_func_call_return_10_twice();
int test_func_call_return_10_three_times();
int test_two_func_call();
int test_many_func_calls();
int test_func_arg();

int main(void)
{
	assert(test_if_true() == 1, "test_if_true");
	assert(test_if_false() == 0, "test_if_false");
	assert(test_if_assign() == 1, "test_if_assign");
	assert(test_if_else_true() == 1, "test_if_else_true");
	assert(test_if_else_false() == 3, "test_if_else_false");
	assert(test_lshift() == 8, "test_lshift");
	assert(test_rshift() == 1, "test_rshift");
	assert(test_rshift2() == 0, "test_rshift2");
	assert(test_lshift2() == 0, "test_lshift2");
	assert(test_lshift3() == 0x400, "test_lshift3");
	assert(test_priority_shift_and_or() == 17, "test_priority_shift_and_or");
	assert(test_mul_assign() == 10, "test_mul_assign");
	assert(test_div_assign() == 3, "test_div_assign");
	assert(test_mod_assign() == 3, "test_mod_assign");
	assert(test_add_assign() == 19, "test_add_assign");
	assert(test_sub_assign() == 11, "test_sub_assign");
	assert(test_multi_assign_op() == 32, "test_multi_assign_op");
	assert(test_var() == 2, "test_var");
	assert(test_factorial_4() == 24, "test_factorial_4");
	assert(test_factorial_5() == 120, "test_factorial_5");
	assert(test_return0() == 0, "test_return0");
	assert(test_return100() == 100, "test_return100");
	assert(test_return10000() == 10000, "test_return10000");
	assert(test_return_arg(0) == 0, "test_return_arg");
	assert(test_add_1_1() == 2, "test_add_1_1");
	assert(test_add_100_1() == 101, "test_add_100_1");
	assert(test_sub_3_1() == 2, "test_sub_3_1");
	assert(test_sub_255_1() == 254, "test_sub_255_1");
	assert(test_add_255_1() == 256, "test_add_255_1");
	assert(test_64_a_64_a_64() == 192, "test_64_a_64_a_64");
	assert(test_64_s_64_a_64_s_64() == 0, "test_64_s_64_a_64_s_64");
	assert(test_sum_1_10() == 55, "test_sum_1_10");
	assert(test_sum_1_10_s_sum_1_10_a_0() == 0, "test_sum_1_10_s_sum_1_10_a_0");
	assert(test_mul_1_2() == 2, "test_mul_1_2");
	assert(test_cal_1_a_1_m_2() == 3, "test_cal_1_a_1_m_2");
	assert(test_cal_4_m_3_a_1_m_2() == 14, "test_cal_4_m_3_a_1_m_2");
	assert(test_cal_4_mod_3_a_1_mod_2() == 2, "test_cal_4_mod_3_a_1_mod_2");
	assert(test_cal_4_d_2() == 2, "test_cal_4_d_2");
	assert(test_cal_5_d_2() == 2, "test_cal_5_d_2");
	assert(test_cal_4_m_3_d_4() == 3, "test_cal_4_m_3_d_4");
	assert(test_cal_11_mod_4() == 3, "test_cal_11_mod_4");
	assert(test_cal_10_d_3_m_3() == 9, "test_cal_10_d_3_m_3");
	assert(test_cal_10_mod_3_m_3() == 3, "test_cal_10_mod_3_m_3");
	assert(test_cal_10_d_3_m_3_with_extra() == 9, "test_cal_10_d_3_m_3_with_extra");
	assert(test_cal_10_d_3_m_3_with_many_extras() == 9, "test_cal_10_d_3_m_3_with_many_extras");
	assert(test_cal_sum_1_10_with_extra() == 55, "test_cal_sum_1_10_with_extra");
	assert(test_func_call_return_10() == 10, "test_func_call_return_10");
	assert(test_func_call_return_10_twice() == 100, "test_func_call_return_10_twice");
	assert(test_func_call_return_10_three_times() == 110, "test_func_call_return_10_three_times");
	assert(test_two_func_call() == 180, "test_two_func_call");
	assert(test_many_func_calls() == 57, "test_many_func_calls");
	assert(test_func_arg() == 17, "test_func_arg");

}

