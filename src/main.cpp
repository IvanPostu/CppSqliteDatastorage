#include <cstdio>
#include <cstdlib>

#include "calculator/calculator.hpp"

int main(int argc, char **argv){

  Calculator *c = new Calculator;
  int a = 22;
  int b = 33;

  printf("%d + %d = %d \n", a, b, c->summation(a, b));
  printf("%d - %d = %d \n", a, b, c->subtraction(a, b));

  return EXIT_SUCCESS;
}
