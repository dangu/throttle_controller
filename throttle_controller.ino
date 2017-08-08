#include "motor.hpp"

#define MOTOR_PWM 9
#define MOTOR_A1  7
#define MOTOR_A2  8

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  motor.init();
}

// the loop routine runs over and over again forever:
void loop() {
  int speed = 100;
  int pause = 300;
  motor.forward(speed);
  delay(pause);
  motor.forward(0);
  delay(pause);
  motor.reverse(speed);
  delay(pause);
  motor.stop();
  delay(pause);
}
