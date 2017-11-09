#include "Arduino.h"
#include "comm.hpp"
#include "system.hpp"

extern status_t status;

void handleCommand(uint8_t rxBuf[])
{
	char delimiter[] = " ";
	char *cmd, *strData;
	Serial.println("Handling command...");
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

	case 'a':
		Serial.println("a command");
		break;

	default:
		break;
	}

	for(int i=0;i<nData;i++)
	{
		Serial.println(data[i]*2);
	}

}
