#include "model.h"
#include "cdlgmultiforward.h"
#include "uicommon.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "setting/include/modmenu.h"
#include "mainwnd/mainwnd.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "account/include/modaccount.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "dsskey/include/moddsskey.h"
#include "uicommon/uikernel/qlineeditutility.h"

//设定edit控件输入长度的最大值
const int EDIT_MAX_LENGTH = 99;

CDlgMultiForward::CDlgMultiForward(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditForward(NULL)
    , m_iKeyIndex(-1)
{
    InitData();
}

void CDlgMultiForward::InitData()
{
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ALWAYS_FORWARD).toUtf8().data();
    //初始化
    m_pEditForward = new QLineEdit(this);

    //指针判空
    if (m_pEditForward == NULL)
    {
        return;
    }

    SetLineEditDefaultStyle(m_pEditForward);

    //设置输入框输入的最大长度
    qLineEditUtility::SetMaxLength(m_pEditForward, EDIT_TYPE_PHONE_NUMBER);

    //为输入框安装输入法
    qInstallIME(m_pEditForward, IME_DIAL);
    /*
    设置标签上文字的大小及标签名
    LANG_TRANSLATE是翻译宏，为了方便语言的转化
    */
    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;


    strLab = LANG_TRANSLATE(TRID_FORWARD_TO);
    qPair.first = strLab;
    qPair.second = m_pEditForward;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pEditForward->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);

    connect(m_pGeneralFrame, SIGNAL(CurrentIndexChanged(int)), this,
            SLOT(OnRefreshSoftKey(int)));
}

void CDlgMultiForward::OnRefreshSoftKey(int nIndex)
{
    emit RefreshSoftkey();
}

bool CDlgMultiForward::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    return false;
}

/*
调用此函数前要先进行点击type的判断，当为ALWAYS时，才能进行调用，再按照type取对应页面值
*/
bool CDlgMultiForward::GetPageData(ForwardData * pData)
{
    return true;
}

bool CDlgMultiForward::OnAction(const QString & strAction)
{
    return CBaseDialog::OnAction(strAction);
}

bool CDlgMultiForward::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{

    //只有开关打开且转接置空时为无效输入

    if (m_pEditForward->text().trimmed().isEmpty())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_TARGET_CANNOT_BE_EMPTY);
        objErrorContent.pLineEdit = m_pEditForward;
        return false;
    }
    return true;
}

/*
调用此函数前要先进行点击type的判断，当为ALWAYS时，才进行对比
*/
bool CDlgMultiForward::IsStatusChanged()
{
    return true;
}


/*
调用此函数前要先进行点击type的判断，当为ALWAYS时，用此函数载入页面
*/
bool CDlgMultiForward::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditForward, EDIT_TYPE_PHONE_NUMBER);
    return true;
}

bool CDlgMultiForward::SavePageData()
{
    if (IsStatusChanged())
    {
        int iDsskeyID = m_iKeyIndex;
        DssKeyLogicData DKlogicData;
        bool ret = dsskey_GetData(iDsskeyID, DKlogicData);
        DKlogicData.strValue = m_pEditForward->text().toUtf8().data();
        dsskey_SetDsskeyData(iDsskeyID, DKlogicData);

        fwd_SetInfo(FWDDND_ACCOUNT_GLOBAL, FWD_TYPE_ALWAYS,
                    true, &DKlogicData.strValue, NULL, FWD_CFG_SRC_TYPE_DSSKEY);

        //回到上一层界面
        emit BtnBackClick();

    }
    return true;
}

CDlgMultiForward::~CDlgMultiForward()
{
}

// 设置子页面前的操作
void CDlgMultiForward::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_iKeyIndex = *((int *)pData);
    }
}

