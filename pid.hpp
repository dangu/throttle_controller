#ifndef PID_H
#define PID_H

class PID
{
public:
  PID();
  void init();
  double calculate(double ref, double in);

private:
  double _P;
  double _I;
  double _D;

  double _P_gain;
  double _I_gain;
  double _D_gain;

  double _uMax;
  double _uMin;
};

  
#endif
