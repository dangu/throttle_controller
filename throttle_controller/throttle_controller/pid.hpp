#ifndef PID_H
#define PID_H

class PID
{
public:
  PID();
  void init();
  void calculate(double ref, double in);
  double getOutput();
  void debugPrint(double, double);
  void setPGain(double);
  void setIGain(double);
  void setDGain(double);
  void setUMax(double);
  void setUMin(double);

  double getPGain();
  double getIGain();
  double getDGain();
  double getUMax();
  double getUMin();


private:
  double _u;
  double _uLimited;
  double _P;
  double _I;
  double _D;
  double _eOld;   //!< Old error, for D part

  double _P_gain;
  double _I_gain;
  double _D_gain;

  double _uMax;
  double _uMin;

  int _millisOld;
  int _sampleTime;
};

  
#endif
