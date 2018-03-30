#include "model.h"
#include "cdlgnetwork_vlan.h"

#include "setting/src/networkcontroller.h"

#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "messagebox/modmessagebox.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"

#include "devicelib/phonedevice.h"
#include "networksettingmanager.h"

#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())

//页面类型
enum ePageType
{
    WANPORT = 0,
    PCPORT
};

CDlgNetwork_VLAN::CDlgNetwork_VLAN(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxVLAN(NULL)
    , m_pEditVID_Number_WAN(NULL)
    , m_pEditPriority_WAN(NULL)
    , m_iPageType(1989)
{
    InitData();
}

CDlgNetwork_VLAN::~CDlgNetwork_VLAN()
{
}


void CDlgNetwork_VLAN::InitData()
{
    m_qCheckBoxVLAN = new qCheckBox(this);
    m_pEditVID_Number_WAN = new QLineEdit(this);
    m_pEditPriority_WAN = new QLineEdit(this);

    if (NULL == m_qCheckBoxVLAN
            || NULL == m_pEditVID_Number_WAN
            || NULL == m_pEditPriority_WAN)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditVID_Number_WAN, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditPriority_WAN, Qt::black);

    //安装输入法
    qInstallIME(m_pEditPriority_WAN, IME_123);
    qInstallIME(m_pEditVID_Number_WAN, IME_123);

    //设置最大输入长度
    m_pEditVID_Number_WAN->setMaxLength(4);
    m_pEditPriority_WAN->setMaxLength(1);

    //为界面上各输入控件安装事件过滤器
    m_qCheckBoxVLAN->installEventFilter(this);
    m_pEditVID_Number_WAN->installEventFilter(this);
    m_pEditPriority_WAN->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_VLAN_STATUS);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxVLAN;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_VID_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditVID_Number_WAN;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_RRIORITY);
    qPair.first = strLab;
    qPair.second = m_pEditPriority_WAN;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetwork_VLAN::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditVID_Number_WAN, EDIT_TYPE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPriority_WAN, EDIT_TYPE_NUMBER);

    if (m_iPageType == WANPORT)
    {
        m_strTitle = LANG_TRANSLATE(TRID_WAN_PORT);
    }
    else if (m_iPageType == PCPORT)
    {
        m_strTitle = LANG_TRANSLATE(TRID_PC_PORT);
    }

    //定义当前页面数据
    NetworkVlanData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVlanData(&currentPageData);

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
bool CDlgNetwork_VLAN::IsStatusChanged()
{
    NetworkVlanData currentPageData;
    GetCurrentPageData(currentPageData);

    NetworkVlanData beforePageData;
    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetVlanData(&beforePageData);

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

bool CDlgNetwork_VLAN::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_qCheckBoxVLAN != NULL)
    {
        //数字判断
        //vid number不能为空
        if (m_pEditVID_Number_WAN->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditVID_Number_WAN->text(), EDIT_TYPE_NUMBER))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_VID_NUMBER_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditVID_Number_WAN;
            return false;
        }

        if (m_pEditPriority_WAN->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditPriority_WAN->text(), EDIT_TYPE_NUMBER))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_RRIORITY_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditPriority_WAN;
            return false;
        }

        if (m_pEditVID_Number_WAN->text().toInt() < 1
                || m_pEditVID_Number_WAN->text().toInt() > 4094)
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_VID_NUMBER_SHOULD_BE_1_TO_4094);
            objErrorContent.pLineEdit = m_pEditVID_Number_WAN;
            return false;
        }
        if (m_pEditPriority_WAN->text().toInt() < 0
                || m_pEditPriority_WAN->text().toInt() > 7)
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_RRIORITY_SHOULD_BE_0_TO_7);
            objErrorContent.pLineEdit = m_pEditPriority_WAN;
            return false;
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgNetwork_VLAN::SavePageData()
{
    //提起页面数据
    if (IsStatusChanged())
    {
        //取得当前页面数据
        NetworkVlanData currentPageData;
        GetCurrentPageData(currentPageData);

        //提示框内容
        QString strContent;

        NetworkWanTypeData wanTypeData;

        //调用控制层：加载数据
        bool bIsSuccess =
            doNetworkControl->GetWanTypeData(&wanTypeData);

        if (bIsSuccess)
        {
#if IF_FEATURE_PPPOE
            //WAN Port方式为PPPoE,开启WAN或VLAN Port VLAN,弹窗提示
            if (wanTypeData.m_eType == WAN_PPPOE
                    && (currentPageData.m_isPcEnable || currentPageData.m_isWanEnable))
            {
                strContent = LANG_TRANSLATE(TRID_VLAN_UNSURPPORTED_IN_PPPOE_MODE);
                MessageBox_ShowCommonMsgBox(NULL, strContent, MESSAGEBOX_NOTE, MESSAGE_BOX_EXIST_TIME);
                return false;
            }
#endif
        }
        else
        {
            //获取信息失败，返回false
            return false;
        }

        //调用控制层：保存数据
        doNetworkControl->SetVlanData(currentPageData);

        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

//比较页面前后数据
bool CDlgNetwork_VLAN::ComparePageData(const NetworkVlanData & beforePageData,
                                       const NetworkVlanData & afterPageData) const
{
    if (m_iPageType == WANPORT)
    {
        if (beforePageData.m_isWanEnable != afterPageData.m_isWanEnable
                || beforePageData.m_nWanPriority != afterPageData.m_nWanPriority
                || beforePageData.m_nWanVid != afterPageData.m_nWanVid)
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
    else
    {
        if (beforePageData.m_isPcEnable != afterPageData.m_isPcEnable
                || beforePageData.m_nPcPriority != afterPageData.m_nPcPriority
                || beforePageData.m_nPcVid != afterPageData.m_nPcVid)
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

}

//取得当前页面数据
NetworkVlanData & CDlgNetwork_VLAN::GetCurrentPageData(NetworkVlanData & currentPageData) const
{
    if (m_qCheckBoxVLAN != NULL)
    {
        //调用控制层：加载原先的数据
        doNetworkControl->GetVlanData(&currentPageData);
        if (m_iPageType == WANPORT)
        {
            currentPageData.m_isWanEnable = m_qCheckBoxVLAN->GetData();
            currentPageData.m_nWanVid =
                m_pEditVID_Number_WAN->text().trimmed().toInt();
            currentPageData.m_nWanPriority =
                m_pEditPriority_WAN->text().trimmed().toInt();
        }
        else
        {
            currentPageData.m_isPcEnable = m_qCheckBoxVLAN->GetData();
            currentPageData.m_nPcVid =
                m_pEditVID_Number_WAN->text().trimmed().toInt();
            currentPageData.m_nPcPriority =
                m_pEditPriority_WAN->text().trimmed().toInt();
        }
    }


    return currentPageData;
}

//设置当前页面数据
void CDlgNetwork_VLAN::SetCurrentPageData(NetworkVlanData & currentPageData)
{
    //指针判空
    if (m_qCheckBoxVLAN == NULL)
    {
        return;
    }

    if (m_iPageType == WANPORT)
    {
        m_qCheckBoxVLAN->SetData(currentPageData.m_isWanEnable);
        m_pEditVID_Number_WAN->setText(QString::number(currentPageData.m_nWanVid));
        m_pEditPriority_WAN->setText(QString::number(currentPageData.m_nWanPriority));
    }
    else
    {
        m_qCheckBoxVLAN->SetData(currentPageData.m_isPcEnable);
        m_pEditVID_Number_WAN->setText(QString::number(currentPageData.m_nPcVid));
        m_pEditPriority_WAN->setText(QString::number(currentPageData.m_nPcPriority));
    }
}

// 设置子页面前的操作
void CDlgNetwork_VLAN::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_iPageType = *((int *)pData);
}
