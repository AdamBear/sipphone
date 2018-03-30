#ifndef __POWERSAVING_CONFIG_TEST_H__
#define __POWERSAVING_CONFIG_TEST_H__

#include "powersavingdatadefine.h"

// 加载节电相关配置
void EXPECT_LoadConfig(bool bEnable = true, bool bSmartWakeEnable = true,
                       int timeActiveInterval = 10, int timePassiveIntervalOfWork = 360,
                       int timePassiveIntervalOfRest = 10, int timeFailbackModeInterval = 4320,
                       int iPreSavingBacklightLevel = 1);

void EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_TYPE day, int hourFrom, int hourTo);

#endif // __POWERSAVING_CONFIG_TEST_H__