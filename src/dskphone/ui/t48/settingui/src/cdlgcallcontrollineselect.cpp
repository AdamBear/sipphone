#include "cdlgcallcontrollineselect.h"

#include "keymap.h"
#include "actiondefinecommon.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "baseui/modbaseui.h"

#include "account/include/modaccount.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"

CDlgCallControlLineSelect::CDlgCallControlLineSelect(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

void CDlgCallControlLineSelect::InitData()
{
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_LINE_SELECT).toUtf8().data();

    // 获取账号数
    ListAccounts lstAccount;
    lstAccount = acc_GetAvailableAccountList();

    // 加载控件
    QWidgetPair qPair(SETTING_ITEM_MENU);
    QString strLab;

    ListAccounts::iterator itr = lstAccount.begin();
    for (int i = 0; itr != lstAccount.end(); ++itr, ++i)
    {
        yl::string strName = acc_GetAccountShowText(*itr);
        strLab = QString::fromUtf8(strName.c_str());
        qPair.first = strLab;
        qPair.second = NULL;
        qPair.third = ACTION_ENTER;
        m_vecWidgetPair.push_back(qPair);

        m_mapAssist[*itr] = i;
    }

    AddWidget(m_vecWidgetPair, true);

    // 为界面自己安装事件过滤器
    this->installEventFilter(this);
}

// 设置页面的SoftKey
bool CDlgCallControlLineSelect::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

bool CDlgCallControlLineSelect::OnAction(const QString & strAction)
{
    if (strAction == ACTION_ENTER)
    {
        if (m_pGeneralFrame == NULL)
        {
            return false;
        }

        YLHashMap<int, int>::iterator mapIter = m_mapAssist.begin();
        for (; mapIter != m_mapAssist.end(); ++mapIter)
        {
            if (mapIter->second == m_pGeneralFrame->GetFocusItemDataIndex())
            {
                break;
            }
        }

        if (mapIter != m_mapAssist.end())
        {
            int iLineId = mapIter->first;

            if (!BWCallControl_IsAccountXSIEnable(iLineId))
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                            MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
            }
            else if (!m_strNextPageAction.empty())
            {
                SubMenuData subData(m_strNextPageAction);
                subData.pBeforeSetSubpageData = &iLineId;
                subData.pAfterSetSubpageData = &m_iUIType;
                emit openpage(subData);
            }
        }

        return true;
    }

    return false;
}

bool CDlgCallControlLineSelect::IsStatusChanged()
{
    return false;
}

bool CDlgCallControlLineSelect::LoadPageData()
{
    return false;
}

bool CDlgCallControlLineSelect::SavePageData()
{
    return false;
}


CDlgCallControlLineSelect::~CDlgCallControlLineSelect()
{
}

// 设置子页面前的操作
void CDlgCallControlLineSelect::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        return ;
    }

    m_strNextPageAction = *((yl::string *)pData);
}

void CDlgCallControlLineSelect::AfterSetSubPage(void * pData)
{
    if (pData != NULL)
    {
        m_iUIType = *(int *)pData;
    }
}
