#include "model.h"
#include "cdlgkeygaurdlock.h"
#include "setting/include/common.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "keyguard/include/modkeyguard.h"
#include "uikernel/qcomboboxutility.h"

#include "uikernel/qlineeditutility.h"
#include "service_config.h"

#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "etlmsghandler/modetlmsghandler.h"

#include <devicelib/phonedevice.h>

namespace
{
#define LOCK_TIME_OUT_MAX_INPUT_LENGTH 4
}

//构造函数
CDlgKeyGuardLock::CDlgKeyGuardLock(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbKeyGuardEnable(NULL)
    , m_pCmbKeyGuardType(NULL)
    , m_pEditKeyGuardTime(NULL)
{
    //初始化数据
    InitData();
}

CDlgKeyGuardLock::~CDlgKeyGuardLock()
{

}

//界面初始化
void CDlgKeyGuardLock::InitData()
{
    QWidgetPair qPair;
    QString strLab;

    m_pCmbKeyGuardEnable = new QComboBox(this);

    if (NULL != m_pCmbKeyGuardEnable)
    {
        qComboBoxUtility::SetComboBoxStyle(m_pCmbKeyGuardEnable, Qt::black);
        m_pCmbKeyGuardEnable->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
        m_pCmbKeyGuardEnable->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

        strLab = LANG_TRANSLATE(TRID_KEYPAD_LOCK_ENABLE);
        qPair.first = strLab;
        qPair.second = m_pCmbKeyGuardEnable;
        m_vecWidgetPair.push_back(qPair);

        m_pCmbKeyGuardEnable->installEventFilter(this);
    }

#if !defined(_T48)
    m_pCmbKeyGuardType = new QComboBox(this);
    if (NULL != m_pCmbKeyGuardType)
    {
        strLab = LANG_TRANSLATE(TRID_LOCK_TYPE);
        qPair.first = strLab;
        qPair.second = m_pCmbKeyGuardType;
        m_vecWidgetPair.push_back(qPair);

        qComboBoxUtility::SetComboBoxStyle(m_pCmbKeyGuardType, Qt::black);
        m_pCmbKeyGuardType->addItem(LANG_TRANSLATE(TRID_ALL_KEYS), (int)KG_ALL_KEYS);
        m_pCmbKeyGuardType->addItem(LANG_TRANSLATE(TRID_FUNCTION_KEY), (int)KG_FUN_KEY);
        m_pCmbKeyGuardType->addItem(LANG_TRANSLATE(TRID_MENU_KEY), (int)KG_MENU_KEY);

        m_pCmbKeyGuardType->installEventFilter(this);
    }
#endif

    m_pEditKeyGuardTime = new QLineEdit(this);
    if (NULL != m_pEditKeyGuardTime)
    {
        qInstallIME(m_pEditKeyGuardTime, IME_Number);
        m_pEditKeyGuardTime->setMaxLength(LOCK_TIME_OUT_MAX_INPUT_LENGTH);

        qLineEditUtility::SetLineEditStyle(m_pEditKeyGuardTime, Qt::black);
        m_pEditKeyGuardTime->setEchoMode(QLineEdit::Normal);

        strLab = LANG_TRANSLATE(TRID_KEYPAD_LOCK_TIME_OUT);
        qPair.first = strLab;
        qPair.second = m_pEditKeyGuardTime;
        m_vecWidgetPair.push_back(qPair);

        m_pEditKeyGuardTime->installEventFilter(this);
    }


    AddWidget(m_vecWidgetPair);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//加载数据
bool CDlgKeyGuardLock::LoadPageData()
{
    this->setFocus();

    m_pCmbKeyGuardEnable->setCurrentIndex(m_pCmbKeyGuardEnable->
                                          findData(keyGuard_IsEnable(), Qt::UserRole));

#if !defined(_T48)
    m_pCmbKeyGuardType->setCurrentIndex(m_pCmbKeyGuardType->
                                        findData((int)keyGuard_GetKeyGuardType(), Qt::UserRole));
#endif
    m_pEditKeyGuardTime->setText(QString::number(configParser_GetConfigInt(kszLockTimeout)));

    return true;
}

//获取数据状态是否改变
bool CDlgKeyGuardLock::IsStatusChanged()
{
    bool bResult = (keyGuard_IsEnable() !=
                    m_pCmbKeyGuardEnable->itemData(m_pCmbKeyGuardEnable->currentIndex(), Qt::UserRole).value<bool>()
                    || (configParser_GetConfigInt(kszLockTimeout) != (m_pEditKeyGuardTime->text()).toInt()));

#if !defined(_T48)
    bResult |= ((int)keyGuard_GetKeyGuardType() !=
                m_pCmbKeyGuardType->itemData(m_pCmbKeyGuardType->currentIndex(), Qt::UserRole).value<int>());
#endif

    return bResult;
}

bool CDlgKeyGuardLock::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    bool bOK;
    int iTime = (m_pEditKeyGuardTime->text()).toInt(&bOK, 10);

    if (!bOK || (m_pEditKeyGuardTime->text()).isEmpty())
    {
        objErrorContent.strErrorContent = TRID_VALUE_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditKeyGuardTime;

        return false;
    }
    //如果自动上锁时间大于3600则是不合法的
    if (iTime > 3600)
    {
        objErrorContent.strErrorContent = TRID_KEYPAD_LOCK_TIME_OVER;
        objErrorContent.pLineEdit = m_pEditKeyGuardTime;

        return false;
    }

    //on code与off code需要同时可用或同时不可用
    return true;
}

//保存数据
bool CDlgKeyGuardLock::SavePageData()
{
#if !defined(_T48)
    configParser_SetConfigInt(kszKeyGuardType,
                              ((KEY_GUARD_TYPE)m_pCmbKeyGuardType->itemData(m_pCmbKeyGuardType->currentIndex(),
                                      Qt::UserRole).value<int>()), CONFIG_LEVEL_PHONE);
#endif

    configParser_SetConfigInt(kszKeyGuardEnable,
                              (m_pCmbKeyGuardEnable->itemData(m_pCmbKeyGuardEnable->currentIndex(), Qt::UserRole).value<bool>()),
                              CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszLockTimeout, (m_pEditKeyGuardTime->text()).toInt(),
                              CONFIG_LEVEL_PHONE);
    etl_NotifyApp(false,  CONFIG_MSG_BCAST_CHANGED,  ST_LOCK, 0);

    return true;
}

