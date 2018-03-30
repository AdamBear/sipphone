#include "imageselectbox.h"

#include "baseui/framelist.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/qwidgetutility.h"

#define IMG_SEL_BOX_MUL_ROW_ITEM_AMOUNT 2

#define IMG_SEL_BOX_POS_MUL_ROW_DLG_LEFT 121
#define IMG_SEL_BOX_POS_MUL_ROW_DLG_TOP 75
#define IMG_SEL_BOX_POS_MUL_ROW_DLG_WIDTH 558
#define IMG_SEL_BOX_POS_MUL_ROW_DLG_HEIGHT 295

#define IMG_SEL_BOX_POS_MUL_ROW_LIST_LEFT 8
#define IMG_SEL_BOX_POS_MUL_ROW_LIST_TOP 20
#define IMG_SEL_BOX_POS_MUL_ROW_LIST_WIDTH 544
#define IMG_SEL_BOX_POS_MUL_ROW_LIST_HEIGHT 260

#define IMG_SEL_BOX_POS_MUL_ROW_PAGE_TOP_MARGIN 2
#define IMG_SEL_BOX_POS_MUL_ROW_PAGE_BOTTOM_MARGIN 6

#define IMG_SEL_BOX_SGL_ROW_ITEM_AMOUNT 1

#define IMG_SEL_BOX_POS_SGL_ROW_DLG_LEFT 144
#define IMG_SEL_BOX_POS_SGL_ROW_DLG_TOP 141
#define IMG_SEL_BOX_POS_SGL_ROW_DLG_WIDTH 512
#define IMG_SEL_BOX_POS_SGL_ROW_DLG_HEIGHT 162

#define IMG_SEL_BOX_POS_SGL_ROW_LIST_LEFT 8
#define IMG_SEL_BOX_POS_SGL_ROW_LIST_TOP 18
#define IMG_SEL_BOX_POS_SGL_ROW_LIST_WIDTH 496
#define IMG_SEL_BOX_POS_SGL_ROW_LIST_HEIGHT 130


CImageSelectBox::CImageSelectBox(int nRowNum, IFrameListDelegate * pDelegate/* = NULL*/,
                                 CMaskFrame * pMaskFrame/* = 0*/)
    : CMessageBoxBase(pMaskFrame)
    , m_nRowNum(nRowNum)
    , m_bRefresh(false)
    , m_pFrameList(NULL)
{
    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        if (m_nRowNum > 1)
        {
            m_pFrameList->SetMaxPageItemCount(IMG_SEL_BOX_MUL_ROW_ITEM_AMOUNT);
        }
        else
        {
            m_pFrameList->SetMaxPageItemCount(IMG_SEL_BOX_SGL_ROW_ITEM_AMOUNT);
        }

        m_pFrameList->SetDelegate(pDelegate);
    }

    Init();
}

CImageSelectBox::~CImageSelectBox()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(NULL);
    }
}

void CImageSelectBox::Init()
{
    if (m_nRowNum > 1)
    {
        setGeometry(IMG_SEL_BOX_POS_MUL_ROW_DLG_LEFT, IMG_SEL_BOX_POS_MUL_ROW_DLG_TOP,
                    IMG_SEL_BOX_POS_MUL_ROW_DLG_WIDTH, IMG_SEL_BOX_POS_MUL_ROW_DLG_HEIGHT);

        qWidgetUtility::SetBackGround(this, PIC_MSGBOX_IMAGE_BOX_MUL_ROW);

        if (NULL != m_pFrameList)
        {
            m_pFrameList->setGeometry(IMG_SEL_BOX_POS_MUL_ROW_LIST_LEFT, IMG_SEL_BOX_POS_MUL_ROW_LIST_TOP,
                                      IMG_SEL_BOX_POS_MUL_ROW_LIST_WIDTH, IMG_SEL_BOX_POS_MUL_ROW_LIST_HEIGHT);

            m_pFrameList->SetItemMargin(0, IMG_SEL_BOX_POS_MUL_ROW_PAGE_TOP_MARGIN,
                                        IMG_SEL_BOX_POS_MUL_ROW_PAGE_BOTTOM_MARGIN);
        }
    }
    else
    {
        setGeometry(IMG_SEL_BOX_POS_SGL_ROW_DLG_LEFT, IMG_SEL_BOX_POS_SGL_ROW_DLG_TOP,
                    IMG_SEL_BOX_POS_SGL_ROW_DLG_WIDTH, IMG_SEL_BOX_POS_SGL_ROW_DLG_HEIGHT);

        qWidgetUtility::SetBackGround(this, PIC_MSGBOX_IMAGE_BOX_SGL_ROW);

        if (NULL != m_pFrameList)
        {
            m_pFrameList->setGeometry(IMG_SEL_BOX_POS_SGL_ROW_LIST_LEFT, IMG_SEL_BOX_POS_SGL_ROW_LIST_TOP,
                                      IMG_SEL_BOX_POS_SGL_ROW_LIST_WIDTH, IMG_SEL_BOX_POS_SGL_ROW_LIST_HEIGHT);

            m_pFrameList->SetItemMargin(0, 0, 0);
        }
    }
}

bool CImageSelectBox::ProcessKeyEvent(int nKeyCode, bool bPress)
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

void CImageSelectBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CImageSelectBox::PrintMsgBoxInfo");
    }
    else
    {
        POPUPBOX_INFO("Del CImageSelectBox::PrintMsgBoxInfo");
    }
}

bool CImageSelectBox::event(QEvent * pEvent)
{
    if (NULL != pEvent && NULL != m_pFrameList)
    {
        if (pEvent->type() == QEvent::Show)
        {
            m_pFrameList->RefreshData(0);
        }
    }

    return CMessageBoxBase::event(pEvent);
}
