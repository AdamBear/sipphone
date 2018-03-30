#include "dlgbtchannelcontroll.h"

#ifdef IF_SUPPORT_BLUETOOTH

CBaseDialog * CDlgBTChannelControll::CreateInstance()
{
    CDlgBTChannelControll * pDlg = new CDlgBTChannelControll;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgbtchannelcontroll.xml");
    }

    return pDlg;
}

CDlgBTChannelControll::CDlgBTChannelControll()
{

}

CDlgBTChannelControll::~CDlgBTChannelControll()
{

}

#endif
