#include "Arduino.h"
#include "comm.hpp"
#include "system.hpp"
#include "pid.hpp"

extern status_t status;
extern PID pid_servo;
extern PID pid_n_eng;

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
		if(nData == 3)
		{
			pid_servo.setPGain(data[0]);
			pid_servo.setIGain(data[1]);
			pid_servo.setDGain(data[2]);
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	case 'f':
		// Set engine speed PID parameters
		if(nData == 3)
		{
			pid_n_eng.setPGain(data[0]);
			pid_n_eng.setIGain(data[1]);
			pid_n_eng.setDGain(data[2]);
			Serial.println("OK");
		}
		else
		{
			Serial.println("Error");
		}
		break;

	default:
		break;
	}
}
