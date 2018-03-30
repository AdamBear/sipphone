#include "drivelogic.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

CExpBaseDrive * GetDriveByPhoneType(EXP_DRIVE_TYPE eDriveType)
{
	switch (eDriveType)
	{
		case  EXP_DRIVE_TYPE_INVALID:
		{
			return NULL;
		}
		break;
#ifdef _EXP_COLOR_SUPPORT
		case EXP_DRIVE_TYPE_USB:
		{
			if (configParser_GetConfigInt(kszUsbPortEnable) == 0)
			{
				EXP_INFO("try to instantiate usb exp drive fail, usb port disabled.");
				return NULL;
			}
			else
			{
				EXP_INFO("try to instantiate usb exp drive ");
				return dynamic_cast<CExpBaseDrive *>(&g_USBDrive);
			}
		}
		break;
#endif

#if defined(_EHS_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
		case EXP_DRIVE_TYPE_RJ:
		{
			EXP_INFO("try to instantiate rj exp drive ");
			return dynamic_cast<CExpBaseDrive *>(&g_RJDrive);
		}
		break;
#endif
		default:
			break;
	}

	return NULL;
}

void DriveInitByType(EXP_DRIVE_TYPE eDriveType)
{
	CExpBaseDrive* expScanDrive = ::GetDriveByPhoneType(eDriveType);

	if (expScanDrive == NULL)
	{
		return;
	}

	//驱动初始化单独处理
	expScanDrive->InitDrive();
	expScanDrive->StartInsertScan();
}