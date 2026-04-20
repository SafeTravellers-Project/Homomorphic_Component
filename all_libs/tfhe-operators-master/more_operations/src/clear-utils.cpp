#include "clear-utils.h"

double sigmoid(double x)
{
     double result;
     result = 1 / (1 + exp(-x));
     return result;
}
 
double oddSigmoid(double x)
{
	double result;
	result = sigmoid(x) - 0.5;
	return result;
}

double absolute(double x)
{
  if (x < 0)
    return -x;
  else
    return x;
}

int min(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}
