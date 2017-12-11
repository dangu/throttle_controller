#ifndef SYSTEM_H
#define SYSTEM_H

void reboot();
void reboot2();

/** @brief Class for measuring task time */
class TaskTimer {
  public:
  uint32_t t0;    //!< [us] Start time
  uint32_t tMin;  //!< [us] Minimum time
  uint32_t tMax;  //!< [us] Maximum time
  void init();
  void start();
  void stop();
};

/** @brief Main modes */
enum mode_t {
  OFF = 0,
  START,
  NORMAL
};

/** @brief Status of the engine speed signal */
enum nEngStat_t {
  INIT = 0,
  OK,
  TOO_OLD,
  ERROR
};

typedef struct {
  mode_t    mode_e;                   //!< Main mode
  uint8_t		nEngRefExtEnable:1;     //!< Enable external engine speed reference
  uint8_t		servoPosRefExtEnable:1; //!< Enable external servo position
  uint8_t		padding:6;              //!< Padding to 8 bit boundary

  uint16_t	nEngRef_u16;            //!< Engine speed reference
  uint16_t	nEngRefExt_u16;         //!< External engine speed reference
  float		nEng_f;                   //!< Measured engine speed
  float		nEngFilt_f;               //!< Filtered engine speed
  nEngStat_t   nEngStatus_e;             //!< Status of the engine speed signal
  float		servoPosRefExt_f;         //!< External servo position
  float		servoPosRef_f;            //!< Internal servo position
  uint16_t	servoPosRaw_u16;        //!< Measured servo position (A/D-converter value)
  float		servoPos_f;               //!< Converted servo position (0.0-100.0)
  float		servoPosFilt_f;           //!< Filtered servo position
  int16_t		servoOutput_u16;        //!< Output signal to servo
  uint16_t	potInCabRaw_u16;        //!< Measured cab potentiometer value (A/D-converter value)
  float		potInCab_f;               //!< Converted cab potentiometer value (0.0-100.0
  float		potInCabFilt_f;           //!< Filtered cab potentiometer value
}status_t;

typedef struct {
  float		servoK;					//!< y=kx+m parameter for servo
  float		servoM;					//!< y=kx+m parameter for servo
  float		potK;					//!< y=kx+m parameter for pot
  float		potM;					//!< y=kx+m parameter for pot
  float		aFiltServo_f;			//!< Filter constant servo
  float		aFiltPot_f;				//!< Filter constant pot
  float		aFiltNEng_f;			//!< Filter constant engine speed
  uint16_t	nEngRefMin;				//!< Min allowed engine speed reference
  uint16_t	nEngRefMax;				//!< Max allowed engine speed reference
}conversions_t;

#endif
