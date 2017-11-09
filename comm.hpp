#ifndef COMM_H
#define COMM_H

void handleCommand(uint8_t rxBuf[]);

typedef struct {
	uint8_t		nEngRefExtEnable:1;		//!< Enable external engine speed reference
	uint8_t		servoPosExtEnable:1;	//!< Enable external servo position
	uint8_t		padding:6;				//!< Padding to 8 bit boundary

	uint16_t	nEngRefExt_u16;			//!< External engine speed reference
	float		servoPosExt_f;			//!< External servo position
}status_t;

#endif
