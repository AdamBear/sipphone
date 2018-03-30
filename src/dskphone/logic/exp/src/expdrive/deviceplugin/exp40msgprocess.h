#ifndef _EXP40_MSG_PROCESSS_H__
#define  _EXP40_MSG_PROCESSS_H__
#include "baseexpmsgprocess.h"
#ifdef _EXP_BLACK_SUPPORT

class CExp40MsgProcess
    : public CBaseExpMsgProcess
{
public:

    CExp40MsgProcess();
    ~CExp40MsgProcess();

public:

    virtual void KeyProcess(int iKeyStaus, msgObject & msg);

    virtual inline DEVICE_TYPE_DEF  GetMsgProcessType() const
    {
        return DT_EXP_40;
    }

private:

    int GetPageIndex(DsskeyID dssId);
};

#endif
#endif  // ! _EXP40_MSG_PROCESSS_H__

