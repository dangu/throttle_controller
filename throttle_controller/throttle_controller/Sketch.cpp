/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include "motor.hpp"
#include "pid.hpp"
#include <avr/wdt.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void wInterrupt();
//End of Auto generated function prototypes by Atmel Studio



#define MOTOR_PWM 9
#define MOTOR_A1  10
#define MOTOR_A2  11
#define MOTOR_POS (unsigned char)1
#define REF_IN    0
#define W_INTERRUPT 0

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);

volatile uint32_t wCounter;

PID pid_servo;

// The do_reboot is courtesy of this forum post:
// https://github.com/Optiboot/optiboot/issues/180
//
// It seems 1023 has to match BOOTSZ fuse, as it sets
// the size of the bootloader
typedef void (*do_reboot_t)(void);
const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND-1023)>>1);

// the setup routine runs once when you press reset:
void setup() {
//  wdt_disable();

  Serial.begin(115200);
  Serial.print("MCUSR WDTCSR: ");
  Serial.println(MCUSR, BIN);
  Serial.println(WDTCSR, BIN);
    cli();
    wdt_reset();
    /* Clear WDRF in MCUSR */
  MCUSR &= ~(1<<WDRF);
  /* Write logical one to WDCE and WDE */
  /* Keep old prescaler serring to prevent unintentional time-out */
  WDTCSR |=  (1<<WDCE) | (1<<WDE);
  
  /* Turn off WDT */
  WDTCSR = 0x00;
  sei();
  Serial.println("After:");
  Serial.println(MCUSR, BIN);
  Serial.println(WDTCSR, BIN);
  motor.init();
  pid_servo.init();
  attachInterrupt(W_INTERRUPT,wInterrupt, RISING);
    pinMode(LED_BUILTIN, OUTPUT);
   
}

// the loop routine runs over and over again forever:
void loop() {
  int pos;
  static int ref = 500;
  int refIn;  //!< Analog reference value input
  int u;
  int t;
  static unsigned int ct;
  static int tOld;
  static int refList[] = {100,200,300,200,100,800,100,800,700,690,680,670,660,650,645,640,635};
  int stepTime=500;
  uint32_t wCounterTemp;
  static uint32_t wCounterTempOld;
  int nEng;
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

  if((millis() % 1000)<100)
  {
  digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
  digitalWrite(LED_BUILTIN, LOW);
  }
  pos = analogRead(MOTOR_POS);
  refIn = analogRead(REF_IN);

  ref = refIn-200;
  ref = max(200,ref);
  ref = min(800,ref);
  if(pid_servo.calculate((double)ref, (double)pos))
  {
    u=-(int)pid_servo.getOutput();
    // Stop motor if the output is small enough
    if(abs(u)<40)
    {
      motor.stop();
    }
    else
    {
      motor.speed(u);
    }

    noInterrupts();
    wCounterTemp = wCounter;
    interrupts();

    nEng=wCounterTemp-wCounterTempOld;
    wCounterTempOld=wCounterTemp;

 //   Serial.println(nEng);
//    Serial.print(" ");
//    Serial.print(ref);
//    Serial.print(" ");
//    Serial.println(refIn);
  }
  
  if(Serial.available() > 0)
  {
    char chr;
    chr = Serial.read();
    Serial.print("Read ");
    Serial.println(chr);
    switch(chr)
    {
      case 'R':
    cli();
    Serial.println("A");

      MCUSR=0;
      do_reboot();
    Serial.println("B");
        // Reset
      //  wdt_enable(WDTO_15MS);
      //  while(1) {};
        break;

      default:
      Serial.println("Setting reference...");
        ref = (chr-'0')*110;
        Serial.println(ref, DEC);
        break;
    }
  }

 /* t=millis();
  if((t-tOld)>stepTime)
  {
    ref = refList[ct];

    ct++;
    if(ct>=sizeof(refList)/sizeof(int))
    {
      ct=0;
    }
    tOld = t;
  }*/
}

/** @brief Interrupt callback at every pulse from the alternator 

Approximately 2 pulses every 10 ms.
*/
void wInterrupt()
{
wCounter++;
}


