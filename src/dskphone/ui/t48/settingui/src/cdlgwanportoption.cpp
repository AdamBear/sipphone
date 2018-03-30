#include "cdlgwanportoption.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uimanager/moduimanager.h"
#include "mainwnd/mainwnd.h"
#include "setting/src/networkcontroller.h"
#include "settingui/include/modsettingui.h"
#include "networksettingmanager.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"
#include "netmsgtypedefine.h"
enum IP_PREFERENCE
{
    Preference_IPv4 = 0,
    Preference_IPv6 = 1,
};

CDlgWanPortOption::CDlgWanPortOption(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxIPMode(NULL)
    , m_pCmbBoxPreference(NULL)
    , m_iIPMode(0)
    , m_iPreference(0)
{
    InitData();
}

CDlgWanPortOption::~CDlgWanPortOption()
{

}

void CDlgWanPortOption::InitData()
{
    //初始化
    m_pCmbBoxIPMode = new QComboBox(this);
    m_pCmbBoxPreference = new QComboBox(this);

    //指针判空
    if (m_pCmbBoxIPMode == NULL || m_pCmbBoxPreference == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxIPMode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxPreference, Qt::black);

    m_pCmbBoxIPMode->addItem(LANG_TRANSLATE(TRID_IPV4), QVariant(IPAM_IPV4));
    m_pCmbBoxIPMode->addItem(LANG_TRANSLATE(TRID_IPV6), QVariant(IPAM_IPV6));
    m_pCmbBoxIPMode->addItem(LANG_TRANSLATE(TRID_IPV4_AND_IPV6), QVariant(IPAM_BOTH));

    //设置Preference选择框内容
    m_pCmbBoxPreference->addItem(LANG_TRANSLATE(TRID_IPV4), QVariant(Preference_IPv4));
    m_pCmbBoxPreference->addItem(LANG_TRANSLATE(TRID_IPV6), QVariant(Preference_IPv6));

    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_IP_ADDRESS_MODE);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxIPMode;
    qPair.bChangeStyleOnPress = false;
    m_vecWidgetPair.push_back(qPair);


    m_pCmbBoxPreference->hide();




    strLab = LANG_TRANSLATE(TRID_IPV4);
    qPair.first = strLab;
    qPair.second = NULL;
    qPair.eType = SETTING_ITEM_MENU;
    qPair.bAddMark =  false;
    qPair.bChangeStyleOnPress = true;
    qPair.third = "enter";
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_IPV6);
    qPair.first = strLab;
    qPair.second = NULL;
    qPair.third = "enter";
    qPair.eType = SETTING_ITEM_MENU;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair, true, true, true, false);

    //安装事件过滤器
    m_pCmbBoxIPMode->installEventFilter(this);

    m_pCmbBoxPreference->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
    //添加IP Mode选择框变化消息
    connect(m_pCmbBoxIPMode, SIGNAL(currentIndexChanged(int)), this, SLOT(IsShowPreference(int)));
}

bool CDlgWanPortOption::OnAction(const QString & strAction)
{
    bool bResult = false;
    if (strAction == "enter")
    {
        int nIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        //当无选择时，GetFocusItemDataIndex()返回-1;
        if (nIndex != -1)
        {
            if (m_vecWidgetPair[nIndex].first == LANG_TRANSLATE(TRID_IPV4) || m_vecWidgetPair[nIndex].first == LANG_TRANSLATE(TRID_IPV6))
            {
                int nIPMode = m_vecWidgetPair[nIndex].first == LANG_TRANSLATE(TRID_IPV4) ? 0 : 1;
                SubMenuData objSubMenu;
                objSubMenu.strMenuAction = "OpenSubPage(network_wan_port)";
                objSubMenu.pBeforeSetSubpageData = (void *)&nIPMode;
                emit openpage(objSubMenu);
            }
        }
        bResult = true;
    }

    return bResult;
}

// 设置子页面的SoftKey
bool CDlgWanPortOption::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    return true;
}

bool CDlgWanPortOption::SavePageData()
{
    if (NULL != m_pCmbBoxIPMode && IsStatusChanged() && NULL != m_pCmbBoxPreference)
    {
        int nIPMode, nIPreference = 0;
        GetPageData(nIPMode, nIPreference);
        g_pNetworkController->SetWanIPMode(nIPMode, nIPreference);
        //网络设置有改变并进行了保存，设置网络改变的标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

bool CDlgWanPortOption::GetPageData(int & nIPMode, int & nIPreference)
{
    if (m_pCmbBoxIPMode == NULL || m_pCmbBoxPreference == NULL)
    {
        return false;
    }

    //从页面获取开关值
    nIPMode = m_pCmbBoxIPMode
              ->itemData(m_pCmbBoxIPMode->currentIndex(), Qt::UserRole).toInt();
    nIPreference = m_pCmbBoxPreference->currentIndex();

    return true;
}

bool CDlgWanPortOption::LoadPageData()
{
    m_iIPMode = g_pNetworkController->GetWanIPMode();
    m_iPreference = configParser_GetConfigInt(kszNetWorkPreference);
    if (NULL != m_pCmbBoxIPMode && NULL != m_pCmbBoxPreference)
    {
        m_pCmbBoxIPMode->setCurrentIndex(m_pCmbBoxIPMode->findData(m_iIPMode, Qt::UserRole));
        m_pCmbBoxPreference->setCurrentIndex(m_iPreference);
    }

    return true;
}

// 对输入框的数据进行验证
bool CDlgWanPortOption::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgWanPortOption::IsStatusChanged()
{
    int nIPMode, nIPreference;
    GetPageData(nIPMode, nIPreference);
    //仅当IPMode为both时，Preference改变才算状态变化
    if (nIPMode != m_iIPMode)
    {
        return true;
    }
    else if (nIPMode == IPAM_BOTH)
    {
        return (nIPreference != m_iPreference);
    }

    return false;
}

//IP Mode选择框选择变化槽函数
void CDlgWanPortOption::IsShowPreference(int nIndex)
{
    bool bHasPreference = false;

    QWPVector::iterator it = m_vecWidgetPair.begin();
    for (; it != m_vecWidgetPair.end(); ++it)
    {
        if (it->first == LANG_TRANSLATE(TRID_PREFERENCE))
        {
            bHasPreference = true;
            break;
        }
    }
    if (nIndex == IPAM_BOTH)
    {
        if (!bHasPreference)
        {
            int nIPMOde = 0;
            for (it = m_vecWidgetPair.begin(); it != m_vecWidgetPair.end(); ++it)
            {

                if (it->first == LANG_TRANSLATE(TRID_IP_ADDRESS_MODE))
                {
                    break;
                }
                nIPMOde++;
            }
            QWidgetPair qPair;
            qPair.first = LANG_TRANSLATE(TRID_PREFERENCE);
            qPair.second = m_pCmbBoxPreference;
            m_vecWidgetPair.insert(nIPMOde + 1, qPair);
            AddWidget(m_vecWidgetPair);
            UpdateWnd();
        }
    }
    else if (bHasPreference)
    {
        m_vecWidgetPair.erase(it);
        AddWidget(m_vecWidgetPair);
        UpdateWnd();

    }

}
