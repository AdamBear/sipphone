#if IF_FEATURE_METASWITCH_ACD
#include "cdlgmtswacdchangestate.h"
#include "messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"

#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "baseui/modbaseui.h"
#include "keymap.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "uikernel/qcomboboxutility.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uimanager/moduimanager.h"
#include "metaswitch_inc.h"


CDlgMTSWAcdChangeState::CDlgMTSWAcdChangeState(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgMTSWAcdChangeState::~CDlgMTSWAcdChangeState()
{
}

void CDlgMTSWAcdChangeState::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_MTSW_ACD_STATE);

    if (m_pGeneralFrame != NULL)
    {
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        QWidgetPair qPair;
        qPair.eType = SETTING_ITEM_RIDIO;
        qPair.third = ACTION_ACD_ITEM_CLICK;

        m_vecOrigName.clear();

        // available
        qPair.first = LANG_TRANSLATE(TRID_AVAILABLE);
        m_vecWidgetPair.push_back(qPair);
        m_vecOrigName.push_back(TRID_AVAILABLE);

        // wrap up
        if (ACD_GetAcdStatus() == AS_WRAPUP)
        {
            yl::string strWrapup = MTSWACD_GetLastAnseredMHLG();
            //如果有code就显示Code，否则显示Unavailable
            if (strWrapup.empty())
            {
                qPair.first = LANG_TRANSLATE(TRID_WRAP_UP);
            }
            else
            {
                char szBuf[128] = {0};
                sprintf(szBuf, "%s(%s)", LANG_TRANSLATE(TRID_WRAP_UP).toUtf8().data(), strWrapup.c_str());
                qPair.first = szBuf;
            }

            m_vecWidgetPair.push_back(qPair);
            m_vecOrigName.push_back(TRID_WRAP_UP);
        }

        YLList<yl::string> listReason;
        MTSWACD_GetUnavailReasonList(listReason);

        YLList<yl::string>::iterator itr = listReason.begin();
        YLList<yl::string>::iterator itrEnd = listReason.end();
        for (; itr != itrEnd; itr++)
        {
            if (!(*itr).empty())
            {
                qPair.first = toQString((*itr).c_str());
                m_vecWidgetPair.push_back(qPair);
                m_vecOrigName.push_back(*itr);
            }
        }

        if (m_vecWidgetPair.size() > 0)
        {
            AddWidget(m_vecWidgetPair, true, false);
        }
    }

    this->installEventFilter(this);
}

// 设置页面的SoftKey
bool CDlgMTSWAcdChangeState::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";//ACTION_OK;
    objWndData[3].m_strSoftkeyTitle = "";//LANG_TRANSLATE(TRID_OK);
    objWndData[3].m_strIconPath = "";//PIC_SOFTKEY_SAVE;

    return true;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgMTSWAcdChangeState::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    return true;
}

bool CDlgMTSWAcdChangeState::LoadPageData()
{
    this->setFocus();

    if (m_pGeneralFrame != NULL)
    {
        ACD_STATUS eStatus = ACD_GetAcdStatus();

        int nIndex = 0;

        if (eStatus == AS_AVAILABLE)
        {
            nIndex = 0;
        }
        else if (eStatus == AS_WRAPUP)
        {
            nIndex = 1;
        }
        else
        {
            yl::string strCurUnavailReason = MTSWACD_GetCurUnavailReason();
            for (int i = 0; i < m_vecOrigName.size(); i++)
            {
                if (strCurUnavailReason == m_vecOrigName[i])
                {
                    nIndex = i;
                    break;
                }
            }
        }

        SetCurrentIndex(nIndex);
        SetConfigedIndex(nIndex);
    }
    return false;
}

bool CDlgMTSWAcdChangeState::SavePageData()
{
    return false;
}

void CDlgMTSWAcdChangeState::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWAcdChangeState::OnShow()
{
}

bool CDlgMTSWAcdChangeState::OnAction(const QString & strAction)
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    bool bHandled = true;

    if (strAction == ACTION_ACD_ITEM_CLICK)
    {
        int nCurIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        if (nCurIndex < 0)
        {
            return false;
        }
        yl::string strState = m_vecOrigName[nCurIndex];
        ACD_STATUS eStatus = ACD_GetAcdStatus();

        if (strState == TRID_AVAILABLE)
        {
            if (eStatus != AS_AVAILABLE)
            {
                ACD_ReqChangeACDStatus(AA_AVAIL);

                if (eStatus == AS_WRAPUP)
                {
                    emit replaceCurrentPage(SubMenuData("mtswacdselectdispcode"));
                }
                else
                {
                    emit BtnBackClick();
                }
            }
        }
        else if (strState == TRID_WRAP_UP)
        {
            return bHandled;
        }
        else
        {
            yl::string strUnavailReason;
            ACD_GetUnavailCode(strUnavailReason);
            if (strUnavailReason == strState)
            {
                return bHandled;
            }
            else
            {
                ACD_SetCurReason(strState);
                ACD_ReqChangeACDStatus(AA_UNAVAIL);
            }

            if (eStatus == AS_WRAPUP)
            {
                emit replaceCurrentPage(SubMenuData("mtswacdselectdispcode"));
            }
            else
            {
                emit BtnBackClick();
            }
        }
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

// 设置子页面前的操作
void CDlgMTSWAcdChangeState::BeforeSetSubPage(void * pData)
{
}

bool CDlgMTSWAcdChangeState::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    OnAction(ACTION_OK);
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWAcdChangeState::IsStatusChanged()
{
    return false;
}
#endif
