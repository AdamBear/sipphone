#if IF_FEATURE_METASWITCH_ACD
#include "cdlgmtswacdmembers.h"
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
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "metaswitch_inc.h"

CDlgMTSWAcdMembers::CDlgMTSWAcdMembers(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgMTSWAcdMembers::~CDlgMTSWAcdMembers()
{
}

void CDlgMTSWAcdMembers::InitData()
{
    //设置标题
    m_strTitle = toQString(MTSWACD_GetCurrentMLHG().c_str());;

    GetMLHGMembers();

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//获取MLHG列表
void CDlgMTSWAcdMembers::GetMLHGMembers()
{
    MakeReadyForRelayout();

    //设置标题
    m_strTitle = toQString(MTSWACD_GetCurrentMLHG().c_str());;

    // MLHG组成员
    YLVector<SMLHGMemberData> vecMemberList;
    //获取数据
    MTSWACD_GetMLHGMemberList(vecMemberList);

    QWidgetPair qPair;
    QString strLab;

    for (int i = 0; i < vecMemberList.size(); ++i)
    {
        yl::string strDisp = vecMemberList[i].strDirectoryNumber.c_str();
        if (strDisp.empty())
        {
            continue;
        }
        qPair.first = toQString(strDisp.c_str());

        QString strStatus = vecMemberList[i].bLoggedIn ? LANG_TRANSLATE(TRID_MTSW_ACD_LOGIN)
                            : LANG_TRANSLATE(TRID_MTSW_ACD_LOGOUT);
        QLabel * pLabel = new QLabel(this);
        if (pLabel == NULL)
        {
            return;
        }
        pLabel->setText(strStatus);
        qPair.second = pLabel;
        qPair.third = ACTION_SEND;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair);
    emit RefreshTitle();
    UpdateWnd();
}

// 设置页面的SoftKey
bool CDlgMTSWAcdMembers::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgMTSWAcdMembers::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    return true;
}

bool CDlgMTSWAcdMembers::LoadPageData()
{
    return false;
}

bool CDlgMTSWAcdMembers::SavePageData()
{
    return false;
}

void CDlgMTSWAcdMembers::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWAcdMembers::OnShow()
{
}

bool CDlgMTSWAcdMembers::OnAction(const QString & strAction)
{
    bool bHandled = true;

    if (strAction == ACTION_SEND)
    {
        int nIndex = m_pGeneralFrame->GetFocusItemDataIndex();

        if (nIndex >= 0
                && nIndex < m_vecWidgetPair.size())
        {
            yl::string strNumber = m_vecWidgetPair[nIndex].first.toUtf8().data();
            talklogic_CallOut(strNumber, "");
        }
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

// 设置子页面前的操作
void CDlgMTSWAcdMembers::BeforeSetSubPage(void * pData)
{
}

bool CDlgMTSWAcdMembers::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    //OnAction(ACTION_SEND);
                    return true;
                }
            case PHONE_KEY_LEFT:
                {
                    OnPaging(false);
                    return true;
                }
            case PHONE_KEY_RIGHT:
                {
                    OnPaging(true);
                    return true;
                }
            default:
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWAcdMembers::IsStatusChanged()
{
    return false;
}

#if IF_SETTING_WND_PRE_NEXT_BTN
// 向左翻页
void CDlgMTSWAcdMembers::OnBtnPreClick()
{
    OnPaging(false);
}

// 向右翻页
void CDlgMTSWAcdMembers::OnBtnNextClick()
{
    OnPaging(true);
}
#endif

// 按左右键翻页
void CDlgMTSWAcdMembers::OnPaging(bool bNext)
{
    YLVector<SMLHGData> vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);
    yl::string strCurMLHGName = MTSWACD_GetCurrentMLHG();

    for (int i = 0; i < vecMLGHData.size(); i++)
    {
        if (vecMLGHData[i].strName == strCurMLHGName)
        {
            if (bNext)
            {
                if (i == vecMLGHData.size() - 1)
                {
                    i = 0;
                }
                else
                {
                    i++;
                }
            }
            else
            {
                if (i == 0)
                {
                    i = vecMLGHData.size() - 1;
                }
                else
                {
                    i--;
                }
            }
            // 重新设置当前mlhg组
            MTSWACD_SetCurrentMLHG(vecMLGHData[i].strName);

            GetMLHGMembers();
            break;
        }
    }
}
#endif
