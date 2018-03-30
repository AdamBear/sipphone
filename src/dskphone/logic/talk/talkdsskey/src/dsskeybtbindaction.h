#ifdef IF_FEATURE_DSSKEY
#ifndef __TALKLOGIC_DSSKEY_DSSKEYBTACTION_H__
#define __TALKLOGIC_DSSKEY_DSSKEYBTACTION_H__

#include "dsskeybindaction.h"

#ifdef IF_BT_SUPPORT_PHONE

class CDsskeyBTBindAction :
    public CDsskeyBindAction
{
public:
    CDsskeyBTBindAction();
    virtual ~CDsskeyBTBindAction();

    //按键响应
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
    // 液晶修改dsskey类型为BT后的响应处理
    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);
    virtual bool OnDebinded(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    virtual bool ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
};

#endif
#endif //__TALKLOGIC_DSSKEY_DSSKEYBTACTION_H__
#endif
