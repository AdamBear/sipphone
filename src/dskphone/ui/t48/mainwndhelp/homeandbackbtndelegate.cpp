#include "homeandbackbtndelegate.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"

CHomeAndBackBtnDelegate::CHomeAndBackBtnDelegate()
{
    m_pBackBtn = new qUIPushButton();
    if (!m_pBackBtn.isNull())
    {
        m_pBackBtn->setObjectName("BackBtn");

        m_pBackBtn->set3StatePic(PIC_BTN_BACK, PIC_BTN_BACK, PIC_BTN_BACK);

        m_pBackBtn->SetMask(PIC_BTN_PRESS);
        connect(m_pBackBtn, SIGNAL(clicked()), this, SLOT(OnBackBtnClick()));
    }

    m_pHomeBtn = new qUIPushButton();
    if (!m_pHomeBtn.isNull())
    {
        m_pHomeBtn->setObjectName("HomeBtn");

        m_pHomeBtn->set3StatePic(PIC_BTN_HOME, PIC_BTN_HOME, PIC_BTN_HOME);

        m_pHomeBtn->SetMask(PIC_BTN_PRESS);
        connect(m_pHomeBtn, SIGNAL(clicked()), this, SLOT(OnHomeBtnClick()));
    }
}

CHomeAndBackBtnDelegate::~CHomeAndBackBtnDelegate()
{

}

void CHomeAndBackBtnDelegate::DrawWidget(CBaseDialog * pDialog)
{
    if (NULL == pDialog)
    {
        return;
    }

    if (pDialog->IsShowHomeButton())
    {
        if (!m_pHomeBtn.isNull())
        {
            m_pHomeBtn->show();
            m_pHomeBtn->raise();
        }
    }
    else
    {
        if (!m_pHomeBtn.isNull())
        {
            m_pHomeBtn->hide();
        }
    }

    if (pDialog->IsShowBackButton())
    {
        if (!m_pBackBtn.isNull())
        {
            m_pBackBtn->show();
            m_pBackBtn->raise();
        }
    }
    else
    {
        if (!m_pBackBtn.isNull())
        {
            m_pBackBtn->hide();
        }
    }
}

void CHomeAndBackBtnDelegate::SetParent(QWidget * pWgt)
{
    if (!m_pBackBtn.isNull())
    {
        m_pBackBtn->setParent(pWgt);
        m_pBackBtn->setGeometry(5, 390, 150, 90);
    }

    if (!m_pHomeBtn.isNull())
    {
        m_pHomeBtn->setParent(pWgt);
        m_pHomeBtn->setGeometry(645, 390, 150, 90);
    }

    setParent(pWgt);
}

void CHomeAndBackBtnDelegate::SetVisible(bool bVisible)
{
    if (!m_pBackBtn.isNull())
    {
        bVisible ? m_pBackBtn->show() : m_pBackBtn->hide();
    }

    if (!m_pHomeBtn.isNull())
    {
        bVisible ? m_pHomeBtn->show() : m_pHomeBtn->hide();
    }
}

void CHomeAndBackBtnDelegate::Raise()
{
    if (!m_pBackBtn.isNull())
    {
        m_pBackBtn->raise();
    }

    if (!m_pHomeBtn.isNull())
    {
        m_pHomeBtn->raise();
    }
}

// 响应点击BackBtn时候的操作
void CHomeAndBackBtnDelegate::OnBackBtnClick()
{
    CBaseDialog * pBaseDialog = UIManager_GetTopWindow();
    if (NULL != pBaseDialog)
    {
        pBaseDialog->OnBackButtonClick();
    }
}

// 响应点击HomeBtn时候的操作
void CHomeAndBackBtnDelegate::OnHomeBtnClick()
{
    CBaseDialog * pBaseDialog = UIManager_GetTopWindow();
    if (NULL != pBaseDialog)
    {
        pBaseDialog->OnHomeButtonClick();
    }
}

