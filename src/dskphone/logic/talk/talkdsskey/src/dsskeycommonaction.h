#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYCAMERAACTION_H__
#define __TALK_DSSKEY_DSSKEYCAMERAACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyCommonAction:
    public CDssTalkBaseAciton
{
public:
    CDsskeyCommonAction(DssKey_Type eDsskeyType);

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

protected:
    bool OnBluetoothKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
#ifdef IF_SUPPORT_WIFI
    bool OnWiFiKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
#endif
#ifdef IF_SUPPORT_VIDEO
    bool OnCameraKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
    bool OnExtDisplayKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
#endif
};

#endif //__TALK_DSSKEY_DSSKEYCAMERAACTION_H__
#endif
