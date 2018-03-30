#include "cdlgzeronetwork_vlan.h"

#include "setting/src/networkcontroller.h"
#include "setting/include/common.h"
#include "settingui/src/networksettingmanager.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "messagebox/modmessagebox.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "devicelib/phonedevice.h"
#include <QtCore/QVariant>

#include "dsklog/log.h"

#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())
namespace
{
#define MAX_VID_NUMBER_LENGTH       4
#define MAX_PRIORITY_LENGTH         1
}


CDlgZeroNetwork_VLAN::CDlgZeroNetwork_VLAN(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxWAN_Port_VLAN(NULL)
    , m_pEditVID_Number_WAN(NULL)
    , m_pEditPriority_WAN(NULL)
{
    InitData();
}

CDlgZeroNetwork_VLAN::~CDlgZeroNetwork_VLAN()
{
    qUninstallIME(m_pEditPriority_WAN);
    qUninstallIME(m_pEditVID_Number_WAN);
    m_vecWidgetPair.clear();
}


void CDlgZeroNetwork_VLAN::InitData()
{
    m_pCheckBoxWAN_Port_VLAN = new qCheckBox(this);
    m_pEditVID_Number_WAN = new QLineEdit(this);
    m_pEditPriority_WAN = new QLineEdit(this);

    if (JudgetWgtNull())
    {
        return;
    }

    SetLineEditDefaultStyle(m_pEditVID_Number_WAN);
    SetLineEditDefaultStyle(m_pEditPriority_WAN);

    //安装输入法
    qInstallIME(m_pEditPriority_WAN, IME_123);
    qInstallIME(m_pEditVID_Number_WAN, IME_123);

    //设置最大输入长度
    m_pEditVID_Number_WAN->setMaxLength(MAX_VID_NUMBER_LENGTH);
    m_pEditPriority_WAN->setMaxLength(MAX_PRIORITY_LENGTH);

    //为界面上各输入控件安装事件过滤器
    m_pCheckBoxWAN_Port_VLAN->installEventFilter(this);
    m_pEditVID_Number_WAN->installEventFilter(this);
    m_pEditPriority_WAN->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);

    connect(m_pCheckBoxWAN_Port_VLAN, SIGNAL(checkChanged(bool)), this,
            SLOT(OnChkBoxEnableVLANChanged(bool)));
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgZeroNetwork_VLAN::LoadPageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }

    this->blockSignals(true);
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditVID_Number_WAN, EDIT_TYPE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPriority_WAN, EDIT_TYPE_NUMBER);

    //定义当前页面数据
    NetworkVlanData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->GetVlanData(&currentPageData);

    if (bIsSuccess)
    {
        SetCurrentPageData(currentPageData);
        this->blockSignals(false);
        return true;
    }
    else
    {
        this->blockSignals(false);
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgZeroNetwork_VLAN::IsStatusChanged()
{
    NetworkVlanData beforePageData;
    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVlanData(&beforePageData);

    if (beforePageData.m_isWanEnable
            && (m_pEditPriority_WAN->text().trimmed().isEmpty()
                || m_pEditVID_Number_WAN->text().trimmed().isEmpty()))
    {
        return true;
    }

    NetworkVlanData currentPageData;
    GetCurrentPageData(currentPageData);

    if (bIsSuccess)
    {
        if (ComparePageData(beforePageData, currentPageData))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool CDlgZeroNetwork_VLAN::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (!JudgetWgtNull())
    {
        bool bEnable = m_pCheckBoxWAN_Port_VLAN->GetData();
        if (!bEnable)
        {
            return true;
        }
        //数字判断
        //vid number不能为空
        if (m_pEditVID_Number_WAN->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditVID_Number_WAN->text(), EDIT_TYPE_NUMBER))
        {
            objErrorContent.strErrorContent = TRID_VID_NUMBER_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditVID_Number_WAN;
            return false;
        }

        if (m_pEditPriority_WAN->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditPriority_WAN->text(), EDIT_TYPE_NUMBER))
        {
            objErrorContent.strErrorContent = TRID_RRIORITY_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditPriority_WAN;
            return false;
        }

        if (m_pEditVID_Number_WAN->text().toInt() < 1
                || m_pEditVID_Number_WAN->text().toInt() > 4094)
        {
            objErrorContent.strErrorContent = TRID_VID_NUMBER_SHOULD_BE_1_TO_4094;
            objErrorContent.pLineEdit = m_pEditVID_Number_WAN;
            return false;
        }
        if (m_pEditPriority_WAN->text().toInt() < 0
                || m_pEditPriority_WAN->text().toInt() > 7)
        {
            objErrorContent.strErrorContent = TRID_RRIORITY_SHOULD_BE_0_TO_7;
            objErrorContent.pLineEdit = m_pEditPriority_WAN;
            return false;
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgZeroNetwork_VLAN::SavePageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }

    if (IsStatusChanged())
    {
        //取得当前页面数据
        NetworkVlanData currentPageData;
        GetCurrentPageData(currentPageData);
        // 开关关闭不保存数据
        if (!currentPageData.m_isWanEnable)
        {
            NetworkVlanData logicPageData;
            //调用控制层：加载数据
            bool bIsSuccess =
                doNetworkControl->GetVlanData(&logicPageData);
            logicPageData.m_isWanEnable = currentPageData.m_isWanEnable;

            doNetworkControl->SetVlanData(logicPageData);
        }
        else
        {
            //提示框内容和标题
            QString strContent;
            QString strTitle;

            NetworkWanTypeData wanTypeData;
            NetworkWanPppoeData wanPppoeData;

            //调用控制层：加载数据
            bool bIsSuccess = doNetworkControl->GetWanTypeData(&wanTypeData);

            if (bIsSuccess)
            {
                //WAN Port方式为PPPoE,开启WAN或VLAN Port VLAN,弹窗提示
                if (WAN_PPPOE == wanTypeData.m_eType
                        && (currentPageData.m_isPcEnable || currentPageData.m_isWanEnable))
                {
                    strContent = LANG_TRANSLATE(TRID_VLAN_UNSURPPORTED_IN_PPPOE_MODE);
                    strTitle = LANG_TRANSLATE(TRID_NOTE);
                    MessageBox_ShowCommonMsgBox(NULL, strContent, MESSAGEBOX_NOTE, MESSAGE_BOX_EXIST_TIME);
                    return false;
                }
            }
            else
            {
                //获取信息失败，返回false
                return false;
            }

            //调用控制层：保存数据
            doNetworkControl->SetVlanData(currentPageData);
        }

        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

void CDlgZeroNetwork_VLAN::OnChkBoxEnableVLANChanged(bool bOn)
{
    m_pEditVID_Number_WAN->hide();
    m_pEditPriority_WAN->hide();

    QString strLab = "";
    QWidgetPair qPair;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_VLAN_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxWAN_Port_VLAN;
    m_vecWidgetPair.push_back(qPair);

    bool bEnable = m_pCheckBoxWAN_Port_VLAN->GetData();
    if (bEnable)
    {
        strLab = LANG_TRANSLATE(TRID_VID_NUMBER);
        qPair.first = strLab;
        qPair.second = m_pEditVID_Number_WAN;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_RRIORITY);
        qPair.first = strLab;
        qPair.second = m_pEditPriority_WAN;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        //与T3X的Zero保持一致
        if (PAGE_MODE_ZERO == m_ePageMode)
        {
            m_pEditVID_Number_WAN->setText("1");
            m_pEditPriority_WAN->setText("0");
        }
    }

    emit ReLayout(this);
}

//比较页面前后数据
bool CDlgZeroNetwork_VLAN::ComparePageData(const NetworkVlanData & beforePageData,
        const NetworkVlanData & afterPageData) const
{
    if (beforePageData.m_isWanEnable != afterPageData.m_isWanEnable
            || (afterPageData.m_isWanEnable && beforePageData.m_nWanPriority != afterPageData.m_nWanPriority)
            || (afterPageData.m_isWanEnable && beforePageData.m_nWanVid != afterPageData.m_nWanVid))
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
NetworkVlanData & CDlgZeroNetwork_VLAN::GetCurrentPageData(NetworkVlanData & currentPageData) const
{
    currentPageData.m_isWanEnable = m_pCheckBoxWAN_Port_VLAN->GetData();
    currentPageData.m_nWanVid =
        m_pEditVID_Number_WAN->text().trimmed().toInt();
    currentPageData.m_nWanPriority =
        m_pEditPriority_WAN->text().trimmed().toInt();

    return currentPageData;
}

//设置当前页面数据
void CDlgZeroNetwork_VLAN::SetCurrentPageData(NetworkVlanData & currentPageData)
{
    m_pCheckBoxWAN_Port_VLAN->SetData(currentPageData.m_isWanEnable);
    m_pEditVID_Number_WAN->setText(QString::number(currentPageData.m_nWanVid));
    m_pEditPriority_WAN->setText(QString::number(currentPageData.m_nWanPriority));
}

// 控件判空
bool CDlgZeroNetwork_VLAN::JudgetWgtNull()
{
    if (NULL == m_pCheckBoxWAN_Port_VLAN
            || NULL == m_pEditPriority_WAN
            || NULL == m_pEditVID_Number_WAN)
    {
        return true;
    }
    else
    {
        return false;
    }
}
