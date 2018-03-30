#ifndef _FACTORYH_
#define _FACTORYH_

#include <ylvector.h>
#include <ETLLib.hpp>

class CLedLamp;
/* 取相应灯号的灯 */
CLedLamp * _GetLedLamp(int nLightNum);
void _RemoveLedLamp(int nLightNum);
//灭掉所有灯
void _AllLightOff();
// 获取灯数组
YLVector<CLedLamp *> & _GetAllLamp();

#endif
