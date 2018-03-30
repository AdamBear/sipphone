#ifndef __XPHONEDEV_MODPHONEDEV_H__
#define __XPHONEDEV_MODPHONEDEV_H__

#include "phonedev.h"

CPhoneDev* GetPhoneDev(const yl::string& strConfigFile);

#ifdef _CP920
bool PhoneDev_IsKeyMute2(int nRawKey);
bool PhoneDev_IsKeyMute3(int nRawKey);
#endif

#endif // __XPHONEDEV_MODPHONEDEV_H__
