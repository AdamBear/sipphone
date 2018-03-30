/************************************************************************
 * FileName  : CDlgACDDispositionCode.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/23 16:33:44
 ************************************************************************/
#include "cdlgacddispositioncode.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"

CDlgACDDispositionCode::CDlgACDDispositionCode(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pCodeEdit(NULL)
    , m_nCallId(-1)
{
    SetWindowType(eWT_SubPage);

    /*SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_SETTING);*/

    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ACD_DISPOSITION_CODE).toUtf8().data();

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

CDlgACDDispositionCode::~CDlgACDDispositionCode()
{

}

// 设置子页面前的操作
void CDlgACDDispositionCode::BeforeSetSubPage(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_nCallId = *(int *)pData;
}

bool CDlgACDDispositionCode::LoadPageData()
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

    //添加编辑框控件
    if (m_pCodeEdit)
    {
        QWidgetPair qPair;
        qPair.first = LANG_TRANSLATE(TRID_ENTER_CODE);
        qPair.second = m_pCodeEdit;
        m_vecWidgetPair.push_back(qPair);
    }

    //获取diposition code列表
    DispCodeList dcl;
    ACD_GetDispCodeList(dcl);

    m_codeVec.clear();

    //添加数据项
    for (DispCodeList::iterator iter = dcl.begin();
            iter != dcl.end(); ++iter)
    {
        PushbackItem((*iter).m_strCode, (*iter).m_strCodeName);
    }

    AddWidget(m_vecWidgetPair, true, true, true, true);

    return true;
}

void CDlgACDDispositionCode::PushbackItem(const yl::string & strCode, const yl::string & strName)
{
    if (strCode.empty())
    {
        // 如果Code为空，则不添加
        return;
    }

    QWidgetPair qPair;
    QLabel * pLab = new QLabel(toQString(strName), this);

    qPair.first = toQString(strCode);
    qPair.second = pLab;

    //记录Reason项，用于操作
    m_codeVec.push_back(strCode);

    //加到显示项中，用于显示
    m_vecWidgetPair.push_back(qPair);
}

bool CDlgACDDispositionCode::IsStatusChanged()
{
    return false;
}

bool CDlgACDDispositionCode::SavePageData()
{
    int iRsnIdx = m_pGeneralFrame->GetFocusItemDataIndex();

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

    CallCenter_SendDispCode(strCode, m_nCallId);

    return true;
}

bool CDlgACDDispositionCode::SaveCompleted()
{
    return false;
}

//事件过滤器
bool CDlgACDDispositionCode::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgACDDispositionCode::SetCallID(int iCallID)
{
    m_nCallId = iCallID;
}

void CDlgACDDispositionCode::SetData(void * pData  /*= NULL*/)
{

}

bool CDlgACDDispositionCode::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData[3].m_strSoftkeyTitle    = LANG_TRANSLATE(TRID_OK);

    return true;
}
