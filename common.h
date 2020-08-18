#ifndef COMMON_H
#define COMMON_H

double roundPrec(double val, int prec) {
  double power = pow(10, prec);
  double rVal = round(val * power) / power;
  return rVal;
}

#endif
