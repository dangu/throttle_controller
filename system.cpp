#include "motor.hpp"
#include "Arduino.h"
#include "system.hpp"

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

/** @brief Init task timer */
void TaskTimer::init()
{
  tMin = UINT32_MAX;
  tMax = 0;
}

/** @brief Start task timer */
void TaskTimer::start()
{
  t0=micros();
}

/** @brief Stop task timer

This will also update the internal data structures of the timer */
void TaskTimer::stop()
{
  uint32_t dt = micros() - t0;
  
  if(dt<tMin)
  {
    tMin = dt;
  }
  
  if(dt>tMax)
  {
    tMax = dt;
  }
}

/**@brief Calculate conversion parameters */
void Converter::calcKM(uint16_t aIn, uint16_t bIn, uint16_t cIn, uint16_t dIn)
{
  a=aIn;
  b=bIn;
  c=cIn;
  d=dIn;
  
  k = (float)(c-d)/(a-b);
  m = d - (float)b*(c-d)/(a-b);
}  

/**@brief Convert (transform) the input value x
to y= kx+m */
float Converter::convert(float x)
{
  return k*x + m;
}  
