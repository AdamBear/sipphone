#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYTIPTELBTNACTION_H__
#define __TALK_DSSKEY_DSSKEYTIPTELBTNACTION_H__
#include "service_sip_interface.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"

#if IF_BUG_30958
class CDsskeyTipTelBtnAciton: public IDsskeyImplAction
{
public:
    CDsskeyTipTelBtnAciton()
        : IDsskeyImplAction(DT_TIPTEL_BUTTON)
    {
    }

    virtual ~CDsskeyTipTelBtnAciton()
    {
    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
    {
        int iKeyIndex = 0;

        if (DMT_LINEKEY_MODULE == logicData.eType)
        {
            iKeyIndex = dsskeyDataID + 1;
        }
        else if (exp_IsEXPKey(dsskeyDataID)) // 处理EXP键
        {
            int iExpIndex = dsskey_GetDsskeyIndexByID(dsskeyDataID);
            iKeyIndex = iExpIndex;
        }

        char szBuf[128] = {0};
        sprintf(szBuf, "Button:%d", iKeyIndex);
        sip_send_notify_tiptel_pbx_button(mkGetHandle(), logicData.iLineIndex, 0, 0, szBuf);
    }
};

#endif //#ifdef IF_FEATURE_TIPTEL_ADAPTER


#endif
#endif
