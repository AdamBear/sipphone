#include "model.h"
#include "cdlgfwdinternational.h"

#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include "networksettingmanager.h"
#include "setting/src/fwdinternationalcontroller.h"
#include "uikernel/qcomboboxutility.h"

CDlgFWDInternational::CDlgFWDInternational(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxFWDInternational_Active(NULL)
{
    InitData();
}

CDlgFWDInternational::~CDlgFWDInternational()
{
}


void CDlgFWDInternational::InitData()
{
    //设定滑块大小位置
    m_pCmbBoxFWDInternational_Active = new QComboBox(this);
    if (m_pCmbBoxFWDInternational_Active == NULL)
    {
        //内存已满立即返回
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxFWDInternational_Active, Qt::black);
    m_pCmbBoxFWDInternational_Active->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxFWDInternational_Active->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    m_pCmbBoxFWDInternational_Active->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_INTERNATIONAL_FORWARDING);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxFWDInternational_Active;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgFWDInternational::LoadPageData()
{
    this->setFocus();
    FWDInternationalData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        g_pFWDInternationalController->GetFWDInternationalData(&currentPageData);

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
bool CDlgFWDInternational::IsStatusChanged()
{
    FWDInternationalData currentPageData;
    FWDInternationalData beforePageData;

    GetCurrentPageData(currentPageData);

    //调用控制层：加载数据
    bool bIsSuccess =
        g_pFWDInternationalController->GetFWDInternationalData(&beforePageData);

    if (bIsSuccess)
    {
        //比较前后数据是否一致
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

bool CDlgFWDInternational::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgFWDInternational::SavePageData()
{
    //提起页面数据
    if (IsStatusChanged())
    {
        FWDInternationalData currentPageData;
        GetCurrentPageData(currentPageData);

        //调用控制层：保存数据
        g_pFWDInternationalController->SetFWDInternationalData(currentPageData);
    }
    return true;
}

//比较页面前后数据
bool CDlgFWDInternational::ComparePageData(const FWDInternationalData & beforePageData,
        const FWDInternationalData & afterPageData) const
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
FWDInternationalData & CDlgFWDInternational::GetCurrentPageData(FWDInternationalData &
        currentPageData) const
{
    currentPageData.m_isEnable = m_pCmbBoxFWDInternational_Active
                                 ->itemData(m_pCmbBoxFWDInternational_Active->currentIndex(), Qt::UserRole).value<bool>();
    return currentPageData;
}

//设置当前页面数据
void CDlgFWDInternational::SetCurrentPageData(FWDInternationalData & currentPageData)
{
    m_pCmbBoxFWDInternational_Active->setCurrentIndex(m_pCmbBoxFWDInternational_Active
            ->findData(currentPageData.m_isEnable, Qt::UserRole));
}

