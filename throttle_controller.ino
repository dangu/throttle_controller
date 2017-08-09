#include "motor.hpp"
#include "pid.hpp"

#define MOTOR_PWM 9
#define MOTOR_A1  10
#define MOTOR_A2  11
#define MOTOR_POS (unsigned char)0

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);

PID pid;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  motor.init();
  pid.init();
}

// the loop routine runs over and over again forever:
void loop() {
  int pos;
  static int ref = 500;
  int u;
  int t;
  static unsigned int ct;
  static int tOld;
  static int refList[] = {100,200,300,200,100,800,100,800,700,690,680,670,660,650,645,640,635};
  int stepTime=500;
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
  if(pid.calculate((double)ref, (double)pos))
  {
    u=-(int)pid.getOutput();
    // Stop motor if the output is small enough
    if(abs(u)<40)
    {
      motor.stop();
    }
    else
    {
      motor.speed(u);
    }
  }
  
  if(Serial.available() > 0)
  {
    char chr;
    chr = Serial.read();
    Serial.print("Read ");
    Serial.println(chr);
    ref = (chr-'0')*110;
    Serial.println(ref, DEC);
  }

  t=millis();
  if((t-tOld)>stepTime)
  {
    ref = refList[ct];

    ct++;
    if(ct>=sizeof(refList)/sizeof(int))
    {
      ct=0;
    }
    tOld = t;
  }
}
