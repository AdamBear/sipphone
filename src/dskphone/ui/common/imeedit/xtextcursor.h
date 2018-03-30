#ifndef __EDIT_TEXT_CURSOR_H__
#define __EDIT_TEXT_CURSOR_H__

#include <ETLLib/ETLLib.hpp>
#include <imeedit/imeeditdefine.h>

#if IF_EDITTEXTVIEW_MULTI_EDIT

class CTextCursor
{
public:
    CTextCursor();
    ~CTextCursor();

public:
    int GetCursorPos();
    void SetCursorPos(int nCursorPos);
    int GetCursorLineIndex();
    void SetCursorLineIndex(int iCursorLineIndex);
    int GetAnchorStPos();
    void SetAnchorStPos(int nAnchorStPos);
    int GetAnchorEnPos();
    void SetAnchorEnPos(int nAnchorEnPos);

    int GetPreCursorCharXPos();
    void SetPreCursorCharXPos(int iPreCursorCharXPos);
    chPoint GetCursorCoordinate();
    void SetCursorCoordinate(chPoint cursorCoordinate);

    void DeSelectText();
    void SetSelectArea(int iStPos, int iEnPos);
    void GetSelectArea(int &iStPos, int &iEnPos);

private:
    int                 m_nCursorPos;       // 焦点位置
    int                 m_iCursorLineIndex; // 焦点所在行
    chPoint             m_cursorCoordinate; // 焦点坐标
    int                 m_iPreCursorCharXPos;   // 焦点前一个字符X轴坐标
    int                 m_nAnchorStPos;     // 选中文本起点
    int                 m_nAnchorEnPos;     // 先中文本终点
};

#endif // if IF_EDITTEXTVIEW_MULTI_EDIT

#endif // __EDIT_TEXT_CURSOR_H__

