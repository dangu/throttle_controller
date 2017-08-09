#include "Arduino.h"
#include "pid.hpp"

PID::PID()
{
}

void PID::init()
{
  _P = 0;
  _I = 0;
  _D = 0;

  _P_gain = 1;
  _I_gain = 0.1;
  _D_gain = 0;

  _uMax = 250;
  _uMin = -250;
}

double PID::calculate(double ref, double in)
{
  double e;
  double u;
  double uLimited;
  e = in - ref;

  _P = e * _P_gain;
  _I = _I + e * _I_gain;

  u = _P + _I;
  if (u > _uMax)
    {
      uLimited = _uMax;
    }
  else if (u < _uMin)
    {
      uLimited = _uMin;
    }
  else
    {
      uLimited = u;
    }

  _I = _I - (u - uLimited);

  Serial.print("P: ");
  Serial.print(_P);
  Serial.print(" I: ");
  Serial.print(_I);
  Serial.print(" u: ");
  Serial.print(u);
  Serial.print("\n");
  return u;
}
