#include "cdlgcustomforward.h"
#include "account/include/modaccount.h"
#include "settingui/include/modsettingui.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "keymap.h"
#include "baseui/fontcolor.h"
#include "setting/src/callforwardcontroller.h"

CDlgCustomForward::CDlgCustomForward(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bEnableAccount(false)
{
    m_strTitle = LANG_TRANSLATE(TRID_CUSTOM_FORWARD).toUtf8().data();

    m_listForwardData.clear();
    InitData();
}

CDlgCustomForward::~CDlgCustomForward()
{
}

void CDlgCustomForward::InitData()
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetNote(LANG_TRANSLATE(TRID_NONE));
    }

    QWidgetPair qPair;
    QString strLab;

    //获取可用账号数据
    ListAccounts listValidAccount = acc_GetAvailableAccountList();
    int nIndex = 0;
    ListAccounts::iterator itAccount = listValidAccount.begin();
    for (/* */; itAccount != listValidAccount.end(); ++itAccount)
    {
        QLabel * pLabel = new QLabel(this);
        if (NULL != pLabel)
        {
            // 设置字体颜色
            SetWgtFontColor(pLabel, FONT_TYPE_TEXT);

            //http://10.2.1.199/Bug.php?BugID=92700 改为和黑白屏一样统一通过acc_GetAccountShowText接口获取
            yl::string strAccountDisplayName = acc_GetAccountShowText(*itAccount);

            //设置Activation标签
            strLab = toQString(strAccountDisplayName.c_str());
            qPair.first = strLab;
            qPair.second = pLabel;
            qPair.third = "enter";
            qPair.bWithStatus = true;
            qPair.eType = SETTING_ITEM_MENU;
            // 加入映射中

            CustomForwardData ItemData;
            ItemData.iAccountID = *itAccount;
            ItemData.pLabel = pLabel;
            ItemData.nIndex = nIndex;
            m_listForwardData.push_back(ItemData);
            m_vecWidgetPair.push_back(qPair);
            nIndex++;
        }
    }

#ifdef IF_FEATURE_FORWARD_OPT
    if (listValidAccount.size() > 1)
    {
        QLabel * pLabel = new QLabel(this);
        if (NULL != pLabel)
        {
            // 设置字体颜色
            SetWgtFontColor(pLabel, FONT_TYPE_TEXT);

            //设置Activation标签
            strLab = LANG_TRANSLATE(TRID_ALL_LINES);
            qPair.first = strLab;
            qPair.second = pLabel;
            qPair.third = "enter";
            qPair.bWithStatus = true;
            qPair.eType = SETTING_ITEM_MENU;

            CustomForwardData ItemData;
            ItemData.iAccountID = -1;
            ItemData.pLabel = pLabel;
            ItemData.nIndex = nIndex++;
            m_listForwardData.push_back(ItemData);
            m_vecWidgetPair.push_back(qPair);
        }
    }
#endif

    //设置为None的提示必须在AddWidget之后
    AddWidget(m_vecWidgetPair, true, false, true, false);

    // 无账号可用
    m_bEnableAccount = (listValidAccount.size() != 0);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

// 设置子页面的SoftKey
bool CDlgCustomForward::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

// 对输入框的数据进行验证
bool CDlgCustomForward::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

//从V层读取当前界面的配置，通过C层保存
bool CDlgCustomForward::SavePageData()
{
    return false;
}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgCustomForward::LoadPageData()
{
    int nAccountID;
    for (nAccountID = 0; nAccountID < acc_AccountNum(); nAccountID++)
    {
        IterForwardData itrBegin = m_listForwardData.begin();
        for (; itrBegin != m_listForwardData.end(); ++itrBegin)
        {
            CustomForwardData & ItemData = *itrBegin;
            if (ItemData.iAccountID == nAccountID)
            {
                // 从配置中读值
                QString strText = g_pCallForwardController->IsForwardEnable(FWD_TYPE_ALL, ItemData.iAccountID)
                                  ? LANG_TRANSLATE(TRID_ON) : LANG_TRANSLATE(TRID_OFF);

                ItemData.pLabel->setText(strText);
            }
        }
    }
    //给自己安装过滤器
    this->installEventFilter(this);
    return true;
}

//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgCustomForward::IsStatusChanged()
{
    return false;
}

bool CDlgCustomForward::OnAction(const QString & strAction)
{
    bool bResult = false;

    if (strAction == "enter")
    {
        //根据账号进入，显示
        IterForwardData itrBegin = m_listForwardData.begin();
        for (; itrBegin != m_listForwardData.end(); ++itrBegin)
        {
            if (itrBegin->nIndex == m_pGeneralFrame->GetFocusItemDataIndex())
            {
                SubMenuData subMenuData;
                subMenuData.iFirstSelectItem = 0;
                subMenuData.pBeforeSetSubpageData = (void *) & (itrBegin->iAccountID);
                subMenuData.strMenuAction = "OpenSubPage(call_forward_option)";
                emit openpage(subMenuData);
                break;
            }
        }

        bResult = true;
    }

    return bResult;
}

void CDlgCustomForward::showEvent(QShowEvent * e)
{
    //从CDlgCustomForwardOption子界面返回，需重新下载数据
    LoadPageData();
}

