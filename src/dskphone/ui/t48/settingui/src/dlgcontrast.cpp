#include "model.h"
#include "dlgcontrast.h"
#include <dsklog/log.h>
#include <interfacedefine.h>
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "devicelib/phonedevice.h"
#include "networksettingmanager.h"
#include "setting/src/contrastcontroller.h"
#include "exp/include/modexp.h"
#include "uikernel/qcomboboxutility.h"

CDlgContrast::CDlgContrast(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxContrast_level(NULL)
    , m_bHaveEXP(false)
{
    InitData();
}

CDlgContrast::~CDlgContrast()
{
    if (m_bHaveEXP)
    {
        //页面销毁前设置再设置为配置的值，防止未保存时对比度改变了
        int iCurrentLevel;
        iCurrentLevel = g_pContrastController->GetCurrentContrastLevel();
        int iIndex = m_pCmbBoxContrast_level
                     ->findData(iCurrentLevel, Qt::UserRole);
        OnCmbBoxIndexChange(iIndex);
    }
}


void CDlgContrast::InitData()
{
    QWidgetPair qPair;
    QString strLab;
    int iExtCount = exp_GetEXPNum();
    if (iExtCount == 0)
    {
        strLab = LANG_TRANSLATE(TRID_NO_EXPASION_MODULE_CONNECTED);
        qPair.first = strLab;
        qPair.second = NULL;
        m_vecWidgetPair.push_back(qPair);
        AddWidget(m_vecWidgetPair, true, false);
        m_bHaveEXP = false;
        return;
    }

    //设定滑块大小位置
    m_pCmbBoxContrast_level = new QComboBox(this);
    if (m_pCmbBoxContrast_level == NULL)
    {
        //内存已满立即返回
        return;
    }

    m_bHaveEXP = true;
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxContrast_level, Qt::black);
    m_pCmbBoxContrast_level->addItem("1", 1);
    m_pCmbBoxContrast_level->addItem("2", 2);
    m_pCmbBoxContrast_level->addItem("3", 3);
    m_pCmbBoxContrast_level->addItem("4", 4);
    m_pCmbBoxContrast_level->addItem("5", 5);
    m_pCmbBoxContrast_level->addItem("6", 6);
    m_pCmbBoxContrast_level->addItem("7", 7);
    m_pCmbBoxContrast_level->addItem("8", 8);
    m_pCmbBoxContrast_level->addItem("9", 9);
    m_pCmbBoxContrast_level->addItem("10", 10);

    connect(m_pCmbBoxContrast_level, SIGNAL(currentIndexChanged(int)), this,
            SLOT(OnCmbBoxIndexChange(int)));

    m_pCmbBoxContrast_level->installEventFilter(this);
    this->installEventFilter(this);

    strLab = LANG_TRANSLATE(TRID_CONTRAST);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxContrast_level;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgContrast::LoadPageData()
{
    if (!m_bHaveEXP)
    {
        return true;
    }
    this->setFocus();
    int iContrastLevel;

    //调用控制层：加载数据
    iContrastLevel = g_pContrastController->GetCurrentContrastLevel();

    SETTING_INFO(" CDlgContrast::LoadPageData  iContrastLevel = %d", iContrastLevel);

    //设置页面数据
    SetCurrentPageData(iContrastLevel);
    return true;
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgContrast::IsStatusChanged()
{
    if (!m_bHaveEXP)
    {
        return false;
    }
    int  iPageContrastLevel;
    int iBeforeContrastLevel;

    iPageContrastLevel = GetCurrentPageData();

    //调用控制层：加载数据
    iBeforeContrastLevel = g_pContrastController->GetCurrentContrastLevel();

    //比较数据
    if (iBeforeContrastLevel == iPageContrastLevel)
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

bool CDlgContrast::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgContrast::SavePageData()
{
    SETTING_INFO("CDlgContrast::SavePageData begain");
    if (!m_bHaveEXP)
    {
        return true;
    }
    //提取页面数据
    if (IsStatusChanged())
    {
        int  iCurrentPageData = GetCurrentPageData();
        SETTING_INFO("CDlgContrast::SavePageData iCurrentPageData = %d ", iCurrentPageData);
        //调用控制层：保存数据
        g_pContrastController->SetCurrentContrastLevel(iCurrentPageData);
    }
    return true;
}

int CDlgContrast::GetCurrentPageData()
{
    if (NULL != m_pCmbBoxContrast_level)
    {
        return m_pCmbBoxContrast_level
               ->itemData(m_pCmbBoxContrast_level->currentIndex(), Qt::UserRole).value<int>();
    }
    else
    {
        return g_pContrastController->GetCurrentContrastLevel();
    }
}

//设置当前页面数据
void CDlgContrast::SetCurrentPageData(int iCurrentPageData)
{
    m_pCmbBoxContrast_level->setCurrentIndex(m_pCmbBoxContrast_level
            ->findData(iCurrentPageData, Qt::UserRole));
}


void CDlgContrast::OnCmbBoxIndexChange(int iCurrentIndex)
{
    SETTINGUI_INFO("CDlgContrast::OnCmbBoxIndexChange");
    int iEXPNum = exp_GetEXPNum();
    for (int i = 0; i < iEXPNum; i++)
    {
        //根据对比度等级获取扩展台对比度值
        int iPageData = m_pCmbBoxContrast_level
                        ->itemData(iCurrentIndex, Qt::UserRole).value<int>();
        int iValue = iPageData * 10;
        // 发消息给EXP线程，让EXP线程处理显示消息
        msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_SET_CONTRAST,
                           i, iValue);
    }
}
