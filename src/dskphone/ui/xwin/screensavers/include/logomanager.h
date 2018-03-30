#ifndef _LOGO_MANAGER_H_
#define _LOGO_MANAGER_H_

#include "idlescreenui/idlescreenuidefine.h"

#if IF_IDLE_SUPPORT_LOGO
#include <singletonclassdefine.h>
#include "idlescreen/src/idlescreen_inc.h"
#include "xwindow-kernel/include/xthreadtimer.h"

class CLogoManager : public xTimerHandler
{
    DECLARE_SINGLETON_CLASS(CLogoManager)
public:
    static LRESULT OnConfigChangeMsg(msgObject & msg);

    void InitLogo();
    void ResetLogo();
    void CancelLogo();
    bool IsNeedToShowLogo();

private:
    void CheckLogoShow();
    void OnLogoTimer(xThreadTimer* timer, LPVOID pExtraData);
    void ShowLogo(bool bShow, const yl::string & strPic);

public:
    int             m_uDelayShowLogoTime;
    int             m_uShowLogoTime;
    bool            m_bShowLogo;
    bool            m_bCurShowLogo;
    yl::string      m_strLogoPic;
    xThreadTimer    m_timerLogo;
};

#define _LogoManager GET_SINGLETON_INSTANCE(CLogoManager)

#endif

#endif //end of _LOGO_MANAGER_H_

