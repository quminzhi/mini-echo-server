#include <gtest/gtest.h>

extern "C" {
#include <stdlib.h>
}

class FooTest: public ::testing::Test {
protected:

  void SetUp() override {
  }

  void TearDown() override {}
};

TEST_F(FooTest, TestWordReference) {
  // do your test
}
