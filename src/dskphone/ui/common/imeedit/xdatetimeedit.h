#ifndef __X_DATE_TIME_EDIT_H__
#define __X_DATE_TIME_EDIT_H__

#include <xwindow/xWindow.h>


class xEditTextView;
class XWINDOW_EXPORT xDateTimeBase : public xLinearLayout, public xSlotHandler
{
public:
    xDateTimeBase();

    virtual ~xDateTimeBase();

    virtual void loadAttributes(xml_node& node);

    virtual bool onKeyPressedEvent(int key);

    virtual void SetDateTime(int key) {}

public:
    void initChildEdit();

    void SetChildEdit();

    void SetFcous();

    yl::string IntToStr(int iTime);

protected:
    xEditTextView* GetCurrentEditTextView();

public:
    enum EDIT_TEXT_VIEW_TYPE
    {
        EDIT_YEAR_OR_HOUR       = 0,
        EDIT_MONTH_OR_MINUTE,
        EDIT_DAY_OR_SECOND,
        EDIT_NUM
    };

public:
    int             m_nCurEidtText;
    xEditTextView*  m_arrEidtText[EDIT_NUM];
    xTextView       *m_plabel;
    xTextView       *m_plabel_2;

    xColor          m_textColor;
    xFont           m_font;
    xBrush          m_textEditBg;
};

class XWINDOW_EXPORT xDateEdit : public xDateTimeBase
{
public:
    xDateEdit();

    virtual ~xDateEdit();

    virtual void SetDateTime(int key);

    void SetYear(const char* strYear);
    void SetMonth(const char* strMonth);
    void SetDay(const char* strDay);

    yl::string GetYear();
    yl::string GetMonth();
    yl::string GetDay();
};

class XWINDOW_EXPORT xTimeEdit : public xDateTimeBase
{
public:
    xTimeEdit();

    virtual ~xTimeEdit();

    virtual void SetDateTime(int key);

    void SetHour(const yl::string& strHour);
    void SetMinute(const yl::string& strMinute);
    void SetSecond(const yl::string& strSecond);

    yl::string GetHour();
    yl::string GetMinute();
    yl::string GetSecond();
};

#endif
