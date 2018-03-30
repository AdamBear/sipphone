#ifndef __ACD_MODEL_COSMOCOM_H__
#define __ACD_MODEL_COSMOCOM_H__

#ifdef IF_FEATURE_ACD

#include "acdmodelbase.h"

class CACDModelCosmocom : public CACDModelBase
{
public:
    CACDModelCosmocom(void);
    ~CACDModelCosmocom(void);

    // 登录ACD
    virtual bool ReqLogin(acd_accout * pAcd = NULL);
    //请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction);

    // 设置绑定账号ID
    virtual void SetAccountId(int iAccountId);

private:
    void AutoLogin();

};

#endif  // IF_FEATURE_BROADSOFT_CALL_CENTER

#endif  // __ACD_MODEL_BASE_H__
