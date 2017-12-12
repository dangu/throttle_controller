/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include "motor.hpp"
#include "pid.hpp"
#include "comm.hpp"
#include "system.hpp"
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

#define N_ENG_MIN             400  //!< [rpm] Min allowed engine speed
#define N_ENG_MAX             2400 //!< [rpm] Max allowed engine speed
#define N_ENG_MAX_SAMPLED     3000 //!< [rpm] Max realistically sampled engine speed
#define N_ENG_MAX_SAMPLE_AGE  1000 //!< [ms] max age of engine speed measurement

Motor motor(MOTOR_PWM, MOTOR_A1, MOTOR_A2);

volatile uint32_t wMicrosDiff_u32;
volatile uint32_t wMillisDiff_u32;
volatile uint32_t wMicrosNow_u32;
volatile uint32_t wMillisNow_u32;

PID pid_servo;
PID pid_n_eng;

Converter convPot;

status_t		status;
parameters_t	parameters;
TaskTimer taskTimerMain;
TaskTimer taskTimerController;
TaskTimer taskTimerSerial;

/** @brief Get one sample of the engine speed

This function tries to do some basic checks of the engine speed
and flag it appropriately.
*/
void getNEngSample()
{
  uint32_t wMicrosDiffTmp_u32;
  uint32_t wMillisNowTmp_u32;
  uint32_t wSampleAge_u32;    //!< [ms] The age of the engine speed measurement
  float nEngTmp_f;
  
  // Critical region:
  // The interrupts need to be turned off before reading the
  // variables used inside the interrupt routine
  noInterrupts();
  wMillisNowTmp_u32 = wMillisNow_u32;
  wMicrosDiffTmp_u32 = wMicrosDiff_u32;
  interrupts();
  
  wSampleAge_u32 = millis() - wMillisNowTmp_u32; // Get the age of the engine speed sample
  
  if(wSampleAge_u32 < N_ENG_MAX_SAMPLE_AGE)
  {
    // If the engine speed measurement is new enough
    if(wMicrosDiffTmp_u32 == 0)
    {
      // Assume this is the initial state, before any interrupts.
      // Should never be here actually...
      status.nEngStatus_e = INIT;
    }
    else
    {
      // Calculate the engine speed from the pulse measurement
      // of the alternator. A factor of 4 was found empirically.
      nEngTmp_f=4*1000000/wMicrosDiffTmp_u32;
      if(nEngTmp_f<N_ENG_MAX_SAMPLED)
      {
        // If the measured engine speed is realistic
        status.nEng_f = nEngTmp_f;
        status.nEngStatus_e = OK;
      }
      else
      {
        // Unrealistic engine speed. Keep the old value and flag as error.
        status.nEngStatus_e = ERROR;
      }
    }
  }
  else
  {
    // Waited too long for a pulse which would happen if the engine is stopped.
    // The problem is it could also happen if there is no signal from the alternator.
    // Other logic needs to take care of this.
    status.nEng_f = 0;
    status.nEngStatus_e = TOO_OLD;
  }
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
  parameters.servoK 		      = -0.1667;
  parameters.servoM 		      = 150;
  parameters.potK 			      = 3;
  parameters.potM 			      = -800;
  parameters.aFiltServo_f 	      = 0.5;
  parameters.aFiltPot_f 	      = 0.1;
  parameters.aFiltNEng_f	      = 0.1;
  parameters.nEngRefMin	          = N_ENG_MIN;
  parameters.nEngRefMax	          = N_ENG_MAX;
  parameters.potKickdownSet_u16   = 95;
  parameters.potKickdownReset_u16 = 30;
  
  // Setup value conversions
  convPot.calcKM(1024, 800, 100, 0);

  // Setup task timers
  taskTimerMain.init();
  taskTimerController.init();
  taskTimerSerial.init();
  
  // Init status
  status.mode_e = NORMAL;     // Assume wakeup from reset in normal mode
  status.nEngStatus_e = INIT; // Initial engine speed state

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

  // y=kx+m conversion of the inputs
  status.servoPos_f = parameters.servoK*(float)status.servoPosRaw_u16 + parameters.servoM;
  status.potInCab_f = parameters.potK*(float)status.potInCabRaw_u16 + parameters.potM;

  // Create filtered values
  status.servoPosFilt_f = status.servoPosFilt_f*(1.0-parameters.aFiltServo_f) + status.servoPos_f*parameters.aFiltServo_f;
  status.potInCabFilt_f = status.potInCabFilt_f*(1.0-parameters.aFiltPot_f) + status.potInCab_f*parameters.aFiltPot_f;
  status.nEngFilt_f		= status.nEngFilt_f*(1.0-parameters.aFiltNEng_f) + status.nEng_f*parameters.aFiltNEng_f;
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
  float nEngFromPotTemp;
  static float u_pid_n_eng = N_ENG_MIN;  // Init with minimal engine speed
  mode_t modeNext_e=status.mode_e;                    // The next mode

  // Handle modes
  switch(status.mode_e)
  {
    case OFF:
    // In this mode, it is assumed that the engine is off
    // The servo should be put in starting position to wait for
    // the engine speed to rise above a minimum value
    if(status.nEngStatus_e == OK &&
    status.nEngFilt_f > N_ENG_MIN)
    {
      // If the filtered engine speed is higher than a minimum value,
      // move to START mode
      modeNext_e = START;
    }
    else
    {
      modeNext_e = OFF;
    }
    break;
    case START:
    // In this mode, wait for "kickdown", that is that the throttle is pushed
    // to 100%
    // The reason for this is that the alternator V-belt seems to be
    // slipping right after engine start, causing the measured engine speed
    // to be unreliable and resulting in engine hunting.
    if(status.potInCabFilt_f>parameters.potKickdownSet_u16)
    {
      modeNext_e = KICKDOWN;
    }
    break;
    case KICKDOWN:
    // In this state, wait for the throttle (and pot) to be released below
    // a setpoint, to allow for smooth transition to engine speed control
    if(status.potInCabFilt_f<parameters.potKickdownReset_u16)
    {
      modeNext_e = NORMAL;
    }
    case NORMAL:
    // No engine speed is measured. Go to mode OFF
    if(status.nEngStatus_e == TOO_OLD)
    {
      status.mode_e = OFF;
    }
    else
    {
      // If there is still a valid engine speed sample, stay in mode NORMAL
      modeNext_e = NORMAL;
    }
    break;
    default:
    // Should never be here!
    break;
  }
  
  if(modeNext_e != status.mode_e)
  {
    // Mode change!
    status.mode_e = modeNext_e;
  }

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
    if(nEngFromPotTemp<parameters.nEngRefMin)
    {
      nEngFromPotTemp = parameters.nEngRefMin;
    }
    else if(nEngFromPotTemp>parameters.nEngRefMax)
    {
      nEngFromPotTemp = parameters.nEngRefMax;
    }

    status.nEngRef_u16 = nEngFromPotTemp;
  }
  
  pid_n_eng.calculate((double)status.nEngRef_u16, (double)status.nEngFilt_f);

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
  
  pid_servo.calculate((double)status.servoPosRef_f, (double)status.servoPosFilt_f);
}

// the loop routine runs over and over again forever:
void loop() {
  static uint32_t millisOld;
  uint32_t millisNow;
  static uint32_t tSampleMain=10; //!< [ms] The main sample time to use
  
  millisNow = millis();
  
  taskTimerMain.start();
  if((millisNow-millisOld)>=tSampleMain)
  {
    taskTimerController.start();

    handleInputs();

    calculate();
    
    handleOutputs();
    
    millisOld = millisNow;
    taskTimerController.stop();
  }
  
  taskTimerSerial.start();
  handleSerialComm();
  taskTimerSerial.stop();
  
  taskTimerMain.stop();
}

/** @brief Interrupt callback at every pulse from the alternator

Approximately 2 pulses every 10 ms.
*/
void wInterrupt()
{
  static uint32_t microsOld_u32;
  static uint32_t millisOld_u32;

  wMicrosNow_u32 = micros();  // Timestamp now
  wMillisNow_u32 = millis();
  wMicrosDiff_u32 = wMicrosNow_u32-microsOld_u32;
  wMillisDiff_u32 = wMillisNow_u32-millisOld_u32;

  microsOld_u32 = wMicrosNow_u32;
  millisOld_u32 = wMillisNow_u32;
}


