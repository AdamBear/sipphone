#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYCALLPARKACTION_H__
#define __TALK_DSSKEY_DSSKEYCALLPARKACTION_H__

#include "dsskeybindaction.h"

class CDsskeyCallparkAction
    : public CDsskeyBindAction
{
public:
    CDsskeyCallparkAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

private:
    bool EnterCallParkUI(const yl::string& strNumber, const DssKeyLogicData& logicData);

    bool HavingParkCall(int iDsskey, const DssKeyLogicData& logicData);
    bool IsCallParkUsable(int iDsskey, const DssKeyLogicData& logicData);
    void SetCallPark(int iDsskey, const DssKeyLogicData& logicData);
    void BlindTransferForCallpark(const CSessionStateProxy& proxy, const yl::string& strNumber);
    bool CanFetchPark(int iDsskey, const DssKeyLogicData& logicData);
    bool ParkInTalking();
    static int GetRemoteStatus(int iDsskey);
};


#endif // __TALK_DSSKEY_DSSKEYHOLDACTION_H__
#endif  //IF_FEATURE_DSSKEY
