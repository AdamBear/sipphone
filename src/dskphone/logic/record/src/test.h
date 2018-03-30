#ifndef __TEST_RECORD_H__
#define __TEST_RECORD_H__

#ifdef _RECORD_TEST_

#include <yllist.h>
#include <ylstring.h>
#include "../include/common.h"
#include <singletonclassdefine.h>

class CRecordTest
{
    DECLARE_SINGLETON_CLASS(CRecordTest)

public:
    bool OnTimer(unsigned int iTimerID);

    void ProcessDevicePlus(int nRet);

    void ProcessDeviceRemove();

    void ProcessDeviceGetStorageInfo();

    void ProcessDeviceGetStorageState();

    void ProcessDeviceGetRecordState();

    void ProcessDeviceGetResourceList(int nRecordType);

    void ProcessDeviceDeleteResource(int nRecordType, const char * pFileName);

    void ProcessDeviceSetDefaultWallpaper(const char * pFileName);

    void ProcessDeviceScreenShot(const char * pFileName);

    void ProcessDeviceRecord(int nRecordType, int nOperation, const char * pFileName);

    void ProcessDevicePlay(int nRecordType, int nOperation, const char * pExtData);

private:
    void TransSizeToString(const unsigned long long & ullSize, yl::string & strOutBuff);
    void TransTimeToString(const unsigned long & lTime, yl::string & strOutBuff);
    void TransRecordTypeToString(RECORD_TYPE eType, yl::string & strOutBuff);
    void TransMessageToString(unsigned int uMsg, yl::string & strOutBuff);

private:
    yl::string m_strDir;

    int m_iDetectingTimerID;

    int m_iRecortingTimerID;

    int m_nRecognizeRet;

    YLList<unsigned long long> m_lstSize;
};

#define _RecordTest GET_SINGLETON_INSTANCE(CRecordTest)

#endif // _RECORD_TEST_

#endif // __TEST_RECORD_H__
