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

  _P_gain = 0;
  _I_gain = 0.1;
  _D_gain = 0;

  _uMax = 250;
  _uMin = -250;
  _millisOld = millis();
  _sampleTime = 100;
}

bool PID::calculate(double ref, double in)
{
  double e;
  double u;

  int millisNow;
  int T;           //!< [ms] Actual sample time

  millisNow = millis();
  T = millisNow - _millisOld;
  // Tsample is the desired sample time
  if(T >= _sampleTime)
    {
      e = in - ref;

      _P = e * _P_gain;
      _I = _I + e * _I_gain;

      u = _P + _I;
      if (u > _uMax)
	{
	  _uLimited = _uMax;
	}
      else if (u < _uMin)
	{
	  _uLimited = _uMin;
	}
      else
	{
	  _uLimited = u;
	}


      _I = _I - (u - _uLimited);
      
      Serial.print("P: ");
      Serial.print(_P);
      Serial.print(" I: ");
      Serial.print(_I);
      Serial.print(" u: ");
      Serial.print(u);
      Serial.print("\n");

      _millisOld = millisNow;
      return true; 
    }
  else
    {
      return false;
    }
}

/** @brief Returns the output value */
double PID::getOutput()
{
  return _uLimited;
}
