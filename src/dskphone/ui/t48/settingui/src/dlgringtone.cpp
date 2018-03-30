#include "dlgringtone.h"
#include "commonunits/modcommonunits.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "mainwnd/mainwnd.h"
#include "actiondefinecommon.h"
#include "account/include/modaccount.h"
#include "settingui/include/modsettingui.h"


///////////////////////////////////////////////////////////////////////////////
CDlgRingtone::CDlgRingtone(QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
{
    m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG);
    InitData();
}

CDlgRingtone::~CDlgRingtone(void)
{
}

void CDlgRingtone::InitData()
{
    m_vecAccount.clear();

    QString strLab;
    QWidgetPair qPair(SETTING_ITEM_MENU);
    strLab = LANG_TRANSLATE(TRID_COMMON_RINGTONE);

    // common
    qPair.first = strLab;
    qPair.second = NULL;
    qPair.third = ACTION_ENTER;
    m_vecWidgetPair.push_back(qPair);
    m_vecAccount.push_back(0);

    // account
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        if (acc_IsAccountAvailable(i))
        {
            yl::string strAccountLab = acc_GetAccountShowText(i);
            qPair.first = toQString(strAccountLab);
            qPair.second = NULL;
            qPair.third = ACTION_ENTER;
            m_vecWidgetPair.push_back(qPair);
            m_vecAccount.push_back(i + 1);
        }
    }

    AddWidget(m_vecWidgetPair, true, false, true, false);

    this->installEventFilter(this);
}

bool CDlgRingtone::OnAction(const QString & strAction)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    int iCurIndex = -1;
    if (NULL != m_pListDelegate)
    {
        iCurIndex = m_pListDelegate->GetFocusItemDataIndex();
    }
    if (iCurIndex < 0 || iCurIndex > m_vecAccount.size())
    {
        return false;
    }

    bool bResult = false;
    if (strAction == ACTION_ENTER)
    {
        SubMenuData objData;
        objData.strMenuAction = commonAPI_FormatString("OpenSubPage(accountsound_ringtone_%d)",
                                m_vecAccount[iCurIndex]);
        objData.pBeforeSetSubpageData = &(m_vecAccount[iCurIndex]);
        emit openpage(objData);

        bResult = true;
    }
    else
    {
        //none
    }

    return bResult;
}

bool CDlgRingtone::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_pGeneralFrame == NULL || objWndData.size() < 4)
    {
        return false;
    }

    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

/*
bool CDlgRingtone::LoadPageData()
{
    this->setFocus();
    return true;
}*/


// 过滤器
/*bool CDlgRingtone::eventFilter(QObject *pObject, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent* pKeyEvent = (QKeyEvent*)pEvent;
        switch (pKeyEvent->key())
        {
        case PHONE_KEY_ENTER:
            {
                if (MsgBox_GetPopupBoxCount() == 0)
                {
                    OnAction(ACTION_ENTER);
                    return true;
                }
            }
            break;
        default:
            break;
        }//end switch (pKeyEvent->key())
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}*/
