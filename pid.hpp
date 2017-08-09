#ifndef PID_H
#define PID_H

class PID
{
public:
  PID();
  void init();
  bool calculate(double ref, double in);
  double getOutput();

private:
  double _uLimited;
  double _P;
  double _I;
  double _D;

  double _P_gain;
  double _I_gain;
  double _D_gain;

  double _uMax;
  double _uMin;

  int _millisOld;
  int _sampleTime;
};

  
#endif
