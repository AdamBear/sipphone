#include "cdlgzeronetwork_vpn.h"

#include "setting/src/networkcontroller.h"
#include "settingui/src/networksettingmanager.h"

#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcheckbox.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())


CDlgZeroNetwork_VPN::CDlgZeroNetwork_VPN(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxVPN_Active(NULL)
{
    InitData();
}

CDlgZeroNetwork_VPN::~CDlgZeroNetwork_VPN()
{
    m_vecWidgetPair.clear();
}


void CDlgZeroNetwork_VPN::InitData()
{
    m_qCheckBoxVPN_Active = new qCheckBox(this);
    if (NULL == m_qCheckBoxVPN_Active)
    {
        return;
    }

    m_qCheckBoxVPN_Active->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_VPN_ACTIVE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxVPN_Active;
    m_vecWidgetPair.push_back(qPair);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgZeroNetwork_VPN::LoadPageData()
{
    NetworkVpnData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVpnData(&currentPageData);

    if (bIsSuccess)
    {
        SetCurrentPageData(currentPageData);
        return true;
    }
    else
    {
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgZeroNetwork_VPN::IsStatusChanged()
{
    NetworkVpnData currentPageData;
    NetworkVpnData beforePageData;

    GetCurrentPageData(currentPageData);

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVpnData(&beforePageData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        if (ComparePageData(beforePageData, currentPageData))
        {
            //前后页面数据没有改变，返回false
            return false;
        }
        else
        {
            //前后页面数据改变，返回true
            return true;
        }
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}

bool CDlgZeroNetwork_VPN::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL != m_qCheckBoxVPN_Active)
    {
        //文件vpn.cnf存在时，才能打开开关
        if (m_qCheckBoxVPN_Active->GetData())
        {
            if (!doNetworkControl->IsOpenVPNConfigExist())
            {
                objErrorContent.strErrorContent = TRID_VPN_CANNOT_BE_ON_WHEN_FILE_DO_NOT_EXIST;
                return false;
            }
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgZeroNetwork_VPN::SavePageData()
{
    if (IsStatusChanged())
    {
        NetworkVpnData currentPageData;
        GetCurrentPageData(currentPageData);

        //调用控制层：保存数据
        doNetworkControl->SetVpnData(currentPageData);

        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

//比较页面前后数据
bool CDlgZeroNetwork_VPN::ComparePageData(const NetworkVpnData & beforePageData,
        const NetworkVpnData & afterPageData) const
{
    if (beforePageData.m_isEnable != afterPageData.m_isEnable)
    {
        //如果不相等那么返回flase
        return false;
    }
    else
    {
        //相等返回true
        return true;
    }
}

//取得当前页面数据
NetworkVpnData & CDlgZeroNetwork_VPN::GetCurrentPageData(NetworkVpnData & currentPageData) const
{
    if (NULL != m_qCheckBoxVPN_Active)
    {
        currentPageData.m_isEnable = m_qCheckBoxVPN_Active->GetData();
    }
    return currentPageData;
}

//设置当前页面数据
void CDlgZeroNetwork_VPN::SetCurrentPageData(NetworkVpnData & currentPageData)
{
    if (NULL != m_qCheckBoxVPN_Active)
    {
        m_qCheckBoxVPN_Active->SetData(currentPageData.m_isEnable);
    }
}

