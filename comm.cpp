#include "Arduino.h"
#include "comm.hpp"
#include "system.hpp"
#include "pid.hpp"

extern status_t status;
extern parameters_t parameters;

extern PID pid_servo;
extern PID pid_n_eng;

extern Converter convServoPos;
extern Converter convPot;

#define CMD_ENABLE_EXT_N_ENG_REF	'a'
#define CMD_DISABLE_EXT_N_ENG_REF	'b'
#define CMD_ENABLE_EXT_SERVO_POS	'c'
#define CMD_DISABLE_EXT_SERVO_POS	'd'

#define CMD_DISP_VALUES				'i'
#define RESP_DISP_VALUES 			'I'

#define CMD_DISP_PID_PARAMS			'g'
#define RESP_DISP_PID_PARAMS 		'G'

#define CMD_SET_CONVERSION_PARAMS  	'j'

#define CMD_DISP_CONVERSION_PARAMS  'k'
#define RESP_DISP_CONVERSION_PARAMS 'K'

#define DECIMALS_IN_DISPLAY	5

void displayPIDParams()
{
	Serial.print(RESP_DISP_PID_PARAMS);
	Serial.print(" ");
	Serial.print(pid_servo.getPGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_servo.getIGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_servo.getDGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_servo.getUMin(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_servo.getUMax(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_n_eng.getPGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_n_eng.getIGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_n_eng.getDGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_n_eng.getUMin(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid_n_eng.getUMax(),DECIMALS_IN_DISPLAY);
	Serial.print('\n');
}

void displayValues()
{
	Serial.print(RESP_DISP_VALUES);
	Serial.print(" ");
	Serial.print((int)status.mode_e);
	Serial.print(" ");
	Serial.print((int)status.nEngStatus_e);
	Serial.print(" ");
	Serial.print(status.nEngRef_u16);
	Serial.print(" ");
	Serial.print(status.nEng_f);
	Serial.print(" ");
	Serial.print(status.servoPosRaw_u16);
	Serial.print(" ");
	Serial.print(status.potInCabRaw_u16);
	Serial.print(" ");
	Serial.print(status.servoPosRef_f);
	Serial.print(" ");
	Serial.print(status.servoPosFilt_f);
	Serial.print(" ");
	Serial.print(status.potInCabFilt_f);
	Serial.print(" ");
	Serial.print(status.servoOutput_u16);
	Serial.print('\n');
}

/** @brief Display conversion parameters */
void displayConversionParams()
{
	Serial.print(RESP_DISP_CONVERSION_PARAMS);
	Serial.print(" ");
	Serial.print(parameters.servoADMax_u16);
	Serial.print(" ");
	Serial.print(parameters.servoADMin_u16);
	Serial.print(" ");
	Serial.print(parameters.potADMax_u16);
	Serial.print(" ");
	Serial.print(parameters.potADMin_u16);
	Serial.print(" ");
	Serial.print(parameters.aFiltServo_f,DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(parameters.aFiltPot_f,DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(parameters.aFiltNEng_f,DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(parameters.nEngRefMin);
	Serial.print(" ");
	Serial.print(parameters.nEngRefMax);
	Serial.print('\n');
}

void handleSerialComm()
{
	static uint8_t rxBuf[100];
	static uint8_t rxBufIn;

	if(Serial.available() > 0)
	{
		char chr;
		chr = Serial.read();

		switch(chr)
		{
		/*   case '0':
  	// Turn off servo
  	Serial.println("Stopping motor...");
  	motor.stop();
		forceMotorStopped = true;
		break;*/

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
}

/** @brief Handle command
 *
 * Handle a serial command
 */
void handleCommand(uint8_t rxBuf[])
{
	char delimiter[] = " ";
	char *cmd, *strData;
	float data[10];
	uint8_t nData=0;

	cmd = strtok((char *)rxBuf, delimiter);

	while((strData = strtok(0, delimiter)))
	{
		data[nData++] = atof(strData);
		if(nData>sizeof(data))
			break;		// Prevent buffer overflow
	}

	switch(cmd[0])
	{
	case 'R':
		reboot();
		break;

	case 'S':
		reboot2();
		break;

	case CMD_ENABLE_EXT_N_ENG_REF:
		if(nData == 1)
		{
			// Enable external engine speed reference
			Serial.println("External engine speed reference");
			status.nEngRefExtEnable = true;
			status.nEngRefExt_u16 = data[0];
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;
	case CMD_DISABLE_EXT_N_ENG_REF:
		// Disable external engine speed reference
		status.nEngRefExtEnable = false;
		Serial.println("OK");
		break;

	case CMD_ENABLE_EXT_SERVO_POS:
		if(nData == 1)
		{
			// Enable external servo position
			status.servoPosRefExtEnable = true;
			status.servoPosRefExt_f = data[0];
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case CMD_DISABLE_EXT_SERVO_POS:
		// Disable external servo position
		status.servoPosRefExtEnable = false;
		Serial.println("OK");
		break;

	case 'e':
		// Set servo PID parameters
		if(nData == 5)
		{
			pid_servo.setPGain(data[0]);
			pid_servo.setIGain(data[1]);
			pid_servo.setDGain(data[2]);
			pid_servo.setUMin(data[3]);
			pid_servo.setUMax(data[4]);
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case 'f':
		// Set engine speed PID parameters
		if(nData == 5)
		{
			pid_n_eng.setPGain(data[0]);
			pid_n_eng.setIGain(data[1]);
			pid_n_eng.setDGain(data[2]);
			pid_n_eng.setUMin(data[3]);
			pid_n_eng.setUMax(data[4]);
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case CMD_DISP_PID_PARAMS:
		// Get PID parameters
		displayPIDParams();
		break;

	case CMD_DISP_VALUES:
		// Display current values
		displayValues();
		break;

	case CMD_SET_CONVERSION_PARAMS:
		// Set conversion parameters
		if(nData == 7)
		{
			parameters.servoADMax_u16	= data[0];
			parameters.servoADMin_u16	= data[1];
			parameters.potADMax_u16		= data[2];
			parameters.potADMin_u16		= data[3];
			parameters.aFiltServo_f 	= data[4];
			parameters.aFiltPot_f 		= data[5];
			parameters.aFiltNEng_f		= data[6];

			// Calculate new kx+m parameters
			convServoPos.calcKM(parameters.servoADMax_u16, parameters.servoADMin_u16, 100, 0);
			convPot.calcKM(parameters.potADMax_u16, parameters.potADMin_u16, 100, 0);
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case CMD_DISP_CONVERSION_PARAMS:
		// Display conversion parameters
		displayConversionParams();
		break;

	default:
		break;
	}
}
