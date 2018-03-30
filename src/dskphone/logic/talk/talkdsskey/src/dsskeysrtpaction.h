#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYSRTP_H__
#define __TALK_DSSKEY_DSSKEYSRTP_H__

#ifdef IF_FEATURE_SRTP
#include "dsskeybindaction.h"

class CDsskeySRTPAction
    : public CDsskeyBindAction
{
public:
    CDsskeySRTPAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

};


#endif
#endif // __TALK_DSSKEY_DSSKEYSRTP_H__


#endif  //IF_FEATURE_DSSKEY
