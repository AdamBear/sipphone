/************************************************************************
 * FileName  : CDlgACDUnavailableReason.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/22 14:52:05
 ************************************************************************/

#include "cdlgacdunavailablereason.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uikernel/languagehelper.h"
#include "mainwnd/mainwnd.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "settinguicommon.h"

CDlgACDUnavailableReason::CDlgACDUnavailableReason(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCodeEdit(NULL)
{
    SetWindowType(eWT_SubPage);

    /*SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_SETTING);*/

    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_UNAVAILABLE_REASON_CODE).toUtf8().data();

    m_pCodeEdit = new QLineEdit(this);

    if (m_pCodeEdit)
    {
        qLineEditUtility::SetLineEditStyle(m_pCodeEdit, Qt::black);
        qLineEditUtility::SetMaxLength(m_pCodeEdit, EDIT_TYPE_PHONE_NUMBER);

        //为输入框安装输入法
        qInstallIME(m_pCodeEdit, IME_DIAL);

        m_pCodeEdit->installEventFilter(this);
    }

    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

CDlgACDUnavailableReason::~CDlgACDUnavailableReason()
{

}

// 读取页面数据
bool CDlgACDUnavailableReason::LoadPageData()
{
    if (m_pListDelegate == NULL)
    {
        return false;
    }

    //清空旧的数据
    m_pListDelegate->ClearContent(false);
    QWPVector::iterator iter = m_vecWidgetPair.begin();
    for (; iter != m_vecWidgetPair.end(); ++iter)
    {
        QWidgetPair pair = *iter;
        if (pair.second != NULL && pair.second != m_pCodeEdit)
        {
            pair.second->deleteLater();
        }
    }
    m_vecWidgetPair.clear();

    //reason数据
    DispCodeList listData;
    ACD_GetReasonMap(listData);

    m_codeVec.clear();

    if (m_pCodeEdit)
    {
        QWidgetPair qPair;
        qPair.first = LANG_TRANSLATE(TRID_ENTER_CODE);
        qPair.second = m_pCodeEdit;
        m_vecWidgetPair.push_back(qPair);
    }

    //添加数据项
    for (DispCodeList::iterator iter = listData.begin();
            iter != listData.end(); ++iter)
    {
        const DispCode & codePair = *iter;
        PushbackResonItem(codePair.m_strCode, codePair.m_strCodeName);
    }

    AddWidget(m_vecWidgetPair, true, true, true, true);

    return true;
}

// 查看页面状态是否改变
bool CDlgACDUnavailableReason::IsStatusChanged()
{
    return false;
}

// 保存页面数据
bool CDlgACDUnavailableReason::SavePageData()
{
    int iRsnIdx = m_pGeneralFrame->GetFocusItemDataIndex();
    ReturnAllEdit();

    yl::string strCode;
    if (iRsnIdx == 0
            && m_pCodeEdit != NULL)
    {
        strCode = m_pCodeEdit->text().toUtf8().data();
    }
    else if (iRsnIdx > 0
             && m_pGeneralFrame != NULL)
    {
        // 去掉第一项的编辑框
        iRsnIdx--;
        if (iRsnIdx < m_codeVec.size())
        {
            strCode = m_codeVec[iRsnIdx];
        }
    }

    //设置Unavailable Reason Code
    ACD_SetCurReason(strCode);
    ACD_ReqChangeACDStatus(AA_UNAVAIL);
    OnAction(ACTION_QUIT);
    return true;
}

// 保存成功后的其他操作
bool CDlgACDUnavailableReason::SaveCompleted()
{
    return false;
}

void CDlgACDUnavailableReason::SetData(void * pData  /*= NULL*/)
{

}

bool CDlgACDUnavailableReason::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData[3].m_strSoftkeyTitle    = LANG_TRANSLATE(TRID_OK);

    return true;
}

void CDlgACDUnavailableReason::PushbackResonItem(const yl::string & strResnCode,
        const yl::string & strResnName)
{
    if (strResnCode.empty())
    {
        // 如果Code为空，则不添加
        return;
    }

    QWidgetPair qPair;
    qPair.eType = SETTING_ITEM_NORMAL;
    QLabel * pLab = new QLabel(toQString(strResnName), this);

    qPair.first = toQString(strResnCode);
    qPair.second = pLab;

    //记录Reason项，用于操作
    m_codeVec.push_back(strResnCode);

    //加到显示项中，用于显示
    m_vecWidgetPair.push_back(qPair);
}

//事件过滤器
bool CDlgACDUnavailableReason::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

