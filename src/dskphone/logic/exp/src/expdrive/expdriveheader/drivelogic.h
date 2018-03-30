#ifndef __DRIVE_LOGIC_H__
#define __DRIVE_LOGIC_H__

#include "expbasedrive.h"
#include "rjdrive.h"
#include "usbdrive.h"

CExpBaseDrive * GetDriveByPhoneType(EXP_DRIVE_TYPE eDriveType);

void DriveInitByType(EXP_DRIVE_TYPE eDriveType);

#endif // !__DRIVE_LOGIC_H__

