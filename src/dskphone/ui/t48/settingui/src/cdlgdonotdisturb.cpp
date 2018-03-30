#include "model.h"
#include "cdlgdonotdisturb.h"
#include "qtcommon/qmisc.h"
#include "setting/src/dndcontroller.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "messagebox/modmessagebox.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qcheckbox.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uikernel/qlineeditutility.h"

CDlgDoNotDisturb::CDlgDoNotDisturb(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditOffCode(NULL)
    , m_pEditOnCode(NULL)
    , m_iAccountID(-1)
    , m_qCheckBox(NULL)
{
    InitData();
}

void CDlgDoNotDisturb::InitData()
{
    m_pEditOnCode = new QLineEdit(this);
    m_pEditOffCode = new QLineEdit(this);
    m_qCheckBox = new qCheckBox(this);

    //指针判空
    if (m_pEditOnCode == NULL || m_pEditOffCode == NULL
            || m_qCheckBox == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditOnCode, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditOffCode, Qt::black);

    //为On Code安装输入法
    qInstallIME(m_pEditOnCode, IME_DIAL);

    //为Off Code安装输入法
    qInstallIME(m_pEditOffCode, IME_DIAL);

    //设置最大输入长度
    qLineEditUtility::SetMaxLength(m_pEditOnCode, EDIT_TYPE_CODE);
    qLineEditUtility::SetMaxLength(m_pEditOffCode, EDIT_TYPE_CODE);


    QWidgetPair qPair;
    QString strLab;


    strLab = LANG_TRANSLATE(TRID_DND_ENABLE);
    qPair.first = strLab;
    qPair.second = m_qCheckBox;
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


    //为界面上各输入控件安装事件过滤器
    m_pEditOnCode->installEventFilter(this);
    m_pEditOffCode->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

bool CDlgDoNotDisturb::GetPageData(DndData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    //指针判空
    if (m_pEditOnCode == NULL || m_pEditOffCode == NULL
            || m_qCheckBox == NULL)
    {
        return false;
    }

    //将当前界面显示值入夏pData
    pData->m_strOnCode = m_pEditOnCode->text().toUtf8().data(); //加入当前页面editOnCode的值
    pData->m_strOffCode =
        m_pEditOffCode->text().toUtf8().data();   //加入当前页面editOffCode的值
    pData->m_bEnabled = m_qCheckBox->GetData();

    return true;
}

bool CDlgDoNotDisturb::LoadPageData()
{
    this->setFocus();

    //指针判空
    if (m_pEditOnCode == NULL || m_pEditOffCode == NULL
            || m_qCheckBox == NULL)
    {
        return false;
    }

    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditOnCode, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditOffCode, EDIT_TYPE_PHONE_NUMBER);

    DndData objData;

    //获取当前界面原有配置值，存入objData
    bool bResult = g_pDndController->GetDndData(&objData, m_iAccountID);
    if (bResult)
    {
        //将界面显示值设为objData对应值
        m_pEditOnCode->setText(toQString(objData.m_strOnCode.c_str()));
        m_pEditOffCode->setText(toQString(objData.m_strOffCode.c_str()));
        m_qCheckBox->SetData(objData.m_bEnabled);
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgDoNotDisturb::IsStatusChanged()
{
    DndData objConfigData;
    DndData objPageData;

    //获取当前界面已有配置数据，存入objConfigData
    bool bResult = g_pDndController->GetDndData(&objConfigData, m_iAccountID);

    //获取当前界面显示数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);

    if (!bResult)
    {
        return false;
    }
    else
    {
        //判断对应项数据是否存在改变，只要有一处改变，就返回true
        if (objConfigData.m_strOnCode != objPageData.m_strOnCode
                || objConfigData.m_strOffCode != objPageData.m_strOffCode
                || objConfigData.m_bEnabled != objPageData.m_bEnabled)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}

bool CDlgDoNotDisturb::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
       )
    {
        if (m_pEditOnCode->text().isEmpty()
                && !m_pEditOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditOffCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditOffCode;
                return false;
            }
        }
        if (m_pEditOffCode->text().isEmpty()
                && !m_pEditOnCode->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OFF_CODE_CANNOT_BE_EMPTY);
            objErrorContent.pLineEdit = m_pEditOffCode;
            return false;
        }
        if (!m_pEditOnCode->text().isEmpty()
                && !m_pEditOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditOnCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_ON_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditOnCode;
                return false;
            }
            if (!qLineEditUtility::IsFormatValid(m_pEditOffCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditOffCode;
                return false;
            }
        }
    }
    return true;
}

// 设置子页面前的操作
void CDlgDoNotDisturb::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_iAccountID = *((int *)pData);
}

bool CDlgDoNotDisturb::SavePageData()
{
    if (IsStatusChanged())
    {
        //获取当前界面显示数据，设置为配置数据
        DndData  pageData;
        DndData objConfigData;

        //获取当前界面已有配置数据，存入objConfigData
        g_pDndController->GetDndData(&objConfigData, m_iAccountID);
        pageData.m_bEnabled = objConfigData.m_bEnabled;

        GetPageData(&pageData);

        if (g_pDndController->SetDndData(pageData, m_iAccountID))
        {
            return true;
        }
        else
        {
            //保存失败
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVE_CONFIG_FAILED_PLEASE_RETRY),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
            return false;
        }
    }
    return true;
}

CDlgDoNotDisturb::~CDlgDoNotDisturb()
{

}
