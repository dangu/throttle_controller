 //*End of auto generated code by Atmel studio */

#include "motor.hpp"
#include "pid.hpp"
#include <avr/wdt.h>
//Beginning of Auto generated function prototypes by Atmel Studio
void wInterrupt();
//End of Auto generated function prototypes by Atmel Studio

/**
 * Motor calibration			Connected
 * Idle position: 			980 980
 * Full throttle position:	50	288
 */


#define MOTOR_PWM 9
#define MOTOR_A1  10
#define MOTOR_A2  11
#define MOTOR_POS (unsigned char)1
#define REF_IN    0
#define W_INTERRUPT 0

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);

volatile uint32_t wCounter;
volatile uint32_t wMicrosDiff;

PID pid;

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

  Serial.begin(19200);
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
  pid.init();
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
  uint32_t wMicrosDiffTemp;
  static uint32_t millisOld;
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


  pos = analogRead(MOTOR_POS);
  refIn = analogRead(REF_IN);

  //ref = refIn-200;
  //ref = max(200,ref);
  //ref = min(800,ref);
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

    // Critical region:
    // The interrupts need to be turned off before reading the
    // variables used inside the interrupt routine
    noInterrupts();
    wMicrosDiffTemp = wMicrosDiff;
    interrupts();

    if((millis()-millisOld)> 1000)
    {
    	digitalWrite(LED_BUILTIN, HIGH);
    	Serial.print(wMicrosDiffTemp);
    	Serial.print(" ");
    	Serial.print(ref);
    	Serial.print(" ");
    	Serial.print(pos);
    	Serial.print(" ");
    	Serial.println(millisOld);
    	millisOld = millis();
    }
    else
    {
    	digitalWrite(LED_BUILTIN, LOW);
    }

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
    	motor.stop();  // Prevent motor runaway during reset
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
	static uint32_t microsOld;
	uint32_t microsNow;
	microsNow = micros();  // Timestamp now
	wMicrosDiff = microsNow-microsOld;
	wCounter++;

	microsOld = microsNow;
}


