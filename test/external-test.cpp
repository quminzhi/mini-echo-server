#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include "stdlib.h"
}

TEST(CSAPPTEST, INVOCATIONTEST) {
  EXPECT_EQ(2, 2);
}
