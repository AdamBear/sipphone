#include "xdatetimeedit.h"
#include "commonapi/stringhelperapi.h"
#include "ximeedit.h"
#include "keymap.h"
#include <xwindow/modviewdump.h>


xDateTimeBase::xDateTimeBase()
{
    m_nCurEidtText = EDIT_YEAR_OR_HOUR;

    initChildEdit();
}

xDateTimeBase::~xDateTimeBase()
{
}

void xDateTimeBase::initChildEdit()
{
    xEditTextView* pEditYearHour = new xEditTextView();

    if (NULL != pEditYearHour)
    {
        pEditYearHour->layout_width = 50;
        pEditYearHour->AddSupportIme((LPCTSTR)("123"));
        pEditYearHour->SetGravity(gravity_center);
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
        pEditYearHour->SetKeyPressedEnable(false);
#endif

        m_arrEidtText[EDIT_YEAR_OR_HOUR] = pEditYearHour;
    }

    xEditTextView* pEditMonthMinute = new xEditTextView();

    if (NULL != pEditMonthMinute)
    {
        pEditMonthMinute->layout_width = 50;
        pEditMonthMinute->AddSupportIme((LPCTSTR)("123"));
        pEditMonthMinute->SetGravity(gravity_center);
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
        pEditMonthMinute->SetKeyPressedEnable(false);
#endif

        m_arrEidtText[EDIT_MONTH_OR_MINUTE] = pEditMonthMinute;
    }

    xEditTextView* pEditDaySecond = new xEditTextView();

    if (NULL != pEditDaySecond)
    {
        pEditDaySecond->layout_width = 50;
        pEditDaySecond->AddSupportIme((LPCTSTR)("123"));
        pEditDaySecond->SetGravity(gravity_center);
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
        pEditDaySecond->SetKeyPressedEnable(false);
#endif

        m_arrEidtText[EDIT_DAY_OR_SECOND] = pEditDaySecond;
    }

    m_plabel = new xTextView();
    m_plabel_2 = new xTextView();
    if (NULL != m_plabel && NULL != m_plabel_2)
    {
        m_plabel->layout_width = 15;
        m_plabel_2->layout_width = 15;
        m_plabel->focusable = xView::bool_false;
        m_plabel_2->focusable = xView::bool_false;
        m_plabel->SetGravity(gravity_center);
        m_plabel_2->SetGravity(gravity_center);
    }

    addChildView(pEditYearHour);
    addChildView(m_plabel);
    addChildView(pEditMonthMinute);
    addChildView(m_plabel_2);
    addChildView(pEditDaySecond);
}

void xDateTimeBase::SetChildEdit()
{
    xEditTextView* pEditYearHour = m_arrEidtText[EDIT_YEAR_OR_HOUR];
    xEditTextView* pEditMonthMinute = m_arrEidtText[EDIT_MONTH_OR_MINUTE];
    xEditTextView* pEditDaySecond = m_arrEidtText[EDIT_DAY_OR_SECOND];

    if (NULL == pEditYearHour || NULL == pEditMonthMinute || NULL == pEditDaySecond)
    {
        return;
    }

    pEditYearHour->SetTextColor(m_textColor);
    pEditMonthMinute->SetTextColor(m_textColor);
    pEditDaySecond->SetTextColor(m_textColor);

    pEditYearHour->SetFont(m_font);
    pEditMonthMinute->SetFont(m_font);
    pEditDaySecond->SetFont(m_font);

    pEditYearHour->background = m_textEditBg;
    pEditMonthMinute->background = m_textEditBg;
    pEditDaySecond->background = m_textEditBg;

    if (layout_width > 30)
    {
        int nWidth = (layout_width - 30) / 3;
        pEditYearHour->layout_width = nWidth;
        pEditMonthMinute->layout_width = nWidth;
        pEditDaySecond->layout_width = nWidth;
    }
}

yl::string xDateTimeBase::IntToStr(int iTime)
{
    iTime = iTime % 100;
    char strTime[3];

    if (iTime < 10)
    {
        strTime[0] = '0';
        strTime[1] = iTime + '0';
    }
    else
    {
        strTime[1] = iTime % 10  + '0';
        strTime[0] = (iTime - iTime % 10) / 10 + '0';
    }

    strTime[2] = '\0';

    return strTime;
}

xEditTextView* xDateTimeBase::GetCurrentEditTextView()
{
    int nEditCount = sizeof(m_arrEidtText) / sizeof(xEditTextView*);

    if (m_nCurEidtText < 0 || m_nCurEidtText >= nEditCount)
    {
        return NULL;
    }

    return m_arrEidtText[m_nCurEidtText];
}

void xDateTimeBase::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);

    PopupAndroidColor(node, XWINDOW_NAME_TEXT_COLOR, m_textColor);
    PopupAndroidBrush(node, XWINDOW_NAME_TEXT_EDIT_BG, m_textEditBg);

    int textSize = -1;

    PopupAndroidPixel(node, XWINDOW_NAME_TEXT_SIZE, textSize);

    if (textSize != -1)
    {
        m_font.setPointSize(textSize);
    }

    SetChildEdit();
}

void xDateTimeBase::SetFcous()
{
    m_nCurEidtText = EDIT_YEAR_OR_HOUR;
    xEditTextView* pEditYearHour = m_arrEidtText[m_nCurEidtText];

    if (NULL != pEditYearHour)
    {
        pEditYearHour->setFocus();
    }
}

bool xDateTimeBase::onKeyPressedEvent(int key)
{
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
    if (key == PHONE_KEY_LEFT)
    {
        m_nCurEidtText--;
        if (m_nCurEidtText < EDIT_YEAR_OR_HOUR
                || m_nCurEidtText > EDIT_DAY_OR_SECOND)
        {
            m_nCurEidtText = EDIT_DAY_OR_SECOND;
        }

        xEditTextView* pCurEditText = GetCurrentEditTextView();

        if (NULL != pCurEditText)
        {
            pCurEditText->setFocus();
        }
        return true;
    }
    else if (key == PHONE_KEY_RIGHT)
    {
        m_nCurEidtText++;
        if (m_nCurEidtText > EDIT_DAY_OR_SECOND
                || m_nCurEidtText < EDIT_YEAR_OR_HOUR)
        {
            m_nCurEidtText = EDIT_YEAR_OR_HOUR;
        }

        xEditTextView* pCurEditText = GetCurrentEditTextView();

        if (NULL != pCurEditText)
        {
            pCurEditText->setFocus();
        }
        return true;
    }

    if (PHONE_KEY_0 <= key && PHONE_KEY_9 >= key)
    {
        SetDateTime(key);
        return true;
    }
#endif
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xDateEdit);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xDateEdit
DECLARE_VIEW_DUMP_BEGIN(xDateEdit)
DECLARE_VIEW_DUMP_PROPERTY_METHOD(year, GetYear())
DECLARE_VIEW_DUMP_PROPERTY_METHOD(month, GetMonth())
DECLARE_VIEW_DUMP_PROPERTY_METHOD(day, GetDay())
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xDateEdit, xLinearLayout)
IMPLEMENT_VIEW_DUMP_PROPERTY(year)
IMPLEMENT_VIEW_DUMP_PROPERTY(month)
IMPLEMENT_VIEW_DUMP_PROPERTY(day)
IMPLEMENT_VIEW_DUMP_END(xDateEdit)

xDateEdit::xDateEdit()
{
    REGISTER_VIEW_DUMP_CLASS(xDateEdit)

    if (NULL != m_plabel && NULL != m_plabel_2)
    {
        m_plabel->SetText("-");
        m_plabel_2->SetText("-");
    }
}

xDateEdit::~xDateEdit()
{
}

void xDateEdit::SetYear(const char* strYear)
{
    xEditTextView* pEditYear = m_arrEidtText[EDIT_YEAR_OR_HOUR];

    if (NULL != pEditYear)
    {
        pEditYear->SetInitText(strYear);
    }
}

void xDateEdit::SetMonth(const char* strMonth)
{
    xEditTextView* pEditMonth = m_arrEidtText[EDIT_MONTH_OR_MINUTE];

    if (NULL != pEditMonth)
    {
        pEditMonth->SetInitText(strMonth);
    }
}

void xDateEdit::SetDay(const char* strDay)
{
    xEditTextView* pEditDay = m_arrEidtText[EDIT_DAY_OR_SECOND];

    if (NULL != pEditDay)
    {
        pEditDay->SetInitText(strDay);
    }
}

yl::string xDateEdit::GetYear()
{
    xEditTextView* pEditYear = m_arrEidtText[EDIT_YEAR_OR_HOUR];

    if (NULL != pEditYear)
    {
        return pEditYear->GetText();
    }
    else
    {
        return "";
    }
}

yl::string xDateEdit::GetMonth()
{
    xEditTextView* pEditMonth = m_arrEidtText[EDIT_MONTH_OR_MINUTE];

    if (NULL != pEditMonth)
    {
        return pEditMonth->GetText();
    }
    else
    {
        return "";
    }
}

yl::string xDateEdit::GetDay()
{
    xEditTextView* pEditDay = m_arrEidtText[EDIT_DAY_OR_SECOND];

    if (NULL != pEditDay)
    {
        return pEditDay->GetText();
    }
    else
    {
        return "";
    }
}

void xDateEdit::SetDateTime(int key)
{
    xEditTextView* pCurEditTextView = GetCurrentEditTextView();

    if (NULL == pCurEditTextView)
    {
        return ;
    }

    int arrMonthDay[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };
    int iFocusValue = atoi(pCurEditTextView->GetText().c_str());
    bool bTimeError = false;

    if (iFocusValue > 10)
    {
        iFocusValue = iFocusValue % 10;
        iFocusValue = iFocusValue * 10 + (key - PHONE_KEY_0);
    }
    else if (iFocusValue == 10)
    {
        iFocusValue =  key - PHONE_KEY_0;
    }
    else
    {
        iFocusValue = iFocusValue * 10 + (key - PHONE_KEY_0);
    }

    switch (m_nCurEidtText)
    {
    case EDIT_YEAR_OR_HOUR:
        {
            bTimeError = false;
        }
        break;
    case EDIT_MONTH_OR_MINUTE:
        {
            if (iFocusValue < 1 || iFocusValue > 12)
            {
                bTimeError = true;
            }
        }
        break;
    case EDIT_DAY_OR_SECOND:
        {
            xEditTextView* pEditYearOrHour = m_arrEidtText[EDIT_YEAR_OR_HOUR];
            xEditTextView* pEditMonthMinute = m_arrEidtText[EDIT_MONTH_OR_MINUTE];
            int nMonthDays = 0;
            int nMonth = 0;

            if (NULL != pEditMonthMinute)
            {
                nMonth = atoi(pEditMonthMinute->GetText().c_str());

                if (nMonth < 0 || nMonth >= 13)
                {
                    nMonth = 0;
                }
            }

            if (NULL != pEditYearOrHour && 0 == atoi(pEditYearOrHour->GetText().c_str()) % 4)
            {
                nMonthDays = arrMonthDay[1][nMonth];
            }
            else
            {
                nMonthDays = arrMonthDay[0][nMonth];
            }

            if (iFocusValue < 1 || iFocusValue > nMonthDays)
            {
                bTimeError = true;
            }
        }
        break;
    default:
        break;
    }

    if (bTimeError)
    {
        pCurEditTextView->SetInitText(IntToStr(key - PHONE_KEY_0).c_str());
    }
    else
    {
        pCurEditTextView->SetInitText(IntToStr(iFocusValue).c_str());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xTimeEdit);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xTimeEdit
DECLARE_VIEW_DUMP_BEGIN(xTimeEdit)
DECLARE_VIEW_DUMP_PROPERTY_METHOD(hour, GetHour())
DECLARE_VIEW_DUMP_PROPERTY_METHOD(minute, GetMinute())
DECLARE_VIEW_DUMP_PROPERTY_METHOD(second, GetSecond())
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xTimeEdit, xLinearLayout)
IMPLEMENT_VIEW_DUMP_PROPERTY(hour)
IMPLEMENT_VIEW_DUMP_PROPERTY(minute)
IMPLEMENT_VIEW_DUMP_PROPERTY(second)
IMPLEMENT_VIEW_DUMP_END(xTimeEdit)

xTimeEdit::xTimeEdit()
{
    REGISTER_VIEW_DUMP_CLASS(xTimeEdit)

    if (NULL != m_plabel && NULL != m_plabel_2)
    {
        m_plabel->SetText(":");
        m_plabel_2->SetText(":");
    }
}
xTimeEdit::~xTimeEdit()
{
}

void xTimeEdit::SetHour(const yl::string& strHour)
{
    xEditTextView* pEditHour = m_arrEidtText[EDIT_YEAR_OR_HOUR];

    if (NULL != pEditHour)
    {
        pEditHour->SetInitText(strHour);
    }
}

void xTimeEdit::SetMinute(const yl::string& strMinute)
{
    xEditTextView* pEditMinute = m_arrEidtText[EDIT_MONTH_OR_MINUTE];

    if (NULL != pEditMinute)
    {
        pEditMinute->SetInitText(strMinute);
    }
}

void xTimeEdit::SetSecond(const yl::string& strSecond)
{
    xEditTextView* pEditSecond = m_arrEidtText[EDIT_DAY_OR_SECOND];

    if (NULL != pEditSecond)
    {
        pEditSecond->SetInitText(strSecond);
    }
}

yl::string xTimeEdit::GetHour()
{
    xEditTextView* pEditHour = m_arrEidtText[EDIT_YEAR_OR_HOUR];

    if (NULL != pEditHour)
    {
        return pEditHour->GetText();
    }
    else
    {
        return "";
    }
}

yl::string xTimeEdit::GetMinute()
{
    xEditTextView* pEditMinute = m_arrEidtText[EDIT_MONTH_OR_MINUTE];

    if (NULL != pEditMinute)
    {
        return pEditMinute->GetText();
    }
    else
    {
        return "";
    }
}

yl::string xTimeEdit::GetSecond()
{
    xEditTextView* pEditSecond = m_arrEidtText[EDIT_DAY_OR_SECOND];

    if (NULL != pEditSecond)
    {
        return pEditSecond->GetText();
    }
    else
    {
        return "";
    }
}

void xTimeEdit::SetDateTime(int key)
{
    xEditTextView* pCurEditTextView = GetCurrentEditTextView();

    if (NULL == pCurEditTextView)
    {
        return ;
    }

    bool bTimeError = false;
    int iFocusValue = atoi(pCurEditTextView->GetText().c_str());

    if (iFocusValue > 10)
    {
        iFocusValue = iFocusValue % 10;
        iFocusValue = iFocusValue * 10 + (key - PHONE_KEY_0);
    }
    else if (iFocusValue == 10)
    {
        iFocusValue =  key - PHONE_KEY_0;
    }
    else
    {
        iFocusValue = iFocusValue * 10 + (key - PHONE_KEY_0);
    }

    switch (m_nCurEidtText)
    {
    case EDIT_YEAR_OR_HOUR:
        {
            if (iFocusValue < 1 || iFocusValue > 23)
            {
                bTimeError = true;
            }
        }
        break;
    case EDIT_MONTH_OR_MINUTE:
        {
            if (iFocusValue < 1 || iFocusValue > 59)
            {
                bTimeError = true;
            }
        }
        break;
    case EDIT_DAY_OR_SECOND:
        {
            if (iFocusValue < 1 || iFocusValue > 59)
            {
                bTimeError = true;
            }
        }
        break;
    default:
        break;
    }

    if (bTimeError)
    {
        pCurEditTextView->SetInitText(IntToStr(key - PHONE_KEY_0).c_str());
    }
    else
    {
        pCurEditTextView->SetInitText(IntToStr(iFocusValue).c_str());
    }
}
