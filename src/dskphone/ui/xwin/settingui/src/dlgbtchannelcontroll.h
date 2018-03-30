#ifndef _DLG_BT_CHANNEL_CONTROLL_H_
#define _DLG_BT_CHANNEL_CONTROLL_H_

#ifdef IF_SUPPORT_BLUETOOTH

#include "dlgsettingui.h"

class CDlgBTChannelControll : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgBTChannelControll();
    virtual ~CDlgBTChannelControll();
};

#endif // IF_SUPPORT_BLUETOOTH
#endif //_DLG_BT_CHANNEL_CONTROLL_H_
