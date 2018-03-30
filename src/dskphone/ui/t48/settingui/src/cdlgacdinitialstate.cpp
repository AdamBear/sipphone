/************************************************************************
 * FileName  : CDlgACDInitialState.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/22 11:56:36
 ************************************************************************/

#include "cdlgacdinitialstate.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcomboboxutility.h"

CDlgACDInitialState::CDlgACDInitialState(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pInitialCmb(NULL)
{
    InitData();
}

CDlgACDInitialState::~CDlgACDInitialState()
{

}

// 读取页面数据
bool CDlgACDInitialState::LoadPageData()
{
    if (NULL != m_pInitialCmb)
    {
        ACD_STATUS eStat = ACD_GetInitStatus();

        if (eStat != AS_AVAILABLE
                && eStat != AS_UNAVAILABLE
                && eStat != AS_WRAPUP)  // 如果为AS_WRAPUP则显示为空
        {
            eStat = AS_UNAVAILABLE;
        }

        m_pInitialCmb->setCurrentIndex(m_pInitialCmb
                                       ->findData(int(eStat), Qt::UserRole));
    }
    return true;
}

// 查看页面状态是否改变
bool CDlgACDInitialState::IsStatusChanged()
{
    ACD_STATUS eUserStat = ACD_STATUS(m_pInitialCmb
                                      ->itemData(m_pInitialCmb->currentIndex(), Qt::UserRole).toInt());

    ACD_STATUS eStat = ACD_GetInitStatus();

    if (eStat != AS_AVAILABLE
            && eStat != AS_UNAVAILABLE
            && eStat != AS_WRAPUP)  // 如果为AS_WRAPUP则显示为空
    {
        //查清楚应该是AS_AVAILABLE还是AS_UNAVAILABLE
        eStat = AS_UNAVAILABLE;
    }

    return (eUserStat != eStat);
}

// 保存页面数据
bool CDlgACDInitialState::SavePageData()
{
    if (NULL == m_pInitialCmb)
    {
        return false;
    }

    ACD_STATUS eAcdStat = ACD_STATUS(m_pInitialCmb
                                     ->itemData(m_pInitialCmb->currentIndex(), Qt::UserRole).toInt());
    ACD_SetInitStatus(eAcdStat);

    return true;
}

void CDlgACDInitialState::SetData(void * pData /* = NULL */)
{

}

bool CDlgACDInitialState::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{

    return true;
}

void CDlgACDInitialState::InitData()
{
    m_strTitle = LANG_TRANSLATE(TRID_INITAIL_STATE).toUtf8().data();

    m_pInitialCmb    = new QComboBox(this);

    //指针判空
    if (m_pInitialCmb == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pInitialCmb, Qt::black);

    m_pInitialCmb->addItem(LANG_TRANSLATE(TRID_AVAILABLE), int(AS_AVAILABLE));
    m_pInitialCmb->addItem(LANG_TRANSLATE(TRID_UNAVAILABLE), int(AS_UNAVAILABLE));

    //设置Activation标签
    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(TRID_LOGIN_AS);
    qPair.second = m_pInitialCmb;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pInitialCmb->installEventFilter(this);
}
