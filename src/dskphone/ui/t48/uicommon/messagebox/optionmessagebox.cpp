#include "optionmessagebox.h"
#include "optionboxitem.h"

#include "baseui/t4xpicpath.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/rectdefine.h"
#include "uikernel/qwidgetutility.h"

namespace
{
static const QString MSGBOX_ACTION_OK("OK");
static const QString MSGBOX_ACTION_CANCEL("Cancel");

const int OPTIONBOX_TOP_EDGE(2);
const int CUTLINE_WIDTH(1);

const int OPTIONBOX_EDGE_WIDTH(0);
const int OPTIONBOX_ITEM_HEIGHT(48);

const int OPTIONBOX_RIGHT_CONTENT_EDGE_WITH_SCROLLBAR(28);

const int OPTIONBOX_WIDTH(357);
const int OPTIONBOX_HEIGHT(196);

const int OPTIONBOX_MAX_COUNT(7);

const int OPTIONBOX_ARC_WIDTH(10);
}

COptionMessageBox::COptionMessageBox(CMaskFrame * pMaskFrame /* = 0 */)
    : CMessageBoxBase(pMaskFrame)
    , m_pOptionFrameList(NULL)
    , m_bFocus(false)
    , m_nIndex(-1)
    , m_strBackGround(PIC_MSGBOX_OPTION_BOX_BACKGROUND)
{
    m_pOptionFrameList = new CFrameList(this);

    if (NULL != m_pOptionFrameList)
    {
        m_pOptionFrameList->SetDelegate(this);
    }
}

COptionMessageBox::~COptionMessageBox()
{
    if (NULL != m_pOptionFrameList)
    {
        m_pOptionFrameList->SetDelegate(NULL);
    }
}

//初始化控件
void COptionMessageBox::Init(const vecOptionMsgBoxItem & vecOptionItem)
{
    if (NULL == m_pOptionFrameList || 0 == vecOptionItem.size())
    {
        return;
    }

    m_vecOptionItem = vecOptionItem;

    m_pOptionFrameList->SetMaxPageItemCount(OPTIONBOX_MAX_COUNT);
    m_pOptionFrameList->SetDefaultFocus(false);
    m_pOptionFrameList->SetNote(LANG_TRANSLATE(TRID_NONE));
    m_pOptionFrameList->SetNoteFontColor(Qt::black);

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    m_pOptionFrameList->SetGridLine(true);
    m_pOptionFrameList->SetLinePosY(CUTLINE_WIDTH);
    m_pOptionFrameList->SetLineHeight(CUTLINE_WIDTH);
    m_pOptionFrameList->SetItemSpace(0);
    m_pOptionFrameList->SetLineImage(PIC_MSGBOX_OPTION_BOX_CUTLINE);
    m_pOptionFrameList->SetItemClickBG(PIC_MSGBOX_SELECT_BOX_ITEM_SELECT);
#endif

    m_pOptionFrameList->SetContentPos(0, 0);
    m_pOptionFrameList->SetItemMargin(OPTIONBOX_EDGE_WIDTH, OPTIONBOX_EDGE_WIDTH, OPTIONBOX_EDGE_WIDTH);
    m_pOptionFrameList->SetItemSpace(0);

    AdjustLayout();
}

void COptionMessageBox::SetDefaultFocus(bool bFocus, int nIndex)
{
    m_bFocus = bFocus;
    m_nIndex = nIndex;
}

void COptionMessageBox::SetTimeToIdle(int iTimeToExit)
{
    SetExitTime(iTimeToExit);
}

void COptionMessageBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add COptionMessageBox::PrintMsgBoxInfo");
    }
    else
    {
        POPUPBOX_INFO("Del COptionMessageBox::PrintMsgBoxInfo");
    }
}

void COptionMessageBox::GetSelectInfoToExtraData()
{
    if (NULL != m_pOptionFrameList)
    {
        int nFocusIndex = m_pOptionFrameList->GetFocusItemDataIndex();

        if (nFocusIndex < m_vecOptionItem.size())
        {
            SetExtraData(QVariant(m_vecOptionItem[nFocusIndex].m_strAction));
        }
    }
}

bool COptionMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (bPress && NULL != m_pOptionFrameList && (PHONE_KEY_DOWN == nKeyCode
            || PHONE_KEY_UP == nKeyCode))
    {
        return true;
    }

    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        return true;
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

bool COptionMessageBox::event(QEvent * pEvent)
{
    if (NULL != pEvent && NULL != m_pOptionFrameList)
    {
        if (pEvent->type() == QEvent::Show)
        {
            RefreshFrameList();
        }
    }

    return CMessageBoxBase::event(pEvent);
}

void COptionMessageBox::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    POPUPBOX_INFO("COptionMessageBox::OnItemAction[%d]", nAction);

    if (NULL == pItem)
    {
        return;
    }

    if (LIST_ITEM_ACTION_SELECT == nAction)
    {
        GetSelectInfoToExtraData();
        ExitMessageBox(MessageBox_OK);
    }
}

void COptionMessageBox::AdjustLayout()
{
    int nOptionSize = m_vecOptionItem.size();
    nOptionSize = nOptionSize > OPTIONBOX_MAX_COUNT ? OPTIONBOX_MAX_COUNT : nOptionSize;
    int nOptionBoxHeight = (CUTLINE_WIDTH + OPTIONBOX_ITEM_HEIGHT) * nOptionSize;
//  int nOptionBoxHeight = OPTIONBOX_HEIGHT;

    QRect rcMsgBox((PHONE_WIDTH - OPTIONBOX_WIDTH) / 2
                   , (PHONE_HEIGHT - nOptionBoxHeight) / 2
                   , OPTIONBOX_WIDTH
                   , nOptionBoxHeight);

    setGeometry(rcMsgBox);

    if (NULL == m_pOptionFrameList)
    {
        return;
    }

    m_pOptionFrameList->setGeometry(0, OPTIONBOX_TOP_EDGE, OPTIONBOX_WIDTH, nOptionBoxHeight);
}

void COptionMessageBox::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    int nOptionSize = m_vecOptionItem.size();
    if (NULL == pFrameList || nOptionSize <= 0 || nOptionSize < nDataIndex)
    {
        return;
    }

    pFrameList->ClearContent();
    pFrameList->SetItemAttach();
    pFrameList->SetTotalDataCount(nOptionSize);

    int nRightWidth = m_vecOptionItem.size() > OPTIONBOX_MAX_COUNT ?
                      OPTIONBOX_RIGHT_CONTENT_EDGE_WITH_SCROLLBAR : 0;

    for (int i = 0; i < nOptionSize && nDataIndex < nOptionSize
            ; ++i, ++nDataIndex)
    {
        COptionBoxItem * pItem = new COptionBoxItem(pFrameList, m_bFocus);
        if (NULL == pItem)
        {
            continue;
        }

        if (nDataIndex < nOptionSize)
        {
            pItem->SetText(m_vecOptionItem[nDataIndex].m_strTitle);
        }

        pItem->setFixedHeight(OPTIONBOX_ITEM_HEIGHT);
        pItem->SetItemRightMargin(nRightWidth);

        pItem->SetDataIndex(nDataIndex);
        pItem->SetAction(this);

        if (m_nIndex == nDataIndex)
        {
            pItem->SetCurrentSelected(true);
        }

        pFrameList->AddItem(pItem);
    }

    pFrameList->update();
}

void COptionMessageBox::paintEvent(QPaintEvent * pPaintEvent)
{
    QRect rcMsgBox = geometry();

    QPixmap picBackGround = THEME_GET_BMP(m_strBackGround.c_str());
    if (picBackGround.isNull())
    {
        return;
    }

    int nPicWidth = picBackGround.width();
    int nPicHeight = picBackGround.height();

    QStylePainter painter(this);

    painter.drawPixmap(
        //目标位置
        0
        , 0
        , rcMsgBox.width()
        , OPTIONBOX_ARC_WIDTH
        //图片
        , picBackGround
        //源位置
        , 0
        , 0
        , nPicWidth
        , OPTIONBOX_ARC_WIDTH);

    painter.drawPixmap(
        //目标位置
        0
        , OPTIONBOX_ARC_WIDTH
        , rcMsgBox.width()
        , rcMsgBox.height() - OPTIONBOX_ARC_WIDTH - OPTIONBOX_ARC_WIDTH
        //图片
        , picBackGround
        //源位置
        , 0
        , OPTIONBOX_ARC_WIDTH
        , nPicWidth
        , nPicHeight - OPTIONBOX_ARC_WIDTH - OPTIONBOX_ARC_WIDTH);

    painter.drawPixmap(
        //目标位置
        0
        , rcMsgBox.height() - OPTIONBOX_ARC_WIDTH
        , rcMsgBox.width()
        , OPTIONBOX_ARC_WIDTH
        //图片
        , picBackGround
        //源位置
        , 0
        , nPicHeight - OPTIONBOX_ARC_WIDTH
        , nPicWidth
        , OPTIONBOX_ARC_WIDTH);
}

