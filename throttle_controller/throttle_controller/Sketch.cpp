/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

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

#define N_ENG_MIN 400
#define N_ENG_MAX 2400

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

PID pid_servo;
PID pid_n_eng;

status_t		status;
conversions_t	conversions;

/** @brief Get one sample of the engine speed */
void getNEngSample()
{
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
    status.nEng_f=0;
  }
  else
  {
    status.nEng_f=4*1000000/wMicrosDiff;
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
}

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
  pid_servo.init();
  pid_servo.setPGain(15.0);
  pid_n_eng.init();
  pid_n_eng.setPGain(0.1);
  pid_n_eng.setIGain(0.1);
  pid_n_eng.setUMin(0.0);
  pid_n_eng.setUMax(100.0);

  // Setup conversion parameters
  conversions.servoK 		= -0.1667;
  conversions.servoM 		= 150;
  conversions.potK 			= 3;
  conversions.potM 			= -800;
  conversions.aFiltServo_f 	= 0.5;
  conversions.aFiltPot_f 	= 0.1;
  conversions.aFiltNEng_f	= 0.1;
  conversions.nEngRefMin	= N_ENG_MIN;
  conversions.nEngRefMax	= N_ENG_MAX;

  attachInterrupt(W_INTERRUPT,wInterrupt, RISING);
  pinMode(LED_BUILTIN, OUTPUT);
  
}

/**@brief
Handle all inputs
*/
void handleInputs()
{

  
  // Inputs
  status.servoPosRaw_u16 = analogRead(MOTOR_POS);
  status.potInCabRaw_u16 = analogRead(REF_IN);
  getNEngSample();
  // status.nEng_f = 400.0; // Remove this! Only for test

  // y=kx+m conversion of the inputs
  status.servoPos_f = conversions.servoK*(float)status.servoPosRaw_u16 + conversions.servoM;
  status.potInCab_f = conversions.potK*(float)status.potInCabRaw_u16 + conversions.potM;

  // Create filtered values
  status.servoPosFilt_f = status.servoPosFilt_f*(1.0-conversions.aFiltServo_f) + status.servoPos_f*conversions.aFiltServo_f;
  status.potInCabFilt_f = status.potInCabFilt_f*(1.0-conversions.aFiltPot_f) + status.potInCab_f*conversions.aFiltPot_f;
  status.nEngFilt_f		= status.nEngFilt_f*(1.0-conversions.aFiltNEng_f) + status.nEng_f*conversions.aFiltNEng_f;




}

uint32_t handleOutputs()
{
  static uint32_t millisOld;
  
  status.servoOutput_u16=(int)pid_servo.getOutput();
  
  // Stop motor if the output is small enough
  if(abs(status.servoOutput_u16)<40)
  {
    motor.stop();
  }
  else
  {
    // Inverse output
    motor.speed(-status.servoOutput_u16);
  }

  if((millis()-millisOld)> 100)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    millisOld = millis();
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }    return millisOld;
}

/** @brief Calculation */
void calculate()
{
  bool forceMotorStopped;
  float nEngFromPotTemp;
  static float u_pid_n_eng = N_ENG_MIN;  // Init with minimal engine speed

  // Engine speed PID calculation
  if(status.nEngRefExtEnable)
  {
    // Use external engine speed reference
    status.nEngRef_u16 = (float)status.nEngRefExt_u16;
  }
  else
  {
    // Use internal engine speed reference

    // Limit the pot reference value
    nEngFromPotTemp = status.potInCabFilt_f;
    if(nEngFromPotTemp<conversions.nEngRefMin)
    {
      nEngFromPotTemp = conversions.nEngRefMin;
    }
    else if(nEngFromPotTemp>conversions.nEngRefMax)
    {
      nEngFromPotTemp = conversions.nEngRefMax;
    }

    status.nEngRef_u16 = nEngFromPotTemp;
  }
  
  pid_n_eng.calculate((double)status.nEngRef_u16, (double)status.nEngFilt_f)

  u_pid_n_eng = pid_n_eng.getOutput();
  
  // Servo PID calculation
  if(status.servoPosRefExtEnable)
  {
    // Use external servo position reference
    status.servoPosRef_f = status.servoPosRefExt_f;
  }
  else
  {
    // Use internal servo position reference
    status.servoPosRef_f = u_pid_n_eng;
  }
  
  pid_servo.calculate((double)status.servoPosRef_f, (double)status.servoPosFilt_f))
}

// the loop routine runs over and over again forever:
void loop() {
  handleInputs();

  calculate();
  
  handleOutputs();
  
  handleSerialComm();
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


