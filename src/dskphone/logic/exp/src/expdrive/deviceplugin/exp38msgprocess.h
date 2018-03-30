#ifndef _EXP38_MSG_PROCESSS_H__
#define  _EXP38_MSG_PROCESSS_H__


#include "baseexpmsgprocess.h"
#ifdef _EXP_BLACK_SUPPORT

class CExp38MsgProcess : public CBaseExpMsgProcess
{
public:

    CExp38MsgProcess();
    virtual ~CExp38MsgProcess();

public:

    virtual void KeyProcess(int iKeyStaus, msgObject & msg);

    virtual inline DEVICE_TYPE_DEF  GetMsgProcessType() const
    {
        return DT_EXP_38;
    }
};

#endif
#endif // !_EXP38_MSG_PROCESSS_H__
