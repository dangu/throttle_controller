 //*End of auto generated code by Atmel studio */

#include "motor.hpp"
#include "pid.hpp"
#include "comm.hpp"
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
volatile uint32_t wMillisDiff;
volatile uint32_t wMicrosNow;
volatile uint32_t wMillisNow;
volatile uint16_t wMicrosDiffList[10];
volatile uint16_t wMicrosDiffList2[10];
volatile uint16_t *wMicrosDiffListPtr=wMicrosDiffList;
volatile uint16_t *wMicrosDiffListPtrToPrint;

PID pid;
PID pid_n_eng;

status_t	status;



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
  pid_n_eng.init();
  pid_n_eng.setPGain(0.1);

  attachInterrupt(W_INTERRUPT,wInterrupt, RISING);
    pinMode(LED_BUILTIN, OUTPUT);
   
}

// the loop routine runs over and over again forever:
void loop() {
  int pos;
  static int ref = 500;
  static int refSerial = 800; //!< rpm value from serial input
  int refIn;  //!< Analog reference value input
  int u;
  int t;
  static unsigned int ct;
  static int tOld;
  static int refList[] = {100,200,300,200,100,800,100,800,700,690,680,670,660,650,645,640,635};
  int stepTime=500;
  uint32_t wMicrosDiffTemp;
  uint32_t wMillisDiffTemp;
  uint32_t wMicrosNowTemp;
  uint32_t wMillisNowTemp;
  uint32_t wCounterTemp;
  static uint32_t wCounterTempOld;
  static uint32_t millisOld;
  float nEng;
  float refTemp;
  static float u_pid_n_eng=50;
  static bool forceMotorStopped;

  static uint8_t rxBuf[20];
  static uint8_t rxBufIn;
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

  // Calculate output of engine speed PID
  //
  // Min servo=980
  // Max servo=288
  if(pid_n_eng.calculate((double)refSerial, (double)nEng))
  {
	  u_pid_n_eng = pid_n_eng.getOutput();

  }

  ref = 980-ref*6.92f;

  pos = analogRead(MOTOR_POS);
  refIn = analogRead(REF_IN);

  // Min 518
  // Max 907 875
  // Max gas pedal 1023
  refTemp = (refIn-518);
  //ref = 980-refTemp*1.37f;
  ref=980-100*6.92f;
  ref = max(288,ref);
  ref = min(980,ref);
  if(pid.calculate((double)ref, (double)pos))
  {
    u=(int)pid.getOutput();
    // Stop motor if the output is small enough
    // or if user pressed '0'
    if((abs(u)<40) || forceMotorStopped)
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
/*    wMicrosDiffTemp = wMicrosDiff;
    wMillisDiffTemp = wMillisDiff;
    wMillisNowTemp = wMillisNow;
    wMicrosNowTemp = wMicrosNow;
    wCounterTemp = wCounter;*/

    if(wMicrosDiff == 0)
    {
    	nEng=0;
    }
    else
    {
    	nEng=4*1000000/wMicrosDiff;
    }

    // Double buffered list
    if(wMicrosDiffListPtr == wMicrosDiffList)
    {
    	wMicrosDiffListPtr = wMicrosDiffList2;
    	wMicrosDiffListPtrToPrint = wMicrosDiffList;
    }
    else
    {
    	wMicrosDiffListPtr = wMicrosDiffList;
    	wMicrosDiffListPtrToPrint = wMicrosDiffList2;
    }
    interrupts();

    if((millis()-millisOld)> 100)
    {

        wCounterTempOld=wCounterTemp;

        /*
         * Pulses:
         * rpm	pulses per second
         * 500	132
         * 1000	720
         * 1500	1133
         * 1700 1287
         *
         */
    	digitalWrite(LED_BUILTIN, HIGH);
/*    	Serial.print(nEng);
    	Serial.print(" ");
    	Serial.print(wMicrosDiffTemp);
    	Serial.print(" ");
    	Serial.print(wMillisDiffTemp);
    	Serial.print(" ");
    	Serial.print(wMicrosNowTemp);
    	Serial.print(" ");
    	Serial.print(wMillisNowTemp);
    	Serial.print(" ");
    	Serial.print(refIn);
    	Serial.print(" ");
    	Serial.print(ref);
    	Serial.print(" ");
    	Serial.println(pos);
    	for(int i=0;i<10;i++)
    	{
    		Serial.print(wMicrosDiffListPtrToPrint[i]);
    		Serial.print(" ");
    	}
    	*/
/*    	Serial.print(nEng);
    	Serial.print(" ");
    	Serial.print(u_pid_n_eng);
    	Serial.print(" ");
    	Serial.print(ref);
    	Serial.print(" ");
    	Serial.print(refSerial);
    	Serial.println("");*/
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

    switch(chr)
    {
    case '0':
    	// Turn off servo
    	Serial.println("Stopping motor...");
    	motor.stop();
		forceMotorStopped = true;
		break;

    default:
    	if(rxBufIn>=(sizeof(rxBuf)-1))
    	{
    		Serial.println("Rx buffer overflow!");
    		rxBufIn = 0;
    	}

    	rxBuf[rxBufIn++] = chr;

    	if(chr == '\n')
    	{
    		rxBuf[rxBufIn] = '\0';	// End the string
    		// Handle command
    		handleCommand(rxBuf);
    		rxBufIn = 0;
    	}

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
	static uint32_t millisOld;
	static uint8_t listPos;

	wMicrosNow = micros();  // Timestamp now
	wMillisNow = millis();
	wMicrosDiff = wMicrosNow-microsOld;
	wMillisDiff = wMillisNow-millisOld;
	wMicrosDiffListPtr[listPos] = wMicrosDiff;
	if(++listPos>9)
		listPos=0;
	wCounter++;

	microsOld = wMicrosNow;
	millisOld = wMillisNow;
}


