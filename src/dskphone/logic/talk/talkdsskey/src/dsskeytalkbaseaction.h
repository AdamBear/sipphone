#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_TALK_BASE_H__
#define __TALK_DSSKEY_TALK_BASE_H__

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"

class CallOut_Param;

/* 需要和通话产生关联的dssskey处理的基类 */
class CDssTalkBaseAciton
    : public IDsskeyImplAction
{
public:
    CDssTalkBaseAciton(DssKey_Type type)
        : IDsskeyImplAction(type)
    {
    }

    /* 依据dssskey和通话的关系自动选择 */
    virtual bool CallOutByDsskey(const DssKeyLogicData& logicData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    /* 呼出号码 */
    virtual const yl::string& GetNumber(const DssKeyLogicData& logicData)
    {
        return logicData.strValue;
    }

    /* 触发通话事件 */
    bool SendTalkEvent(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, int iActionType);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    virtual bool IsDTMFRelpalceTransfer(int iAccountID);

    virtual bool IsRelateTalk()
    {
        return true;
    }

    virtual bool IsBLF()
    {
        return false;
    }

protected:

    /* 进拨号 */
    bool EnterDialUI(const DssKeyLogicData& logicData);
    bool EnterDialUI(const DssKeyLogicData& logicData, CallOut_Param& param);

    /* 呼出 */
    bool ProcessCallOut(const DssKeyLogicData& logicData);

    /* 通过Dsskey进入呼出界面 */
    bool ProcessDial(const DssKeyLogicData& logicData);

    /* 复用当前话路呼出 */
    bool DialByCurrent(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    void GetLigaturesText(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    bool TransferDirectly(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    bool ConfTo(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    bool OnActionSend(const CSessionStateProxy& proxy, const yl::string& strNumber);

    CDssTalkBaseAciton* GetImplAction(DssKey_Type eType);
};

#endif


#endif  //IF_FEATURE_DSSKEY
