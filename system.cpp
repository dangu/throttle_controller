#include "motor.hpp"
#include "Arduino.h"

// The do_reboot is courtesy of this forum post:
// https://github.com/Optiboot/optiboot/issues/180
//
// It seems 1023 has to match BOOTSZ fuse, as it sets
// the size of the bootloader
typedef void (*do_reboot_t)(void);
const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND-1023)>>1);

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
