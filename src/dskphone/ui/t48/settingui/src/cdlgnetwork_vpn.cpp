#include "model.h"
#include "cdlgnetwork_vpn.h"

#include "setting/src/networkcontroller.h"

#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include "networksettingmanager.h"
#include "uikernel/qcheckbox.h"

#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())

CDlgNetwork_VPN::CDlgNetwork_VPN(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxVPN_Active(NULL)
{
    InitData();
}

CDlgNetwork_VPN::~CDlgNetwork_VPN()
{
}


void CDlgNetwork_VPN::InitData()
{
    //设定滑块大小位置
    m_qCheckBoxVPN_Active = new qCheckBox(this);
    if (m_qCheckBoxVPN_Active == NULL)
    {
        //内存已满立即返回
        return;
    }

    m_qCheckBoxVPN_Active->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_VPN_ACTIVE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxVPN_Active;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetwork_VPN::LoadPageData()
{
    this->setFocus();
    NetworkVpnData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVpnData(&currentPageData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(currentPageData);
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgNetwork_VPN::IsStatusChanged()
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

bool CDlgNetwork_VPN::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_qCheckBoxVPN_Active != NULL)
    {
        //文件vpn.cnf存在时，才能打开开关
        if (m_qCheckBoxVPN_Active->GetData())
        {
            if (!doNetworkControl->IsOpenVPNConfigExist())
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_VPN_CANNOT_BE_ON_WHEN_FILE_DO_NOT_EXIST);
                return false;
            }
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgNetwork_VPN::SavePageData()
{
    //提起页面数据
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
bool CDlgNetwork_VPN::ComparePageData(const NetworkVpnData & beforePageData,
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
NetworkVpnData & CDlgNetwork_VPN::GetCurrentPageData(NetworkVpnData & currentPageData) const
{
    if (NULL != m_qCheckBoxVPN_Active)
    {
        currentPageData.m_isEnable = m_qCheckBoxVPN_Active->GetData();
    }
    return currentPageData;
}

//设置当前页面数据
void CDlgNetwork_VPN::SetCurrentPageData(NetworkVpnData & currentPageData)
{
    if (NULL != m_qCheckBoxVPN_Active)
    {
        m_qCheckBoxVPN_Active->SetData(currentPageData.m_isEnable);
    }
}

