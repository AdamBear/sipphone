#include "t2xwidget.h"


CIconHotKey::CIconHotKey()
    : m_strIcon("")
{

}

CIconHotKey::~CIconHotKey()
{

}

void CIconHotKey::UpdateHotKeyBtn(xButton * pBtn)
{
    xSlotHotKey::UpdateHotKeyBtn(pBtn);

    if (NULL != pBtn)
    {
        pBtn->SetPixmap(m_strIcon);
    }
}
