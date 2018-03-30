#ifndef _GENESYSACD_H_
#define _GENESYSACD_H_

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_GENESYS_ACD)
#include "acdmodelcosmocom.h"

class CACDModelGenesys : public CACDModelCosmocom
{
public:
    CACDModelGenesys(void);
    ~CACDModelGenesys(void);

    // 消息处理
    virtual LRESULT OnMessage(msgObject & msg);

    // 登录ACD
    virtual bool ReqLogin(acd_accout * pAcd = NULL);

    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction);

    virtual void SetStatus(ACD_STATUS eStatus);

    void OnACDNotify();

    bool GetACDNotifyFlag()
    {
        return m_bReqChang;
    }

private:
    ACD_STATUS Action2Status();

private:
    bool            m_bReqChang;      // 是否有更改ACD状态
    ACD_ACTION      m_eActionWanted;  // 上一次请求的ACD状态
    YLHashMap<int, int> m_mapTalkInfo; // Key=SessID Value=iAcc
};

#endif //IF_FEATURE_GENESYS_ACD

#endif

