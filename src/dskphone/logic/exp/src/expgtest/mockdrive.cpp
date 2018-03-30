#include "mockdrive.h"
//#include "../expdriveheader/rjdrive.h"
//#include "../expdriveheader/usbdrive.h"


// casuse we use c-style instance, we need a global pointer
CMockRjScanDrive * g_MockRjScanDrive;
CMockUsbScanDrive * g_MockUsbScanDrive;

int FakeDssId(DEVICE_TYPE_DEF eType, int ikeyIndex, int iExpIndex)
{

    int iKeyAfterMap = EXP_KEY_NONE;
    // 为测试方便，暂时不判断EXP
    if (DT_EXP_38 == eType)
    {
        switch (ikeyIndex)
        {
        case EXP38_KEY_1:
            iKeyAfterMap = 0;
            break;
        case EXP38_KEY_2:
            iKeyAfterMap = 1;
            break;
        case EXP38_KEY_3:
            iKeyAfterMap = 2;
            break;
        case EXP38_KEY_4:
            iKeyAfterMap = 3;
            break;
        case EXP38_KEY_5:
            iKeyAfterMap = 4;
            break;
        case EXP38_KEY_6:
            iKeyAfterMap = 5;
            break;
        case EXP38_KEY_7:
            iKeyAfterMap = 6;
            break;
        case EXP38_KEY_8:
            iKeyAfterMap = 7;
            break;
        case EXP38_KEY_9:
            iKeyAfterMap = 8;
            break;
        case EXP38_KEY_10:
            iKeyAfterMap = 9;
            break;
        case EXP38_KEY_11:
            iKeyAfterMap = 10;
            break;
        case EXP38_KEY_12:
            iKeyAfterMap = 11;
            break;
        case EXP38_KEY_13:
            iKeyAfterMap = 12;
            break;
        case EXP38_KEY_14:
            iKeyAfterMap = 13;
            break;
        case EXP38_KEY_15:
            iKeyAfterMap = 14;
            break;
        case EXP38_KEY_16:
            iKeyAfterMap = 15;
            break;
        case EXP38_KEY_17:
            iKeyAfterMap = 16;
            break;
        case EXP38_KEY_18:
            iKeyAfterMap = 17;
            break;
        case EXP38_KEY_19:
            iKeyAfterMap = 18;
            break;
        case EXP38_KEY_20:
            iKeyAfterMap = 19;
            break;
        case EXP38_KEY_21:
            iKeyAfterMap = 20;
            break;
        case EXP38_KEY_22:
            iKeyAfterMap = 21;
            break;
        case EXP38_KEY_23:
            iKeyAfterMap = 22;
            break;
        case EXP38_KEY_24:
            iKeyAfterMap = 23;
            break;
        case EXP38_KEY_25:
            iKeyAfterMap = 24;
            break;
        case EXP38_KEY_26:
            iKeyAfterMap = 25;
            break;
        case EXP38_KEY_27:
            iKeyAfterMap = 26;
            break;
        case EXP38_KEY_28:
            iKeyAfterMap = 27;
            break;
        case EXP38_KEY_29:
            iKeyAfterMap = 28;
            break;
        case EXP38_KEY_30:
            iKeyAfterMap = 29;
            break;
        case EXP38_KEY_31:
            iKeyAfterMap = 30;
            break;
        case EXP38_KEY_32:
            iKeyAfterMap = 31;
            break;
        case EXP38_KEY_33:
            iKeyAfterMap = 32;
            break;
        case EXP38_KEY_34:
            iKeyAfterMap = 33;
            break;
        case EXP38_KEY_35:
            iKeyAfterMap = 34;
            break;
        case EXP38_KEY_36:
            iKeyAfterMap = 35;
            break;
        case EXP38_KEY_37:
            iKeyAfterMap = 36;
            break;
        case EXP38_KEY_38:
            iKeyAfterMap = 37;
            break;
        default:
            break;
        }
    }
    else if (DT_EXP_20 == eType
             || DT_EXP_39 == eType
             || DT_EXP_40 == eType
             || DT_EXP_CM69 == eType)
    {
        switch (ikeyIndex)
        {
        case EXP_INSERT:
            iKeyAfterMap = EXP_INSERT;
            break;
        case EXP_REMOVE:
            iKeyAfterMap = EXP_REMOVE;
            break;
        case EXP_KEY_RELEASE:
            iKeyAfterMap = EXP_KEY_RELEASE;
            break;
        case EXP40_KEY_1:
            iKeyAfterMap = 0;
            break;
        case EXP40_KEY_2:
            iKeyAfterMap = 1;
            break;
        case EXP40_KEY_3:
            iKeyAfterMap = 2;
            break;
        case EXP40_KEY_4:
            iKeyAfterMap = 3;
            break;
        case EXP40_KEY_5:
            iKeyAfterMap = 4;
            break;
        case EXP40_KEY_6:
            iKeyAfterMap = 5;
            break;
        case EXP40_KEY_7:
            iKeyAfterMap = 6;
            break;
        case EXP40_KEY_8:
            iKeyAfterMap = 7;
            break;
        case EXP40_KEY_9:
            iKeyAfterMap = 8;
            break;
        case EXP40_KEY_10:
            iKeyAfterMap = 9;
            break;
        case EXP40_KEY_11:
            iKeyAfterMap = 10;
            break;
        case EXP40_KEY_12:
            iKeyAfterMap = 11;
            break;
        case EXP40_KEY_13:
            iKeyAfterMap = 12;
            break;
        case EXP40_KEY_14:
            iKeyAfterMap = 13;
            break;
        case EXP40_KEY_15:
            iKeyAfterMap = 14;
            break;
        case EXP40_KEY_16:
            iKeyAfterMap = 15;
            break;
        case EXP40_KEY_17:
            iKeyAfterMap = 16;
            break;
        case EXP40_KEY_18:
            iKeyAfterMap = 17;
            break;
        case EXP40_KEY_19:
            iKeyAfterMap = 18;
            break;
        case EXP40_KEY_20:
            iKeyAfterMap = 19;
            break;
        case EXP40_KEY_PAGE1:
            iKeyAfterMap = 20;
            break;
        case EXP40_KEY_PAGE2:
            iKeyAfterMap = 21;
            break;
        default:
            break;
        }
    }
    else if (DT_EXP_50 == eType)
    {

        switch (ikeyIndex)
        {
        case EXP50_KEY_1:
            iKeyAfterMap = 0;
            break;
        case EXP50_KEY_2:
            iKeyAfterMap = 2;
            break;
        case EXP50_KEY_3:
            iKeyAfterMap = 4;
            break;
        case EXP50_KEY_4:
            iKeyAfterMap = 6;
            break;
        case EXP50_KEY_5:
            iKeyAfterMap = 8;
            break;
        case EXP50_KEY_6:
            iKeyAfterMap = 10;
            break;
        case EXP50_KEY_7:
            iKeyAfterMap = 12;
            break;
        case EXP50_KEY_8:
            iKeyAfterMap = 14;
            break;
        case EXP50_KEY_9:
            iKeyAfterMap = 16;
            break;
        case EXP50_KEY_10:
            iKeyAfterMap = 18;
            break;
        case EXP50_KEY_11:
            iKeyAfterMap = 1;
            break;
        case EXP50_KEY_12:
            iKeyAfterMap = 3;
            break;
        case EXP50_KEY_13:
            iKeyAfterMap = 5;
            break;
        case EXP50_KEY_14:
            iKeyAfterMap = 7;
            break;
        case EXP50_KEY_15:
            iKeyAfterMap = 9;
            break;
        case EXP50_KEY_16:
            iKeyAfterMap = 11;
            break;
        case EXP50_KEY_17:
            iKeyAfterMap = 13;
            break;
        case EXP50_KEY_18:
            iKeyAfterMap = 15;
            break;
        case EXP50_KEY_19:
            iKeyAfterMap = 17;
            break;
        case EXP50_KEY_20:
            iKeyAfterMap = 19;
            break;
        //底层给的翻页键值是反过来的
        case EXP50_KEY_PAGE1:
            iKeyAfterMap = EXP50_KEY_PAGE3;
            break;
        case EXP50_KEY_PAGE2:
            iKeyAfterMap = EXP50_KEY_PAGE2;
            break;
        case EXP50_KEY_PAGE3:
            iKeyAfterMap = EXP50_KEY_PAGE1;
            break;
        default:
            iKeyAfterMap = ikeyIndex;
            break;
        }
    }

    return ((eType << 24) + (iExpIndex << 6) + iKeyAfterMap);


}



CRJDrive & CRJDrive::GetInstance()
{
    return *(dynamic_cast<CRJDrive *>(g_MockRjScanDrive));
}

CRJDrive::CRJDrive()
{

}

CRJDrive::~CRJDrive()
{

}

void CRJDrive::StartKeyScan()
{
    return g_MockRjScanDrive->StartKeyScan();
}

void CRJDrive::StartInsertScan()
{
    return g_MockRjScanDrive->StartInsertScan();
}

void CRJDrive::InitDrive()
{
    return g_MockRjScanDrive->InitDrive();
}

void CRJDrive::InitExpByIndex(ExpID expId)
{
    return g_MockRjScanDrive->InitExpByIndex(expId);
}

CRJDrive::CRJDrive(const CRJDrive &)
{

}

CUSBDrive & CUSBDrive::GetInstance()
{
    return *(dynamic_cast<CUSBDrive *>(g_MockUsbScanDrive));
}


CUSBDrive::CUSBDrive()
{

}

CUSBDrive::CUSBDrive(const CUSBDrive &)
{

}

CUSBDrive::~CUSBDrive()
{

}

void CUSBDrive::StartKeyScan()
{
    return g_MockUsbScanDrive->StartKeyScan();
}

void CUSBDrive::StartInsertScan()
{
    return g_MockUsbScanDrive->StartInsertScan();
}

void CUSBDrive::InitDrive()
{
    return g_MockUsbScanDrive->InitDrive();
}

void CUSBDrive::InitExpByIndex(ExpID expId)
{
    return g_MockUsbScanDrive->InitExpByIndex(expId);
}




