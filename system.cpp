#include "motor.hpp"
#include "Arduino.h"

// The do_reboot is courtesy of this forum post:
// https://github.com/Optiboot/optiboot/issues/180
//
// It seems 1023 has to match BOOTSZ fuse, as it sets
// the size of the bootloader
// Optiboot is placed 256 words = 512 bytes before end of flash, that
// is at 0x7E00 (byte address)
typedef void (*do_reboot_t)(void);
const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND-511)>>1);
// Keeping this old command as it somehow was working, maybe because 0xFFFF
// instructioni is a nop, so that it reached 0x7E00 after a while
const do_reboot_t do_reboot2 = (do_reboot_t)((FLASHEND-1024)>>1);

extern Motor motor;
/** @brief Reboot the MCU to get into the bootloader
 */
void reboot()
{
	motor.stop();  // Prevent motor runaway during reset
	cli();

	MCUSR=0;
	do_reboot();
}

void reboot2()
{
	motor.stop();  // Prevent motor runaway during reset
	cli();

	MCUSR=0;
	do_reboot2();
}

