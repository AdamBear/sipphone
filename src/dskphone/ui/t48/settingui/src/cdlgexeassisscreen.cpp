#include "cdlgexeassisscreen.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

CDlgExeAssisScreen::CDlgExeAssisScreen(QWidget * parent /*= 0*/)
    : CDlgExeAssisUIBase(parent)
{
    m_eUIType = UI_Screen;

    InitData();
}


ExeAssis_Action CDlgExeAssisScreen::GetAction()
{
    return EAA_Screen;
}

void CDlgExeAssisScreen::InitData()
{
    m_strTitle = LANG_TRANSLATE(TRID_EXECUTIVE_CALL_SCREEN);

    m_pCobScreenSwitch = new QComboBox(this);
    m_pCobAlterType = new QComboBox(this);
    m_pCobMobility = new QComboBox(this);
    m_pCobAnywhere = new QComboBox(this);
    m_pCobSCA = new QComboBox(this);

    qComboBoxUtility::SetComboBoxStyle(m_pCobScreenSwitch, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobAlterType, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobMobility, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobAnywhere, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCobSCA, Qt::black);

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_SCREEN_SWITCH);
    qPair.second = m_pCobScreenSwitch;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_SCREEN_TYPE);
    qPair.second = m_pCobAlterType;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_SCREEN_MOBILITY);
    qPair.second = m_pCobMobility;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_SCREEN_ANYWHERE);
    qPair.second = m_pCobAnywhere;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_SCREEN_SCA);
    qPair.second = m_pCobSCA;
    m_vecWidgetPair.push_back(qPair);

    m_pCobScreenSwitch->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCobScreenSwitch->addItem(LANG_TRANSLATE(TRID_ON), true);

    m_pCobAlterType->addItem(LANG_TRANSLATE(TRID_SCREEN_ALERTTYPE_SILENT), EAT_Silent);
    m_pCobAlterType->addItem(LANG_TRANSLATE(TRID_SCREEN_ALERTTYPE_SPLASH), EAT_Ring_Splash);

    m_pCobMobility->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCobMobility->addItem(LANG_TRANSLATE(TRID_ON), true);

    m_pCobAnywhere->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCobAnywhere->addItem(LANG_TRANSLATE(TRID_ON), true);

    m_pCobSCA->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCobSCA->addItem(LANG_TRANSLATE(TRID_ON), true);

    this->installEventFilter(this);
    m_pCobScreenSwitch->installEventFilter(this);
    m_pCobAlterType->installEventFilter(this);
    m_pCobMobility->installEventFilter(this);
    m_pCobAnywhere->installEventFilter(this);
    m_pCobSCA->installEventFilter(this);

    AddWidget(m_vecWidgetPair);
}

bool CDlgExeAssisScreen::RefreshData()
{
    if (g_refExeAssisUILogic.GetLogicData(&m_dataInfo))
    {
        m_pCobScreenSwitch->setCurrentIndex(m_pCobScreenSwitch->findData(m_dataInfo.bScreeningOn,
                                            Qt::UserRole));
        m_pCobAlterType->setCurrentIndex(m_pCobAlterType->findData(m_dataInfo.eAlterType, Qt::UserRole));
        m_pCobMobility->setCurrentIndex(m_pCobMobility->findData(m_dataInfo.bAlertMobility, Qt::UserRole));
        m_pCobAnywhere->setCurrentIndex(m_pCobAnywhere->findData(m_dataInfo.bAlertAnywhere, Qt::UserRole));
        m_pCobSCA->setCurrentIndex(m_pCobSCA->findData(m_dataInfo.bAlertSCA, Qt::UserRole));
    }

    return true;
}

bool CDlgExeAssisScreen::IsStatusChanged()
{
    CExeScreenInfo dataInfo;
    if (g_refExeAssisUILogic.GetLogicData(&dataInfo))
    {
        GetPageData();

        return dataInfo != m_dataInfo;
    }

    return false;
}

CExeAssisInspector * CDlgExeAssisScreen::GetPageData()
{
    ReturnAllEdit();

    m_dataInfo.bScreeningOn = m_pCobScreenSwitch->itemData(m_pCobScreenSwitch->currentIndex(),
                              Qt::UserRole).value<bool>();
    m_dataInfo.eAlterType = (EXE_Screen_Alter_Type)m_pCobAlterType->itemData(
                                m_pCobAlterType->currentIndex(), Qt::UserRole).value<int>();
    m_dataInfo.bAlertMobility = m_pCobMobility->itemData(m_pCobMobility->currentIndex(),
                                Qt::UserRole).value<bool>();
    m_dataInfo.bAlertAnywhere = m_pCobAnywhere->itemData(m_pCobAnywhere->currentIndex(),
                                Qt::UserRole).value<bool>();
    m_dataInfo.bAlertSCA = m_pCobSCA->itemData(m_pCobSCA->currentIndex(), Qt::UserRole).value<bool>();

    return &m_dataInfo;
}
#endif
