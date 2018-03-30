#include "model.h"
#include "cdlgcallwaiting.h"

#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callwaitingcontroller.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "inputmethod/inputmethod.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

CDlgCallWaiting::CDlgCallWaiting(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxCallWait(NULL)
    , m_qCheckBoxPlayTone(NULL)
    , m_pEditOnCode(NULL)
    , m_pEditOffCode(NULL)

{
    InitData();
}

void CDlgCallWaiting::InitData()
{
    m_qCheckBoxPlayTone = new qCheckBox(this);
    m_qCheckBoxCallWait = new qCheckBox(this);

    m_pEditOnCode = new QLineEdit(this);
    qLineEditUtility::SetLineEditStyle(m_pEditOnCode, Qt::black);
    qLineEditUtility::SetMaxLength(m_pEditOnCode, EDIT_TYPE_CODE);
    qInstallIME(m_pEditOnCode, IME_DIAL);

    m_pEditOffCode = new QLineEdit(this);
    qLineEditUtility::SetLineEditStyle(m_pEditOffCode, Qt::black);
    qLineEditUtility::SetMaxLength(m_pEditOffCode, EDIT_TYPE_CODE);
    qInstallIME(m_pEditOffCode, IME_DIAL);

    //指针判空
    if (m_qCheckBoxCallWait == NULL || m_qCheckBoxPlayTone == NULL
            || m_pEditOnCode == NULL || m_pEditOffCode == NULL)
    {
        return;
    }

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_CALL_WAITING);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxCallWait;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PLAY_TONE);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxPlayTone;
    m_vecWidgetPair.push_back(qPair);

    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
       )
    {
        strLab = LANG_TRANSLATE(TRID_ON_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditOnCode;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_OFF_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditOffCode;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pEditOnCode->hide();
        m_pEditOffCode->hide();
    }

    AddWidget(m_vecWidgetPair);

    //为界面上各控件安装事件过滤器
    m_qCheckBoxPlayTone->installEventFilter(this);
    m_qCheckBoxCallWait->installEventFilter(this);
    m_pEditOnCode->installEventFilter(this);
    m_pEditOffCode->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

bool CDlgCallWaiting::GetPageData(CallWaitingData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    if (NULL == m_qCheckBoxCallWait  || NULL == m_qCheckBoxPlayTone
            || NULL == m_pEditOnCode || NULL == m_pEditOffCode)
    {
        return false;
    }

    //读取界面上滑块位置表示的值，存入pData
    pData->m_isOn = m_qCheckBoxCallWait->GetData();

    pData->m_isPlayTone = m_qCheckBoxPlayTone->GetData();

    pData->m_strOnCode = m_pEditOnCode->text().toUtf8().data();
    pData->m_strOffCode = m_pEditOffCode->text().toUtf8().data();

    return true;
}

bool CDlgCallWaiting::IsStatusChanged()
{
    CallWaitingData objConfigData;
    CallWaitingData objPageData;

    //读取页面已有的配置数据，存入objConfigData
    bool bResult = g_pCallWaitingController->GetCallWaitingData(&objConfigData);

    //读取界面当前显示的配置值，存入objPageData
    bResult = bResult && GetPageData(&objPageData);

    if (bResult)
    {
        //判断对应项数据是否存在改变，只要有一处改变，就返回true
        if (objConfigData.m_isOn != objPageData.m_isOn
                || objConfigData.m_isPlayTone != objPageData.m_isPlayTone
                || objConfigData.m_strOnCode != objPageData.m_strOnCode
                || objConfigData.m_strOffCode != objPageData.m_strOffCode)
        {
            return true;
        }
    }

    return false;
}

bool CDlgCallWaiting::LoadPageData()
{
    this->setFocus();
    CallWaitingData objData;

    //指针判空
    if (m_qCheckBoxCallWait == NULL || m_qCheckBoxPlayTone == NULL
            || m_pEditOnCode == NULL || m_pEditOffCode == NULL)
    {
        return false;
    }

    //读取界面已有的配置数据，存入objData
    bool bResult = g_pCallWaitingController->GetCallWaitingData(&objData);
    if (bResult)
    {
        //根据已有的配置值，设置Call Waiting对应的滑块位置
        if (NULL == m_qCheckBoxPlayTone  || NULL == m_qCheckBoxCallWait
                || NULL == m_pEditOnCode || NULL == m_pEditOffCode)
        {
            return false;
        }

        m_qCheckBoxCallWait->SetData(objData.m_isOn);
        m_qCheckBoxPlayTone->SetData(objData.m_isPlayTone);

        m_pEditOnCode->setText(toQString(objData.m_strOnCode.c_str()));
        m_pEditOffCode->setText(toQString(objData.m_strOffCode.c_str()));

        return true;
    }

    return false;
}

bool CDlgCallWaiting::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
       )
    {
        if (!m_pEditOnCode->text().isEmpty() && m_pEditOffCode->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(
                                                  TRID_OFF_CODE_CANNOT_BE_EMPTY_WHEN_ON_CODE_NOT_EMTPY);
            objErrorContent.pLineEdit = m_pEditOffCode;
            return false;
        }
    }
    return true;
}

bool CDlgCallWaiting::SavePageData()
{
    if (!IsStatusChanged())
    {
        return true;
    }

    CallWaitingData pageData;
    GetPageData(&pageData);

    //把当前界面显示数据设为配置数据
    g_pCallWaitingController->SetCallWaitingData(pageData);
    return true;

}


CDlgCallWaiting::~CDlgCallWaiting()
{

}
