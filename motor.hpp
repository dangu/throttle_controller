#ifndef MOTOR_H
#define MOTOR_H

class Motor
{
public:
  Motor(int pwm, int en1, int en2);
  void init();
  void forward(int spd);
  void reverse(int spd);
  void speed(int spd);
  void stop();
  
private:
  int _pwm;
  int _en1;
  int _en2;
};
#endif
