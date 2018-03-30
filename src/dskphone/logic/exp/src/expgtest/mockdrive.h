#ifndef _EXP_EXP_GTEST_MOCK_DRIVE_H__
#define _EXP_EXP_GTEST_MOCK_DRIVE_H__

#include "expgtestinc.h"

#include "../expdriveheader/rjdrive.h"
#include "../expdriveheader/usbdrive.h"

class CMockRjScanDrive : public CRJDrive
{
public:

    MOCK_METHOD0(InitDrive, void());
    MOCK_METHOD0(StartKeyScan, void());
    MOCK_METHOD0(StartInsertScan, void());
    MOCK_METHOD1(InitExpByIndex, void(int iExpIndex));
    //MOCK_METHOD0(GetInstance(), CMockScanDrive*() );
    //MOCK_METHOD2(EhsCtrl, void(ExpID expId, int iCrtlCode));
};



class CMockUsbScanDrive : public CUSBDrive
{
public:

    MOCK_METHOD0(InitDrive, void());
    MOCK_METHOD0(StartKeyScan, void());
    MOCK_METHOD0(StartInsertScan, void());
    MOCK_METHOD1(InitExpByIndex, void(int iExpIndex));
    //MOCK_METHOD0(GetInstance(), CMockScanDrive*() );
};


extern CMockRjScanDrive * g_MockRjScanDrive;
extern CMockUsbScanDrive * g_MockUsbScanDrive;





int FakeDssId(DEVICE_TYPE_DEF  eType, int ikeyIndex, int iExpIndex);


//
//
//#define  EXPECT_EXP_IN(expType) EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThread1(testing::_,))
//#define  EXPECT_EXP_OUT()
//#define  EXPECT_EXP_KEY_DOWN()
//#define  EXPECT_EXP_KEY_UP()
//
//EXPECT_CALL();












#endif // ! _EXP_EXP_GTEST_MOCK_DRIVE_H__
