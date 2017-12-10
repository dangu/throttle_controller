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

#endif
