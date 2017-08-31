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

void Motor::forward(int spd)
{
  speed(spd);
}

void Motor::reverse(int spd)
{
  speed(-spd);
}

void Motor::speed(int spd)
{
  digitalWrite(_pwm,1);
  if(spd>0)
    {
      analogWrite(_en1, spd);
      digitalWrite(_en2, LOW);
    }
  else
    {
      digitalWrite(_en1, LOW);
      analogWrite(_en2, -spd);
    }
      
      
}
/**@brief Stop motor by shorting the leads */
void Motor::stop()
{
  digitalWrite(_en1, LOW);
  digitalWrite(_en2, LOW);
  digitalWrite(_pwm, 1);
}

