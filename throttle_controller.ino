#include "motor.hpp"
#include "pid.hpp"

#define MOTOR_PWM 9
#define MOTOR_A1  7
#define MOTOR_A2  8
#define MOTOR_POS 0

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);

PID pid;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  motor.init();
  pid.init();
}

// the loop routine runs over and over again forever:
void loop() {
  int pos;
  int ref = 500;
  int u;
  /*
  int pause = 300;
  motor.forward(speed);
  delay(pause);
  motor.forward(0);
  delay(pause);
  motor.reverse(speed);
  delay(pause);
  motor.stop();
  delay(pause);*/
  pos = analogRead(MOTOR_POS);
  u = -(int)pid.calculate((double)ref, (double)pos);
 
  motor.speed(u);
}
