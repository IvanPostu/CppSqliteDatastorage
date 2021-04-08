#include "calculator/calculator.hpp"
#include "gtest/gtest.h"

TEST(CalculatorClassTestCase, test1)
{
  Calculator *c = new Calculator();

  EXPECT_EQ(c->summation(2, 19), 21);
  EXPECT_EQ(c->subtraction(100, 77), 23);

  delete c;
}
