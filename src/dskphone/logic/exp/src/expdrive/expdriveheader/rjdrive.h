#ifndef  _EXP_RJ_DRIVER_H__
#define  _EXP_RJ_DRIVER_H__
#if defined(_EHS_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "exp_struct.h"
#include "expbasedrive.h"
#include "exp_drv.h"

#define EXP_DRIVER_UN_INITED -1
#define EXP_INVALID_EXP_ID   -1
#define EXP20_HWV "1"   // used in FixExpTypeByHW()

class CRJDrive : public CExpBaseDrive
{
public:

    static CRJDrive & GetInstance();

    virtual void StartKeyScan();

    virtual void StartInsertScan();

    virtual void InitDrive();

    virtual void InitExpByIndex(ExpID expId);

public:

    CRJDrive();
    virtual ~CRJDrive();
    CRJDrive(const CRJDrive &);

public:

    DEVICE_TYPE_DEF GetExpTypeByIndex(ExpID expId);

    int GetConnectedExpCounts();

    DEVICE_TYPE_DEF GetEventDevType(int iEventType, int iExpIndex);

    /**********************************************************************************************//**
     * @fn  DsskeyID CRJDrive::KeyCode2DsskeyID(KeyID keyId);
     *
     * @brief   Key code TO dsskey identifier.
     * @param   keyId   Identifier for the key.
     *
     * @return  A DsskeyID.
     * @note    This is not a simple dsskey id . Actually it was concluded by three parts:
     *          exptype, expid, keyid(simple)
     **************************************************************************************************/

    DsskeyID KeyCode2DsskeyID(KeyID keyId, DEVICE_TYPE_DEF eType, ExpID expId);

    LRESULT RjScanFunc();

    void EhsCtrl(ExpID expId, int iCrtlCode);

	DEVICE_TYPE_DEF GetExpTypeByExpIndex(ExpID expId);

	virtual ExpHandle GetExpHandle();
private:

    inline bool IsInited()
    {
        return m_ExpHandle != EXP_DRIVER_UN_INITED;
    }

private:

    int GetEventType();

    ExpID GetEventExpID();

    KeyID GetEventKeyID();

    void InitConnectedExp();

    ExpType FixExpTypeByHW(ExpType expType, ExpID expId);

    EXP_SCANNER_MESSAGE GetMsgByEventType(int iEventCode);

	bool RecordExpTypeWithExpId(ExpID expId, DEVICE_TYPE_DEF eType);

	DEVICE_TYPE_DEF ExpType2DevType(ExpType expType);
private:

    ExpHandle m_ExpHandle;

    struct exp_event m_ExpEvent;

	//插入的设备列表
	YLVector<DEVICE_TYPE_DEF> m_vecDeviceList;
};
#define g_RJDrive CRJDrive::GetInstance()

#endif
#endif // !_EXP_RJ_DRIVER_H__








