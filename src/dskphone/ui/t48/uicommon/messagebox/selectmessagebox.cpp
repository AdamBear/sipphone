#include "selectmessagebox.h"

#include "baseui/framelistdelegate.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/rectdefine.h"
#include "uikernel/iframelistdelegate.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"

namespace
{
// 列表项数量
#define SELECT_POP_BOX_TYPE_NORMAL_PAGE_ITEM 5
#define SELECT_POP_BOX_TYPE_TINY_PAGE_ITEM 3

// 标题位置
#define SELECT_POP_BOX_TITLE_LEFT 0
#define SELECT_POP_BOX_TITLE_TOP 0
#define SELECT_POP_BOX_TITLE_WIDHT 357
#define SELECT_POP_BOX_TITLE_HEIGHT 47

// 普通选择框位置
#define SELECT_POP_BOX_NORMAL_POS_DLG_LEFT 222
#define SELECT_POP_BOX_NORMAL_POS_DLG_TOP 64
#define SELECT_POP_BOX_NORMAL_POS_DLG_WIDTH 357
#define SELECT_POP_BOX_NORMAL_POS_DLG_HEIGHT 348

// FrameList位置
#define SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_LEFT 0
#define SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_WIDHT 357
#define SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_HEIGHT 235
#define SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_ITEM_MARGIN 8

// Tiny选择框位置
#define SELECT_POP_BOX_TINY_POS_DLG_LEFT 222
#define SELECT_POP_BOX_TINY_POS_DLG_TOP 101
#define SELECT_POP_BOX_TINY_POS_DLG_WIDTH 357
#define SELECT_POP_BOX_TINY_POS_DLG_HEIGHT 252

// FrameList位置
#define SELECT_POP_BOX_TINY_POS_FRAME_LIST_LEFT 0
#define SELECT_POP_BOX_TINY_POS_FRAME_LIST_WIDTH 357
#define SELECT_POP_BOX_TINY_POS_FRAME_LIST_HEIGHT 141
#define SELECT_POP_BOX_TINY_POS_FRAME_LIST_ITEM_MARGIN 0

// 按钮位置
#define SELECT_POP_BOX_POS_BTN_LEFT 4
#define SELECT_POP_BOX_POS_BTN_WIDHT 349
#define SELECT_POP_BOX_POS_BTN_HEIGHT 53

// 列表项高度
#define SELECT_POP_BOX_POS_FRAME_LIST_Y_SPACE 4
#define SELECT_POP_BOX_GRID_LINE_COLOR QColor(220, 220, 220, 255)

// #define DLG_SELECT_POP_BOX_FONT_SIZE 16

}

CSelectMessageBox::CSelectMessageBox(int nSelectBoxType, IFrameListDelegate * pDelegate/* = NULL*/,
                                     CMaskFrame * pMaskFrame/* = 0*/)
    : CMessageBoxBase(pMaskFrame)
    , m_nSelectBoxType(nSelectBoxType)
    , m_bRefresh(false)
    , m_pFrameList(NULL)
    , m_iHight(0)
{
//  qWidgetUtility::setFontStyle(this, DLG_SELECT_POP_BOX_FONT_SIZE);

    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(pDelegate);
    }
}

CSelectMessageBox::~CSelectMessageBox()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(NULL);
    }
}

//初始化控件
void CSelectMessageBox::Init(const QString & strTitle)
{
    int nFrameListLeft;
    int nFrameListWidth;
    int nFrameListHeight;


    m_labTitle.setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    SetWgtFontColor(&m_labTitle, FONT_TYPE_TEXT, Qt::white);
    m_labTitle.setGeometry(SELECT_POP_BOX_TITLE_LEFT, SELECT_POP_BOX_TITLE_TOP,
                           SELECT_POP_BOX_TITLE_WIDHT, SELECT_POP_BOX_TITLE_HEIGHT);
    m_labTitle.setText(strTitle);

    m_labTitle.setParent(this);
    m_labTitle.setObjectName("labTitle");

    if (NULL != m_pFrameList)
    {
        if (SELECT_POP_BOX_TYPE_NORMAL == m_nSelectBoxType)
        {
            m_pFrameList->SetMaxPageItemCount(SELECT_POP_BOX_TYPE_NORMAL_PAGE_ITEM);
            m_pFrameList->SetContentPos(SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_ITEM_MARGIN,
                                        SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_ITEM_MARGIN);
        }
        else
        {
            m_pFrameList->SetMaxPageItemCount(SELECT_POP_BOX_TYPE_TINY_PAGE_ITEM);
            m_pFrameList->SetContentPos(SELECT_POP_BOX_TINY_POS_FRAME_LIST_ITEM_MARGIN,
                                        SELECT_POP_BOX_TINY_POS_FRAME_LIST_ITEM_MARGIN);
        }

        m_pFrameList->SetGridLine(true);
        m_pFrameList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineImage(PIC_MSGBOX_SELECT_BOX_ITEM_LINE);
        m_pFrameList->SetItemClickBG(PIC_MSGBOX_SELECT_BOX_ITEM_SELECT);

        QColor clrGridLine = SELECT_POP_BOX_GRID_LINE_COLOR;

        m_pFrameList->SetGridLineColor(clrGridLine);
    }

    // 对话框
    if (SELECT_POP_BOX_TYPE_NORMAL == m_nSelectBoxType)
    {
        QRect rcDlg(SELECT_POP_BOX_NORMAL_POS_DLG_LEFT, SELECT_POP_BOX_NORMAL_POS_DLG_TOP,
                    SELECT_POP_BOX_NORMAL_POS_DLG_WIDTH, SELECT_POP_BOX_NORMAL_POS_DLG_HEIGHT);

        setGeometry(rcDlg);
        qWidgetUtility::SetBackGround(this, PIC_MSGBOX_SELECT_BOX_BG);

        nFrameListLeft = SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_LEFT;
        nFrameListWidth = SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_WIDHT;
        nFrameListHeight = SELECT_POP_BOX_NORMAL_POS_FRAME_LIST_HEIGHT;

        m_iHight = SELECT_POP_BOX_NORMAL_POS_DLG_HEIGHT;
    }
    else
    {
        QRect rcDlg(SELECT_POP_BOX_TINY_POS_DLG_LEFT, SELECT_POP_BOX_TINY_POS_DLG_TOP,
                    SELECT_POP_BOX_TINY_POS_DLG_WIDTH, SELECT_POP_BOX_TINY_POS_DLG_HEIGHT);

        setGeometry(rcDlg);
        qWidgetUtility::SetBackGround(this, PIC_MSGBOX_SELECT_BOX_BG_SMALL);

        nFrameListLeft = SELECT_POP_BOX_TINY_POS_FRAME_LIST_LEFT;
        nFrameListWidth = SELECT_POP_BOX_TINY_POS_FRAME_LIST_WIDTH;
        nFrameListHeight = SELECT_POP_BOX_TINY_POS_FRAME_LIST_HEIGHT;

        m_iHight = SELECT_POP_BOX_TINY_POS_DLG_HEIGHT;
    }


    // FrameList
    int nFrameListTop = SELECT_POP_BOX_TITLE_TOP + SELECT_POP_BOX_TITLE_HEIGHT +
                        SELECT_POP_BOX_POS_FRAME_LIST_Y_SPACE;

    if (NULL != m_pFrameList)
    {
        m_pFrameList->setGeometry(nFrameListLeft, nFrameListTop, nFrameListWidth, nFrameListHeight);
    }


}

void CSelectMessageBox::SetMessageBoxSpecialInfo(VecMsgBoxSoftkey & vecSoftkey)
{
    m_vecSoftkey = vecSoftkey;

    int nBtnTop = m_iHight - SELECT_POP_BOX_POS_BTN_HEIGHT - SELECT_POP_BOX_POS_FRAME_LIST_Y_SPACE;

    //自定义softkey
    m_BtnSet.setGeometry(SELECT_POP_BOX_POS_BTN_LEFT, nBtnTop,
                         SELECT_POP_BOX_POS_BTN_WIDHT, SELECT_POP_BOX_POS_BTN_HEIGHT);

    m_BtnSet.SetMaxBtnCount(vecSoftkey.size());

    m_BtnSet.show();

    int iCount = m_BtnSet.GetMaxBtnCount();

    ItMsgBoxSoftkey it = m_vecSoftkey.begin();
    for (int i = 0; it != m_vecSoftkey.end() && i < iCount; ++it, ++i)
    {
        qUIPushButton * pButton = new qUIPushButton();
        //设置图片和文字
        pButton->set3StatePic("", PIC_BTN_MSG_PRESS, "");

        pButton->setText(it->m_title);
        pButton->setBtnAction(it->m_title);

        if (MessageBox_OK == it->m_result)
        {
            m_bAllowOkButton = true;
        }

        //连接槽
        connect(pButton, SIGNAL(clicked(const QString &)), this, SLOT(OnButtonPressed(const QString &)));
        m_BtnSet.AddButton(pButton);
    }
}

bool CSelectMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (bPress && NULL != m_pFrameList && (PHONE_KEY_DOWN == nKeyCode || PHONE_KEY_UP == nKeyCode))
    {
        QKeyEvent keyEvent(QEvent::KeyPress, nKeyCode, Qt::NoModifier);

        if (m_pFrameList->eventFilter(this, &keyEvent))
        {
            return true;
        }
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

void CSelectMessageBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CSelectMessageBox::PrintMsgBoxInfo: %s", m_labTitle.text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CSelectMessageBox::PrintMsgBoxInfo: %s", m_labTitle.text().toUtf8().data());
    }
}

bool CSelectMessageBox::event(QEvent * pEvent)
{
    if (NULL != pEvent && NULL != m_pFrameList)
    {
        if (pEvent->type() == QEvent::Show)
        {
            CFrameListDelegatePtr pDelegate = (CFrameListDelegatePtr)m_pFrameList->GetFrameDelegate();

            if (NULL != pDelegate)
            {
                pDelegate->RefreshFrameList();
            }
        }
    }

    return CMessageBoxBase::event(pEvent);
}
