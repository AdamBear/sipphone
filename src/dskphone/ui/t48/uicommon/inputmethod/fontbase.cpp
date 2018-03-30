#include "fontbase.h"

CFontBase::CFontBase(void)
{
}

CFontBase::~CFontBase(void)
{
}

void CFontBase::SetName(QString _qstrName)
{
    m_qstrName          = _qstrName;

}

void CFontBase::SetTitle(QString _qstrTitle)
{
    m_qstrTitle         = _qstrTitle;
}

QString CFontBase::GetName()
{
    return m_qstrName;
}

QString CFontBase::GetTitle()
{
    return m_qstrTitle;
}

QString CFontBase::GetFirstChar(int iKeyCode)
{
    return "";
}

bool CFontBase::IsPerInput(int iKeyCode)
{
    return false;
}

void CFontBase::Init()
{

}

void CFontBase::Release()
{

}

void CFontBase::LoadIMEFile()
{

}

bool CFontBase::DealKeyRelease(QObject* pObject, int iKeyCode)
{
    return false;
}
