#include "cdlgdndcustom.h"
#include "account/include/modaccount.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "setting/src/dndcontroller.h"
#include "actiondefinecommon.h"
#include "uikernel/qcheckbox.h"
#include "baseui/t4xpicpath.h"

CDlgDNDCustom::CDlgDNDCustom(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bFN2IsAllOn(false)
    , m_bEnableAccount(false)
{
    m_strTitle = LANG_TRANSLATE(TRID_CUSTOM_DND).toUtf8().data();

    InitData();
}

CDlgDNDCustom::~CDlgDNDCustom()
{

}

void CDlgDNDCustom::InitData()
{
    QWidgetPair qPair;
    QString strLab;

    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetNote(LANG_TRANSLATE(TRID_NONE));
    }

    m_listAccount = acc_GetAvailableAccountList();
    ListAccounts::iterator iterAccount = m_listAccount.begin();
    for (; iterAccount != m_listAccount.end(); ++iterAccount)
    {
        //初始化
        QLabel * pLabel = new QLabel(this);
        pLabel->setText(LANG_TRANSLATE(TRID_OFF));

        //指针判空
        if (pLabel == NULL)
        {
            return;
        }

        //http://10.2.1.199/Bug.php?BugID=92700 改为和黑白屏一样统一通过acc_GetAccountShowText接口获取
        yl::string strAccountDisplayName = acc_GetAccountShowText(*iterAccount);

        //设置Activation标签
        strLab = toQString(strAccountDisplayName.c_str());
        qPair.first = strLab;
        qPair.second = pLabel;
        qPair.third = ACTION_ENTER;
        qPair.bWithStatus = true;
        qPair.eType = SETTING_ITEM_MENU;


        // 加入映射中
        m_VecAccountID.push_back(make_pair(*iterAccount, pLabel));
        m_vecWidgetPair.push_back(qPair);
        m_bEnableAccount = true;


    }
    //设置为None的提示必须在AddWidget之后
    AddWidget(m_vecWidgetPair, true, false, true, false);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

// 设置子页面的SoftKey
bool CDlgDNDCustom::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_bEnableAccount)
    {
        if (m_bFN2IsAllOn)
        {
            objWndData[1].m_strSoftkeyAction = ACTION_ALLON;
            objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ALL_ON);
            objWndData[1].m_strIconPath = PIC_SOFTKEY_ALL_ON;
        }
        else
        {
            objWndData[1].m_strSoftkeyAction = ACTION_ALLOFF;
            objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ALL_OFF);
            objWndData[1].m_strIconPath = PIC_SOFTKEY_ALL_OFF;
        }
    }
    else
    {
        objWndData[1].m_strSoftkeyAction = "";
        objWndData[1].m_strSoftkeyTitle = "";
    }
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

bool CDlgDNDCustom::LoadPageData()
{
    bool bFN2IsAllOn = false;

    VecAccountID::iterator iterAccount = m_VecAccountID.begin();
    for (; iterAccount != m_VecAccountID.end(); ++iterAccount)
    {
        // 根据ID从配置中读取状态
        DndData objData;
        bool bEnable = false;
        if (g_pDndController->GetDndData(&objData, iterAccount->first))
        {
            bEnable = objData.m_bEnabled;

            // 任意一个为Off，Softkey显示AllOn
            bFN2IsAllOn = bFN2IsAllOn || (!bEnable);
        }
        iterAccount->second->setText(bEnable ? LANG_TRANSLATE(TRID_ON) : LANG_TRANSLATE(TRID_OFF));
    }

    // 状态发生改变刷新Softkey
    if (bFN2IsAllOn != m_bFN2IsAllOn)
    {
        m_bFN2IsAllOn = bFN2IsAllOn;
    }

    return true;
}

bool CDlgDNDCustom::IsStatusChanged()
{
    return false;
}

bool CDlgDNDCustom::SavePageData()
{
    return false;
}

void CDlgDNDCustom::SetAllStatus(bool bStatus)
{
    VecAccountID::iterator iterAccount = m_VecAccountID.begin();
    bool bSuccess = true;
    for (; iterAccount != m_VecAccountID.end(); ++iterAccount)
    {
        // 根据ID保存状态
        DndData objData;
        if (g_pDndController->GetDndData(&objData, iterAccount->first))
        {
            objData.m_bEnabled = bStatus;
        }
        else
        {
            //读取原先配置不成功，则不保存，提示保存失败
            bSuccess = false;
            continue;
        }

        g_pDndController->SetDndData(objData, iterAccount->first);
    }

    if (!bSuccess)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVE_CONFIG_FAILED_PLEASE_RETRY),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
    }
}

bool CDlgDNDCustom::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

bool CDlgDNDCustom::OnAction(const QString & strAction)
{
    if (strAction == ACTION_ALLOFF)
    {
        for (int i = 0; i < m_VecAccountID.size(); ++i)
        {
            // 0 为关闭状态
            m_VecAccountID[i].second->setText(LANG_TRANSLATE(TRID_OFF));
        }

        SetAllStatus(false);
        m_bFN2IsAllOn = true;
        LoadPageData();
        emit RefreshSoftkey();
        return true;
    }
    else if (strAction == ACTION_ALLON)
    {
        for (int i = 0; i < m_VecAccountID.size(); ++i)
        {
            // 1 为开启状态
            m_VecAccountID[i].second->setText(LANG_TRANSLATE(TRID_ON));
        }

        SetAllStatus(true);
        m_bFN2IsAllOn = false;
        LoadPageData();
        emit RefreshSoftkey();
        return true;
    }
    else if (strAction == ACTION_ENTER)
    {
        if (m_pGeneralFrame != NULL
                && m_pGeneralFrame->GetFocusItemIndex() < m_VecAccountID.size())
        {
            int iAccountId = m_VecAccountID[m_pGeneralFrame->GetFocusItemDataIndex()].first;
            SubMenuData objData;
            objData.strMenuAction = "OpenSubPage(dnd)";
            objData.pBeforeSetSubpageData = (void *)&iAccountId;
            emit openpage(objData);
        }
        return true;
    }
    else
    {
        return false;
    }

    return true;
}


void CDlgDNDCustom::OnReShow()
{
    //从下面的子界面返回，需重新下载数据
    LoadPageData();
}
