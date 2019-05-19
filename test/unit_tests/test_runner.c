#include "unity.h"
#include "test_runner.h"

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_fake_thing);
    return UNITY_END();
}
