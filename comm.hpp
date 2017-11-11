#ifndef COMM_H
#define COMM_H

void handleSerialComm();
void handleCommand(uint8_t rxBuf[]);

typedef struct {
	uint8_t		nEngRefExtEnable:1;		//!< Enable external engine speed reference
	uint8_t		servoPosExtEnable:1;	//!< Enable external servo position
	uint8_t		padding:6;				//!< Padding to 8 bit boundary

	uint16_t	nEngRefExt_u16;			//!< External engine speed reference
	float		servoPosExt_f;			//!< External servo position
	float		nEng_f;					//!< Measured engine speed
	uint16_t	servoPosRaw_u16;		//!< Measured servo position (A/D-converter value)
	uint16_t	potInCabRaw_u16;		//!< Measured cab potentiometer value (A/D-converter value)
}status_t;

typedef struct {
	float		servoK;					//!< y=kx+m parameter for servo
	float		servoM;					//!< y=kx+m parameter for servo
	float		potK;					//!< y=kx+m parameter for pot
	float		potM;					//!< y=kx+m parameter for pot
}conversions_t;

#endif
