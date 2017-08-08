#include "Arduino.h"
#include "motor.hpp"

/**@brief Motor class constructor */
Motor::Motor(int pwm, int en1, int en2)
{
  _pwm = pwm;
  _en1 = en1;
  _en2 = en2;
}

/**@brief Initialize motor object */
void Motor::init()
{
  pinMode(_pwm, OUTPUT);
  pinMode(_en1, OUTPUT);
  pinMode(_en2, OUTPUT);
  digitalWrite(_en1, LOW);
  digitalWrite(_en2, LOW);
}

void Motor::forward(int speed)
{
  digitalWrite(_en1, HIGH);
  digitalWrite(_en2, LOW);
  analogWrite(_pwm, speed);
}

void Motor::reverse(int speed)
{
  digitalWrite(_en1, LOW);
  digitalWrite(_en2, HIGH);
  analogWrite(_pwm, speed); 
}

void Motor::speed(int speed)
{
  if(speed>0)
    {
      digitalWrite(_en1, HIGH);
      digitalWrite(_en2, LOW);
      analogWrite(_pwm, speed);
    }
  else
    {
      digitalWrite(_en1, LOW);
      digitalWrite(_en2, HIGH);
      analogWrite(_pwm, -speed); 
    }
      
      
}
/**@brief Stop motor by shorting the leads */
void Motor::stop()
{
  digitalWrite(_en1, LOW);
  digitalWrite(_en2, LOW);
  digitalWrite(_pwm, 1);
}

