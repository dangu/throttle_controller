/*
 Fade

 This example shows how to fade an LED on pin 9
 using the analogWrite() function.

 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

 This example code is in the public domain.
 */

#define MOTOR_A1  7
#define MOTOR_A2  8


int led = 9;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

int direction = 0;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  digitalWrite(MOTOR_A1, HIGH);
  digitalWrite(MOTOR_A2, LOW);
}

// the loop routine runs over and over again forever:
void loop() {

  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
    if(direction==1)
    {
      direction=0;
    }
    else
    {
      direction=1;
    }
  }

  if(direction==1)
  {
      digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, LOW);
    Serial.println("FW");
  }
  else
  {
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, HIGH);
    Serial.println("REV");
  }


  Serial.print("Analog: ");
  Serial.println(brightness);
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
