#ifndef MOTOR_H
#define MOTOR_H

class Motor
{
public:
  Motor(int pwm, int en1, int en2);
  void init();
  void forward(int speed);
  void reverse(int speed);
  void stop();
  
private:
  int _pwm;
  int _en1;
  int _en2;
};
#endif
