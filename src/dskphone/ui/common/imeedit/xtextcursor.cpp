#include "xtextcursor.h"

#include "dsklog/log.h"

#if IF_EDITTEXTVIEW_MULTI_EDIT

CTextCursor::CTextCursor()
    : m_nCursorPos(-1)
    , m_iCursorLineIndex(-1)
    , m_cursorCoordinate(0, 0)
    , m_iPreCursorCharXPos(0)
    , m_nAnchorStPos(-1)
    , m_nAnchorEnPos(-1)
{
}

CTextCursor::~CTextCursor()
{
}

int CTextCursor::GetCursorPos()
{
    return m_nCursorPos;
}

void CTextCursor::SetCursorPos(int nCursorPos)
{
    m_nCursorPos = nCursorPos;
}

int CTextCursor::GetCursorLineIndex()
{
    return m_iCursorLineIndex;
}

void CTextCursor::SetCursorLineIndex(int iCursorLineIndex)
{
    m_iCursorLineIndex = iCursorLineIndex;
}

int CTextCursor::GetAnchorStPos()
{
    return m_nAnchorStPos;
}

void CTextCursor::SetAnchorStPos(int nAnchorStPos)
{
    m_nAnchorStPos = nAnchorStPos;
}

int CTextCursor::GetAnchorEnPos()
{
    return m_nAnchorEnPos;
}

void CTextCursor::SetAnchorEnPos(int nAnchorEnPos)
{
    m_nAnchorEnPos = nAnchorEnPos;
}

int CTextCursor::GetPreCursorCharXPos()
{
    return m_iPreCursorCharXPos;
}

void CTextCursor::SetPreCursorCharXPos(int iPreCursorCharXPos)
{
    m_iPreCursorCharXPos = iPreCursorCharXPos;
}

chPoint CTextCursor::GetCursorCoordinate()
{
    return m_cursorCoordinate;
}

void CTextCursor::SetCursorCoordinate(chPoint cursorCoordinate)
{
    m_cursorCoordinate = cursorCoordinate;
}

void CTextCursor::DeSelectText()
{
    m_nAnchorStPos = -1;
    m_nAnchorEnPos = -1;
}

void CTextCursor::SetSelectArea(int iStPos, int iEnPos)
{
    m_nAnchorStPos = iStPos;
    m_nAnchorEnPos = iEnPos;
}

void CTextCursor::GetSelectArea(int &iStPos, int &iEnPos)
{
    iStPos = m_nAnchorStPos;
    iEnPos = m_nAnchorEnPos;
}

#endif // if IF_EDITTEXTVIEW_MULTI_EDIT
