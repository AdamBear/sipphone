#ifndef _EXP_USB_DRIVE_H__
#define _EXP_USB_DRIVE_H__
#ifdef _EXP_COLOR_SUPPORT
#include "exp_struct.h"
#include "expbasedrive.h"
#define  USB_INVALID_HANDLE  -1

class CUSBDrive : public CExpBaseDrive
{

public:

    LRESULT USBEXPInsertScanFunc();

    static CUSBDrive & GetInstance();

    virtual void StartKeyScan();

    virtual void StartInsertScan();

    virtual void InitDrive();

    virtual void InitExpByIndex(ExpID expId);

    void OnUSBExpDownloadApp(DEVICE_TYPE_DEF devType, ExpID expId);

    int GetExpStatus(ExpID expId);

	virtual ExpHandle GetExpHandle();
public:

    CUSBDrive();

    virtual ~CUSBDrive();

    CUSBDrive(const CUSBDrive &);

private:

    void LoadUSBExpAppFile();

    void InitInsertScan();

    void InitKeyScan();

    DsskeyID KeyID2DsskeyID(KeyID keyId, ExpID expId);

private:

    inline int GetKeyCodeFromDsskeyID(DsskeyID dssId)
    {
        return (dssId & 0x3f);
    }

    inline ExpID GetExpIndexFromDsskeyID(DsskeyID dssId)
    {
        return ((dssId >> 6) & 0xf);
    }

    inline EXP_SCANNER_MESSAGE KeyStatus2Msg(int iKeyStatus)
    {
        return iKeyStatus == 0 ? EXP_KEY_UP : EXP_KEY_DOWN;
    }

    bool PostMsgWithRetry(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool PostMsgWithRetry(UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                          LPCVOID lpExtraData);

private:

    USBHandle m_KeyScanHandle;

    USBHandle m_InsertScanHandle;

    int  m_nExpStatus[MAX_EXP_NUM];

    bool m_bFileLoaded;

    long m_lFileSize;

    char * m_ExpAppBuffer;
};

#define  g_USBDrive CUSBDrive::GetInstance()

#endif
#endif // _EXP_USB_DRIVE_H__
