#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "chip.h"
extern void watchdog_INIT(int feedTime);
extern void watchdog_feedDog(void);
extern void watchdog_disable(void);
extern void watchdog_enable(void);
extern bool watchdog_feed_flag;
#endif
