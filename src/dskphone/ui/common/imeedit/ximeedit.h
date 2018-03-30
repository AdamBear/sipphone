#ifndef __IMEEDIT_XIMEEDIT_H__
#define __IMEEDIT_XIMEEDIT_H__

#include <xwindow/xWindow.h>
#include "ylime.h"
#include <imeedit/imeeditdefine.h>
#include "xwindow-kernel/include/xthreadtimer.h"


USING_XKERNEL_NAMESPACE

class CImeInputer;
#if IF_EDITTEXTVIEW_MULTI_EDIT
class CTextDocument;
#endif

class xEditTextView : public xTextView, public CImeTarget, xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xEditTextView)

public:
    xEditTextView();
    virtual ~xEditTextView();

    static void SetFunInstallIME(FUN_INSTALL_IME pFunInstallIME);

public:
    virtual void loadAttributes(xml_node& node);
    virtual bool SetOverLengthStyle(OVERLENGTH_TYPE eType);

#if IF_EDIT_TEXT_VIEW_SCROLL
    virtual void OnScrollFinish();
    void StartScroll(int nScrollCount = 1);
    void StopScroll();
#endif

public:
    virtual bool onPaintContent(xPainter& p);
    virtual bool onFocusEvent(bool bFocusIn);

    virtual bool OnChar(IMECHAR ch);
    virtual bool OnPreChar(IMECHAR ch);
    virtual bool OnPreInputConfirm();
    virtual bool OnImeChange(const yl::string& strImeName);

    virtual void onContentChanged();
    virtual bool onRectChanged(const chRect& rectOld);

    //设置文本并将光标移动至尾部
    virtual void SetInitText(const yl::string&);

    void OnFlushCursor(xThreadTimer* timer, LPVOID pExtraData);

    void slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/);
    void slotOnAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/);

#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
public:
    // softkey相关
    void SetDeleteSoftkeyIndex(int nIndex);
    void SetImeSoftkeyIndex(int nIndex);

    bool SetupDeleteSoftkey();
    bool SetupImeSoftkey();
    bool ReleteDeleteSoftkey();
    bool ReleteImeSoftkey();
#endif

#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    void SetKeyboardShow(bool bShow, bool bSwitch);
#endif

public:
    // 属性设置
    bool SetEnabled(bool bEnabled = true);
    bool SetPassword(bool bPassword = true);
    bool SetSuddenPassword(bool bPassword = true);

    bool SetPartitionPwdPrefix(yl::string strPrefix);
    bool SetPartitionPwdLength(size_t iLength);

    bool SetMaxLength(int iLength);
    int GetMaxLength();
    bool SetMaxByte(int iSize);

    virtual void SetMultiLineEnable(bool bMultiLineEnable);

#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    // 设置、获取提示内容
    yl::string GetHintText() const;
    void SetHintText(const yl::string& strHintText);
    virtual void SetTranText(const yl::string& strText);
    xFont GetHintFont();
#endif
    virtual void SetText(const yl::string& src);

#if IF_EDITTEXTVIEW_PRELINE_ENABLE
    void SetPreLineColor(const xColor& color);
#endif

public:
    //删除前一个字符
    virtual bool DeleteChar(bool bSignalNotify = true);
    virtual bool InsertChar(IMECHAR ch, bool bCheckLength = true);
    //直接输入字符，全键盘用（没有预输入状态）
    virtual bool DirectInserChar(IMECHAR ch, bool bCheckLength = true);

    bool MoveCursorNext();
    bool MoveCursorPrevious();

#if IF_EDITTEXTVIEW_SELECT_ENABLE
    bool DeleteSelectChar(bool bSignalNotify = true);

    //设置字符选取范围（nindex:起始位置，num:选取个数）
    void SetSelectRange(int nindex, int num);
    void SetSelectInvalid();
    void SetAllSelectStatus();

    void SetSelectTextBgColor(const xColor& color);
    void SetSelectTextColor(const xColor& color);

    void DrawSelcetText(xPainter& painter, chRect &rect, yl::string &text);
#endif
    void DrawNormalText(xPainter& painter, chRect &rect, yl::string &text);
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    void DrawHintText(xPainter& painter, chRect &rcText, yl::string &text);
#endif

#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
    void SetKeyPressedEnable(bool benable);
#endif

    virtual bool onKeyPressedEvent(int key);

#if IF_MULTI_TOUCH_ENABLE
    virtual bool onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);

    void MoveCursorToPos(const chPoint& ptPos);
#endif

    void SetCursorPosition(int nPosition);

    int GetCursorPos();

    int GetTextLineHeight();

    //获取当前字符的光标全局位置
    chRect GetCursorRectToGlobal();

protected:

#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    virtual bool onKeyReleasedEvent(int key);

    bool ProcessKeyboardPress(int key, bool& bResult);
#endif

    virtual yl::string GetStringToPaint();
    bool PasswordShowChar();
    bool PasswordHideChar();
    void OnPasswordHideCharTimer(xThreadTimer* timer, LPVOID pExtraData);
    bool DoDeleteAction();
    //移动光标
    int MoveCursor(int offset);

    //获取用于paint的ViewPort
    chRect GetViewPort();

    //获取当前字符的光标位置和输入法位置
    chRect GetCursorRect();
    chRect GetImeCharRect();

    //使用Ime输入字符
    void slotOnSoftkeypress(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void slotOnSoftKeyLonngPress(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    bool CheckLength(const yl::wstring& wStr);
#if IF_EDITTEXTVIEW_MULTI_EDIT
    int GetMoveStep(int eOp);
#endif

    void EnsureCursorVisiable();
    void UpdateCursorData();
    void CheckImeInputText();

    void OnEditEnable();
    void OnEditDisable();
    void SetCursorStatus(bool bShowCursor);

    void CheckRTLDirection();

    virtual void OnDirectionChange();

public:
    xSignal                signalTextConfirmChange;
    xSignal                signalTextPreChange;
#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    xSignal                 signalKeyboardShow;
#endif
#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
    xSignal                 signalImeChanged;
#endif

private:
    bool                    m_bEnabled;     //是否只读 Attr:enabled
    bool                    m_bPassword;     //是否为密码 Attr:password
    bool                    m_bSuddenPassword;  //密码框是否显示当前输入的字符 Attr:sudden_password

    xThreadTimer            m_tmHideChar;
    bool                    m_bShowingHideChar;

    bool                    m_bShowCursor;   //当前是否显示光标，用于光标闪烁
    int                     m_iCursorPos;    //光标位置
    xThreadTimer            m_tmCursor;      //光标闪烁定时器
    chRect                  m_LastViewPort;  //文本视口，用于超长时移动显示范围

    bool                    m_bPreInput;     //当前是否为Ime状态
#if IF_EDIT_TEXT_VIEW_SCROLL
    bool                    m_bInScroll;     //当前是否为滚动状态
#endif
    int                     m_nDeleteSoftkeyIndex;  //Attr:delete_softkey
    int                     m_nImeSoftkeyIndex;     //Attr:ime_softkey

    int                     m_nMaxLength;           //最大字符数
    int                     m_nMaxByte;             //最大字节数
#if IF_EDITTEXTVIEW_PRELINE_ENABLE
    xColor                  m_PreLineColor;         //预选底线颜色
#endif

#if IF_EDITTEXTVIEW_SELECT_ENABLE
    xColor                  m_SelectBgColor;        //全选状态字符背景
    xColor                  m_SelectTextColor;      //全选状态字体颜色

    int                     m_nSelectIndex;
    int                     m_nSelectNum;
#endif

    //部分password功能
    yl::wstring             m_strPartitionPwdPrefix;
    size_t                  m_iPartitionPwdLength;
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
    bool                    m_bKeyPressedEnable;
#endif
#if IF_EDITTEXTVIEW_MULTI_EDIT
    CTextDocument*          m_pTextDocument;

    friend class CTextDocument;
#endif
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    yl::string              m_strHintText;          // 内容为空时，显示的提示语
    int                     m_nHintWidth;           // 文字宽度
    int                     m_nHintFontSize;        // 提示语文本字体大小
    xColor                  m_clrHintText;        // 提示语文本字体颜色
#endif
#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    bool                    m_bKeyBoardShow;
#endif
    static FUN_INSTALL_IME  s_pFunInstallIME;
};

#endif

