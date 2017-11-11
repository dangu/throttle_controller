#include "Arduino.h"
#include "comm.hpp"
#include "system.hpp"
#include "pid.hpp"

extern status_t status;
extern PID pid_servo;
extern PID pid_n_eng;

#define CMD_DISP_VALUES				'i'
#define RESP_DISP_VALUES 			'I'

#define CMD_DISP_PID_PARAMS_SERVO	'g'
#define CMD_DISP_PID_PARAMS_N_ENG	'h'
#define RESP_DISP_PID_PARAMS 		'G'

#define DECIMALS_IN_DISPLAY	5

void displayPIDParams(PID *pid)
{
	Serial.print(RESP_DISP_PID_PARAMS);
	Serial.print(" ");
	Serial.print(pid->getPGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid->getIGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid->getDGain(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.print(pid->getUMin(),DECIMALS_IN_DISPLAY);
	Serial.print(" ");
	Serial.println(pid->getUMax(),DECIMALS_IN_DISPLAY);
}

void displayValues()
{
	Serial.print(RESP_DISP_VALUES);
	Serial.print(" ");
	Serial.print(status.nEng_f);
	Serial.print(" ");
	Serial.print(status.servoPosRaw_u16);
	Serial.print(" ");
	Serial.print(status.potInCabRaw_u16);
	Serial.print('\n');
}

void handleSerialComm()
{
	static uint8_t rxBuf[30];
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
	float data[5];
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

	case 'a':
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
	case 'b':
		// Disable external engine speed reference
		status.nEngRefExtEnable = false;
		Serial.println("OK");
		break;

	case 'c':
		if(nData == 1)
		{
			// Enable external servo position
			status.servoPosExtEnable = true;
			status.servoPosExt_f = data[0];
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case 'd':
		// Disable external servo position
		status.servoPosExtEnable = false;
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

	case CMD_DISP_PID_PARAMS_SERVO:
		// Get servo PID parameters
		displayPIDParams(&pid_servo);
		break;

	case CMD_DISP_PID_PARAMS_N_ENG:
		// Get engine speed PID parameters
		displayPIDParams(&pid_n_eng);
		break;

	case CMD_DISP_VALUES:
		// Display current values
		displayValues();

	default:
		break;
	}
}
