#ifdef IF_FEATURE_DSSKEY

#if IF_FEATURE_METASWITCH_CALLPARK
#ifndef __TALK_DSSKEY_DSSKEYMTSWCALLPARKACTION_H__
#define __TALK_DSSKEY_DSSKEYMTSWCALLPARKACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyMTSWCallparkAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyMTSWCallparkAction();
    virtual ~CDsskeyMTSWCallparkAction(void);

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
};


#endif // __TALK_DSSKEY_DSSKEYMTSWCALLPARKACTION_H__
#endif
#endif
