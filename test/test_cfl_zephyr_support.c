/* test_cfl_zephyr_support.c - Unit tests for cfl_zephyr_support */

/* All Rights Reserved */

/* Includes */

#include "cfl_zephyr_support/cfl_zephyr_support.h"
#include "unity.h"
#include <limits.h>
#include <string.h>

/* Test Setup and Teardown */

void setUp(void)
{
    /* This is run before EACH test */
}

void tearDown(void)
{
    /* This is run after EACH test */
}

/* Test Cases for cfl_zephyr_supportGetVersion */

void test_get_version_should_return_version_string(void)
{
    const char *version = cfl_zephyr_support_get_version();
    TEST_ASSERT_NOT_NULL(version);
    TEST_ASSERT_EQUAL_STRING("1.0.0", version);
}

/* Test Cases for cfl_zephyr_supportAdd */

void test_add_should_return_sum_when_adding_positive_numbers(void)
{
    int32_t result = cfl_zephyr_support_add(5, 3);
    TEST_ASSERT_EQUAL_INT32(8, result);
}

void test_add_should_return_sum_when_adding_negative_numbers(void)
{
    int32_t result = cfl_zephyr_support_add(-5, -3);
    TEST_ASSERT_EQUAL_INT32(-8, result);
}

void test_add_should_return_sum_when_adding_mixed_numbers(void)
{
    int32_t result = cfl_zephyr_support_add(10, -5);
    TEST_ASSERT_EQUAL_INT32(5, result);
}

void test_add_should_return_zero_when_adding_zeros(void)
{
    int32_t result = cfl_zephyr_support_add(0, 0);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

/* Test Cases for cfl_zephyr_supportMultiply */

void test_multiply_should_return_success_when_multiplying_positive_numbers(void)
{
    int32_t result;
    cfl_zephyr_support_status_t status = cfl_zephyr_support_multiply(5, 3, &result);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(15, result);
}

void test_multiply_should_return_success_when_multiplying_by_zero(void)
{
    int32_t result;
    cfl_zephyr_support_status_t status = cfl_zephyr_support_multiply(5, 0, &result);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_multiply_should_return_error_null_when_result_pointer_is_null(void)
{
    cfl_zephyr_support_status_t status = cfl_zephyr_support_multiply(5, 3, NULL);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_NULL, status);
}

/* Test Cases for cfl_zephyr_supportFoo */

void test_foo_should_return_success_when_processing_valid_input(void)
{
    char output[100];
    cfl_zephyr_support_status_t status = cfl_zephyr_support_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, status);
    TEST_ASSERT_EQUAL_STRING("Processed: test", output);
}

void test_foo_should_return_error_null_when_input_is_null(void)
{
    char output[100];
    cfl_zephyr_support_status_t status = cfl_zephyr_support_foo(NULL, output, sizeof(output));
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_NULL, status);
}

void test_foo_should_return_error_null_when_output_is_null(void)
{
    cfl_zephyr_support_status_t status = cfl_zephyr_support_foo("test", NULL, 100);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_NULL, status);
}

void test_foo_should_return_error_invalid_when_output_size_is_zero(void)
{
    char output[100];
    cfl_zephyr_support_status_t status = cfl_zephyr_support_foo("test", output, 0);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_INVALID, status);
}

void test_foo_should_return_error_invalid_when_buffer_too_small(void)
{
    char output[5];
    cfl_zephyr_support_status_t status = cfl_zephyr_support_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_INVALID, status);
}

/* Test Cases for cfl_zephyr_supportBar */

void test_bar_should_return_true_when_value_is_in_range(void)
{
    TEST_ASSERT_TRUE(cfl_zephyr_support_bar(50));
    TEST_ASSERT_TRUE(cfl_zephyr_support_bar(0));
    TEST_ASSERT_TRUE(cfl_zephyr_support_bar(100));
}

void test_bar_should_return_false_when_value_is_out_of_range(void)
{
    TEST_ASSERT_FALSE(cfl_zephyr_support_bar(-1));
    TEST_ASSERT_FALSE(cfl_zephyr_support_bar(101));
    TEST_ASSERT_FALSE(cfl_zephyr_support_bar(-100));
    TEST_ASSERT_FALSE(cfl_zephyr_support_bar(200));
}

/* Test Cases for cfl_zephyr_supportFactorial */

void test_factorial_should_return_correct_value_when_input_is_zero(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(0);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_one(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(1);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_five(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(5);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(120, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_ten(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(10);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(3628800, result.value);
}

void test_factorial_should_return_error_invalid_when_input_is_negative(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(-1);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

void test_factorial_should_return_error_invalid_when_input_is_too_large(void)
{
    cfl_zephyr_support_result_t result = cfl_zephyr_support_factorial(13);
    TEST_ASSERT_EQUAL(CFL_ZEPHYR_SUPPORT_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

/* Main Test Runner */

int main(void)
{
    UNITY_BEGIN();

    /* Version tests */
    RUN_TEST(test_get_version_should_return_version_string);

    /* Add function tests */
    RUN_TEST(test_add_should_return_sum_when_adding_positive_numbers);
    RUN_TEST(test_add_should_return_sum_when_adding_negative_numbers);
    RUN_TEST(test_add_should_return_sum_when_adding_mixed_numbers);
    RUN_TEST(test_add_should_return_zero_when_adding_zeros);
    RUN_TEST(test_add_should_saturate_on_overflow);
    RUN_TEST(test_add_should_saturate_on_underflow);

    /* Multiply function tests */
    RUN_TEST(test_multiply_should_return_success_when_multiplying_positive_numbers);
    RUN_TEST(test_multiply_should_return_success_when_multiplying_by_zero);
    RUN_TEST(test_multiply_should_return_error_null_when_result_pointer_is_null);
    RUN_TEST(test_multiply_should_return_error_invalid_on_overflow);
    RUN_TEST(test_multiply_should_return_error_invalid_on_underflow);

    /* Foo function tests */
    RUN_TEST(test_foo_should_return_success_when_processing_valid_input);
    RUN_TEST(test_foo_should_return_error_null_when_input_is_null);
    RUN_TEST(test_foo_should_return_error_null_when_output_is_null);
    RUN_TEST(test_foo_should_return_error_invalid_when_output_size_is_zero);
    RUN_TEST(test_foo_should_return_error_invalid_when_buffer_too_small);
    RUN_TEST(test_foo_should_succeed_when_buffer_exact_size);
    RUN_TEST(test_foo_should_return_error_invalid_when_buffer_one_byte_short);

    /* Bar function tests */
    RUN_TEST(test_bar_should_return_true_when_value_is_in_range);
    RUN_TEST(test_bar_should_return_false_when_value_is_out_of_range);

    /* Factorial function tests */
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_zero);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_one);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_five);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_ten);
    RUN_TEST(test_factorial_should_return_error_invalid_when_input_is_negative);
    RUN_TEST(test_factorial_should_return_error_invalid_when_input_is_too_large);

    return UNITY_END();
}
