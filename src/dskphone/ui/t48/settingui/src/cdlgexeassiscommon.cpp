#include "cdlgexeassiscommon.h"
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

CDlgExeAssisCommon::CDlgExeAssisCommon(QWidget * parent /*= 0*/) : CDlgExeAssisUIBase(parent),
    m_pComboBox(0),
    m_pLineEdit(0),
    m_pExeDataInfo(0),
    m_pAssDataInfo(0)
{
    m_eUIType = UI_Common;

    InitData();
}

CDlgExeAssisCommon::~CDlgExeAssisCommon()
{
    DELETE_OBJ(m_pExeDataInfo);
    DELETE_OBJ(m_pAssDataInfo);
}

ExeAssis_Action CDlgExeAssisCommon::GetAction()
{
    if (IsExecutive())
    {
        return EAA_ExeCommon;
    }
    else if (IsAssistant())
    {
        return EAA_AssisCommon;
    }

    return EAA_None;
}

void CDlgExeAssisCommon::InitData()
{
    m_pComboBox = new QComboBox(this);
    m_pLineEdit = new QLineEdit(this);

    qComboBoxUtility::SetComboBoxStyle(m_pComboBox, Qt::black);
    SetLineEditDefaultStyle(m_pLineEdit);

    QWidgetPair qPair;
    if (IsAssistant() && m_pAssDataInfo == NULL)
    {
        m_strTitle = LANG_TRANSLATE(TRID_ASSISTANT_COMMON_SETTINGS);
        m_pAssDataInfo = new CAssCommon;

        qPair.first = LANG_TRANSLATE(TRID_ASSISTANT_DIVERT);
        qPair.second = m_pComboBox;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_ASSISTANT_DIVERT_TO);
        qPair.second = m_pLineEdit;
        m_vecWidgetPair.push_back(qPair);

        qInstallIME(m_pLineEdit, IME_Name);
    }
    else if (IsExecutive() && m_pExeDataInfo == NULL)
    {
        m_strTitle = LANG_TRANSLATE(TRID_EXECUTIVE_COMMON_SETTINGS);
        m_pExeDataInfo = new CExeCommon;

        qPair.first = LANG_TRANSLATE(TRID_EXECUTIVE_ALLOW_OPT);
        qPair.second = m_pComboBox;
        m_vecWidgetPair.push_back(qPair);
    }

    m_pComboBox->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pComboBox->addItem(LANG_TRANSLATE(TRID_ON), true);
    m_pComboBox->setCurrentIndex(m_pComboBox->findData(true, Qt::UserRole));

    m_pLineEdit->hide();

    m_pComboBox->installEventFilter(this);
    m_pLineEdit->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboxValueChanged(int)));

    AddWidget(m_vecWidgetPair);
}

bool CDlgExeAssisCommon::RefreshData()
{
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pLineEdit, EDIT_TYPE_NAME);

    if (IsExecutive() && NOT_EMPTY(m_pExeDataInfo))
    {
        if (g_refExeAssisUILogic.GetLogicData(m_pExeDataInfo))
        {
            m_pComboBox->setCurrentIndex(m_pComboBox->findData(m_pExeDataInfo->bAllowOpt, Qt::UserRole));
        }
    }
    else if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        if (g_refExeAssisUILogic.GetLogicData(m_pAssDataInfo))
        {
            m_pComboBox->setCurrentIndex(m_pComboBox->findData(m_pAssDataInfo->bDivertOn, Qt::UserRole));
            m_pLineEdit->setText(toQString(m_pAssDataInfo->strDivertNumber.c_str()));

            if (m_pAssDataInfo->listData.size() == 0)
            {
                this->setEnabled(false);
            }
        }
    }

    return true;
}

bool CDlgExeAssisCommon::IsStatusChanged()
{
    if (GetPageData() == NULL)
    {
        return false;
    }

    if (IsExecutive() && NOT_EMPTY(m_pExeDataInfo))
    {
        CExeCommon dataInfo;
        if (g_refExeAssisUILogic.GetLogicData(&dataInfo))
        {
            return dataInfo.bAllowOpt != m_pExeDataInfo->bAllowOpt;
        }
    }
    else if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        CAssCommon dataInfo;
        if (g_refExeAssisUILogic.GetLogicData(&dataInfo))
        {
            return dataInfo.bDivertOn != m_pAssDataInfo->bDivertOn
                   || dataInfo.strDivertNumber != m_pAssDataInfo->strDivertNumber;
        }
    }

    return false;
}

CExeAssisInspector * CDlgExeAssisCommon::GetPageData()
{
    ReturnAllEdit();

    if (IsExecutive() && NOT_EMPTY(m_pExeDataInfo))
    {
        m_pExeDataInfo->bAllowOpt = m_pComboBox->itemData(m_pComboBox->currentIndex(),
                                    Qt::UserRole).value<bool>();
        return m_pExeDataInfo;
    }
    else if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
    {
        m_pAssDataInfo->bDivertOn = m_pComboBox->itemData(m_pComboBox->currentIndex(),
                                    Qt::UserRole).value<bool>();
        m_pAssDataInfo->strDivertNumber = m_pLineEdit->text().toUtf8().data();

        return m_pAssDataInfo;
    }

    return NULL;
}

void CDlgExeAssisCommon::OnComboxValueChanged(int iIndex)
{
    if (IsAssistant())
    {
        m_pLineEdit->setEnabled(iIndex == 1);
    }
}

bool CDlgExeAssisCommon::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (GetPageData() != NULL)
    {
        if (IsAssistant() && NOT_EMPTY(m_pAssDataInfo))
        {
            if (m_pAssDataInfo->bDivertOn && m_pAssDataInfo->strDivertNumber.empty())
            {
                objErrorContent.strErrorContent = TRID_INVALID_RANGE_VALUE;
                objErrorContent.pLineEdit = m_pLineEdit;
                return false;
            }
        }
    }

    return true;
}
#endif
