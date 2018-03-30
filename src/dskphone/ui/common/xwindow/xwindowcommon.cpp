#include "xwindowcommon.h"
#include "xWindowManager.h"
#if IF_XWINDOW_DEBUG
#include "devicelib/networkserver.h"
#endif


const char* XWINDOW_ENUM_TRUE = "true";
const char* XWINDOW_ENUM_FALSE = "false";
const char* XWINDOW_ENUM_AUTO = "auto";
const char* XWINDOW_ENUM_WRAP_CONTENT = "wrap_content";
const char* XWINDOW_ENUM_MATCH_PARENT = "match_parent";
const char* XWINDOW_ENUM_FILL_PARENT = "fill_parent";
const char* XWINDOW_ENUM_TOP = "top";
const char* XWINDOW_ENUM_BOTTOM = "bottom";
const char* XWINDOW_ENUM_LEFT = "left";
const char* XWINDOW_ENUM_RIGHT = "right";
const char* XWINDOW_ENUM_CENTER_VERTICAL = "center_vertical";
const char* XWINDOW_ENUM_CENTER_HORIZONTAL = "center_horizontal";
const char* XWINDOW_ENUM_CENTER = "center";
const char* XWINDOW_ENUM_VISIBLE = "visible";
const char* XWINDOW_ENUM_INVISIBLE = "invisible";
const char* XWINDOW_ENUM_GONE = "gone";
const char* XWINDOW_ENUM_VERTICAL = "vertical";
const char* XWINDOW_ENUM_HORIZONTAL = "horizontal";
const char* XWINDOW_ENUM_LEFT_TOP = "left_top";
const char* XWINDOW_ENUM_RIGHT_TOP = "right_top";
const char* XWINDOW_ENUM_LEFT_BOTTOM = "left_bottom";
const char* XWINDOW_ENUM_RIGHT_BOTTOM = "right_bottom";
const char* XWINDOW_ENUM_ALL = "all";

const char* XWINDOW_ENUM_DIRECT_CUT = "direct_cut";
const char* XWINDOW_ENUM_CUT_WITH_ELLIPSIS = "cut_with_ellipsis";
const char* XWINDOW_ENUM_SCROLL = "scroll";
const char* XWINDOW_ENUM_AUTO_RETURN = "auto_return";
const char* XWINDOW_ENUM_VSCROLL = "vscroll";

const char* XWINDOW_NAME_LAYOUT_MARGIN = "android:layout_margin";
const char* XWINDOW_NAME_LAYOUT_MARGIN_LEFT = "android:layout_marginLeft";
const char* XWINDOW_NAME_LAYOUT_MARGIN_RIGHT = "android:layout_marginRight";
const char* XWINDOW_NAME_LAYOUT_MARGIN_TOP = "android:layout_marginTop";
const char* XWINDOW_NAME_LAYOUT_MARGIN_BOTTOM = "android:layout_marginBottom";
const char* XWINDOW_NAME_LAYOUT_HEIGHT = "android:layout_height";
const char* XWINDOW_NAME_LAYOUT_WIDTH = "android:layout_width";
const char* XWINDOW_NAME_PADDING = "android:padding";
const char* XWINDOW_NAME_PADDING_LEFT = "android:paddingLeft";
const char* XWINDOW_NAME_PADDING_RIGHT = "android:paddingRight";
const char* XWINDOW_NAME_PADDING_TOP = "android:paddingTop";
const char* XWINDOW_NAME_PADDING_BOTTOM = "android:paddingBottom";
const char* XWINDOW_NAME_ID = "android:id";
const char* XWINDOW_NAME_BACKGROUND = "android:background";
const char* XWINDOW_NAME_STYLE = "android:style";
const char* XWINDOW_NAME_FOCUSABLE = "android:focusable";
const char* XWINDOW_NAME_FOCUS_PRESENT = "android:focuspresent";
const char* XWINDOW_NAME_CLICKABLE = "android:clickable";
const char* XWINDOW_NAME_FOCUS_ON_CLICK = "android:focusOnClick";
const char* XWINDOW_NAME_OVERLAP = "android:overlap";
const char* XWINDOW_NAME_DISABLE = "android:disable";
#if IF_MULTI_TOUCH_ENABLE
const char* XWINDOW_NAME_TOUCHABLE = "android:touchable";
#endif
#if IF_VIEW_MASK
const char* XWINDOW_NAME_MASK = "android:mask";
#endif
const char* XWINDOW_NAME_VISIBLITY = "android:visibility";
#if IF_MOVE_FOCUS_BY_KEY
const char* XWINDOW_NAME_NEXT_FOCUS_UP = "android:nextFocusUp";
const char* XWINDOW_NAME_NEXT_FOCUS_DOWN = "android:nextFocusDown";
const char* XWINDOW_NAME_NEXT_FOCUS_LEFT = "android:nextFocusLeft";
const char* XWINDOW_NAME_NEXT_FOCUS_RIGHT = "android:nextFocusRight";
#if IF_PC_ENABLE
const char* XWINDOW_NAME_NEXT_FOCUS_PREVIOUS = "android:nextFocusPrevious";
const char* XWINDOW_NAME_NEXT_FOUCS_NEXT = "android:nextFocusNext";
#endif
#endif
#if IF_VIEW_USER_ACTION
const char* XWINDOW_NAME_ON_CLICK = "android:onClick";
#endif
#if IF_VIEW_RENDER
const char* XWINDOW_NAME_RENDERER = "android:renderer";
#endif
const char* XWINDOW_NAME_SRC = "android:src";
#if IF_INVERT_EFFECT
const char* XWINDOW_NAME_INVERT = "android:invert";
#endif
#if IF_IMAGE_VIEW_BORDER
const char* XWINDOW_NAME_BORDER_WIDTH = "android:borderWidth";
const char* XWINDOW_NAME_BORDER_COLOR = "android:borderColor";
#endif
const char* XWINDOW_NAME_GRAVITY = "android:gravity";
const char* XWINDOW_NAME_ALIGN = "android:align";
const char* XWINDOW_NAME_TEXT = "android:text";
const char* XWINDOW_NAME_TEXT_COLOR = "android:textColor";
const char* XWINDOW_NAME_TEXT_SIZE = "android:textSize";
const char* XWINDOW_NAME_TEXT_BOLD = "android:textBold";
const char* XWINDOW_NAME_MULTILINE = "android:multiline";
#if IF_BUTTON_SUPPORT_DISABLE_STATE
const char* XWINDOW_NAME_COLOR_DISABLE = "android:colorDisable";
const char* XWINDOW_NAME_PIXMAP_DISABLE = "android:pixDisable";
#endif
#if IF_BUTTON_SUPPORT_FOCUS_STATE
const char* XWINDOW_NAME_COLOR_FOCUS = "android:colorFocus";
const char* XWINDOW_NAME_PIXMAP_FOCUS = "android:pixFocus";
#endif
#if IF_BUTTON_SUPPORT_PRESS_STATE
const char* XWINDOW_NAME_COLOR_PRESS = "android:colorPress";
const char* XWINDOW_NAME_PIXMAP_PRESS = "android:pixPress";
#endif
const char* XWINDOW_NAME_ITEM_STYLE = "android:item_style";
const char* XWINDOW_NAME_FOCUS_ITEM_BG = "android:focusItemBG";
const char* XWINDOW_NAME_PAGE = "android:page";
const char* XWINDOW_NAME_BG = "android:bg";
const char* XWINDOW_NAME_BLOCK = "android:block";
const char* XWINDOW_NAME_RANGE = "android:range";
const char* XWINDOW_NAME_BAR = "android:bar";
const char* XWINDOW_NAME_SLIDE_ICON = "android:slide_icon";
const char* XWINDOW_NAME_RC_ICON = "android:rcIcon";
const char* XWINDOW_NAME_ON = "android:on";
const char* XWINDOW_NAME_OFF = "android:off";
const char* XWINDOW_NAME_RC_TEXT = "android:rcText";
const char* XWINDOW_NAME_LAYOUT_GRAVITY = "android:layout_gravity";
const char* XWINDOW_NAME_LAYOUT_WEIGHT = "android:layout_weight";
const char* XWINDOW_NAME_ORIENTATION = "android:orientation";
const char* XWINDOW_NAME_CONTENT = "android:content";
const char* XWINDOW_NAME_SCROLL_BAR = "android:scroll_bar";
const char* XWINDOW_NAME_THUMB = "android:thumb";
const char* XWINDOW_NAME_SCALE_LTWH = "android:scale_ltwh";
const char* XWINDOW_NAME_SCALE_HEIGHT = "android:scale_height";
const char* XWINDOW_NAME_SCALE_WIDTH = "android:scale_width";
const char* XWINDOW_NAME_RECT_CORNER_SIZE = "android:rect_corner_size";
const char* XWINDOW_NAME_RECT_COLOR = "android:rect_color";
const char* XWINDOW_NAME_RECT_CORNER_TYPE = "android:rect_corner_type";
const char* XWINDOW_NAME_ACTIVABLE = "android:activable";
const char* XWINDOW_NAME_LOGIC_LTWH = "android:logic_ltwh";
const char* XWINDOW_NAME_LAYOUT = "android:layout";
const char* XWINDOW_NAME_COLUMN = "android:column";
const char* XWINDOW_NAME_COLUMN_SPACE = "android:column_space";
const char* XWINDOW_NAME_BOUND_LINE_TYPE = "android:bound_line_type";
const char* XWINDOW_NAME_LIST_STYLE = "android:list_style";
#if IF_XLISTVIEW_SUPPORT_SLIDE
const char* XWINDOW_NAME_SLIDABLE = "android:slidable";
#endif
const char* XWINDOW_NAME_SCROLL_TYPE = "android:scroll_type";
const char* XWINDOW_NAME_FOCUS_TYPE = "android:focus_type";
const char* XWINDOW_NAME_PAGE_ITEM_COUNT = "android:page_item_count";
const char* XWINDOW_NAME_CONTENT_HEIGHT = "android:content_height";
const char* XWINDOW_NAME_PAGE_HEIGHT = "android:page_height";
const char* XWINDOW_NAME_IM_TOP = "android:im_top";
const char* XWINDOW_NAME_IM_LEFT = "android:im_left";
const char* XWINDOW_NAME_IM_RIGHT = "android:im_right";
const char* XWINDOW_NAME_IM_BOTTOM = "android:im_bottom";
const char* XWINDOW_NAME_ITEM_WIDTH = "android:item_width";
const char* XWINDOW_NAME_ITEM_HEIGHT = "android:item_height";
const char* XWINDOW_NAME_ITEM_SPACE = "android:item_space";
const char* XWINDOW_NAME_SCM_TOP = "android:scm_top";
const char* XWINDOW_NAME_SCM_BOTTOM = "android:scm_bottom";
const char* XWINDOW_NAME_SCM_RIGHT = "android:scm_right";
const char* XWINDOW_NAME_FIX_POS = "android:fix_pos";
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
const char* XWINDOW_NAME_SPACE_LINE_COLOR = "android:space_line_color";
const char* XWINDOW_NAME_SPACE_LINE_STYLE = "android:space_line_style";
const char* XWINDOW_NAME_SPACE_LINE_IGNORE_END = "android:space_line_ignore_end";
#endif
#if IF_XLISTVIEW_SUPPORT_GROUP
const char* XWINDOW_NAME_ITEMS_STYLE = "android:items_style";
const char* XWINDOW_NAME_SECTION_COUNT = "android:section_count";
const char* XWINDOW_NAME_SECTION_STYLE = "android:section_style";
const char* XWINDOW_NAME_SECTION_TITLE_BG = "android:section_title_bg";
const char* XWINDOW_NAME_SECTION_TEXT_COLOR = "android:section_text_color";
const char* XWINDOW_NAME_SECTION_TEXT_SIZE = "android:section_text_size";
const char* XWINDOW_NAME_SECTION_TEXT_BOLD = "android:section_text_bold";
const char* XWINDOW_NAME_SECTION_TEXT_ALIGN = "android:section_text_align";
const char* XWINDOW_NAME_SECTION_TEXT_MARGIN = "android:section_text_margin";
#endif
#if IF_XLISTVIEW_SUPPORT_TITLE
const char* XWINDOW_NAME_TITLE_SPACE = "android:title_space";
const char* XWINDOW_NAME_TITLE_LINE_TYPE = "android:title_line_type";
const char* XWINDOW_NAME_TITLE_LINE_POS_TYPE = "android:title_line_pos_type";
#endif
const char* XWINDOW_NAME_ITEM = "android:item";
const char* XWINDOW_NAME_MAX_LENGTH = "android:maxLength";
const char* XWINDOW_NAME_SHOW_TIME = "android:showTime";
const char* XWINDOW_NAME_HIDE_TIME = "android:hideTime";
const char* XWINDOW_NAME_INTERVAL = "android:interval";
const char* XWINDOW_NAME_PICS = "android:pics";
const char* XWINDOW_NAME_TEXT_EDIT_BG = "android:textEditBg";
const char* XWINDOW_NAME_PREFERENCE = "android:preference";
const char* XWINDOW_NAME_VALUE = "android:value";
const char* XWINDOW_NAME_THUMB_NORMAL = "android:thumb_normal";
const char* XWINDOW_NAME_THUMB_SELECTED = "android:thumb_selected";
const char* XWINDOW_NAME_MIN = "android:min";
const char* XWINDOW_NAME_MAX = "android:max";
const char* XWINDOW_NAME_SINGLE_STEP = "android:singleStep";
const char* XWINDOW_NAME_PAGE_STEP = "android:pageStep";
const char* XWINDOW_NAME_POPUP_GRAVITY = "android:popupGravity";;
const char* XWINDOW_NAME_NORMAL = "android:normal";
const char* XWINDOW_NAME_HOVER = "android:hover";
const char* XWINDOW_NAME_PRESSED = "android:pressed";
const char* XWINDOW_NAME_HIGH_LIGHT = "android:highLight";
const char* XWINDOW_NAME_COUNT = "android:count";
const char* XWINDOW_NAME_FLUSH_TIME = "android:flushTime";
const char* XWINDOW_NAME_IMAGE_NAME = "android:imageName";
const char* XWINDOW_NAME_DISABLED = "android:disabled";
const char* XWINDOW_NAME_VISIBLE = "android:visbile";
const char* XWINDOW_NAME_FILE = "android:file";
const char* XWINDOW_NAME_SIZE = "android:size";
const char* XWINDOW_NAME_COLOR = "android:color";
const char* XWINDOW_NAME_FAMILY = "android:family";
const char* XWINDOW_NAME_PRESENT = "android:present";
const char* XWINDOW_NAME_TITLE = "android:title";
const char* XWINDOW_NAME_ON_LONG_CLICK = "android:onLongClick";
const char* XWINDOW_NAME_WRAPLIMITED = "android:wraplimited";

const char* XWINDOW_NAME_OVER_LENGTH_STYLE = "android:overlength_style";
const char* XWINDOW_NAME_ACTIVE_FONTS = "android:active_fonts";
const char* XWINDOW_NAME_SCROLL_DIRECTION = "android:scroll_direction";
const char* XWINDOW_NAME_SCROLL_TICK = "android:scroll_tick";
const char* XWINDOW_NAME_SCROLL_SHIFT = "android:scroll_shift";
const char* XWINDOW_NAME_ROUND_DURATION = "android:scroll_round_duration";
const char* XWINDOW_NAME_LINE_DURATION = "android:scroll_line_duration";

const char* XWINDOW_TAG_NAME_VIEW_STUB = "ViewStub";
const char* XWINDOW_TAG_NAME_VIEW = "view";
const char* XWINDOW_TAG_NAME_DATA = "data";
const char* XWINDOW_TAG_NAME_GROUP = "group";


xTextTranslate::xTextTranslate()
{

}

xTextTranslate::~xTextTranslate()
{
    g_WorkStation.ReleaseTextTran(this);
}

#if IF_XWINDOW_DEBUG
void XWIN_TRACE_TIME(const char* lpszFmt, ...)
{
#ifndef _WIN32
    struct timeval tData;

    gettimeofday(&tData, NULL);

    ulong ulCostTimes = tData.tv_sec * 1000 + tData.tv_usec / 1000;
    va_list argList;

    va_start(argList, lpszFmt);
    fprintf(stdout, "[Time:(%.8u)] ", ulCostTimes);
    vfprintf(stdout, lpszFmt, argList);
    fprintf(stdout, "\n");
    va_end(argList);
#endif
}
#endif

BEGIN_XWINDOW_NAMESPACE
//是否为空白字符
bool IsWhiteSpace(int ch)
{
    return (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
}

//跳过空白字符，返回第一个正常字符的字符串指针
LPCSTR SkipWhiteSpace(LPCSTR lpszText)
{
    while (NS_xWindow::IsWhiteSpace(*lpszText))
    {
        lpszText ++;
    }
    return lpszText;
}

//根据lpszToken将lpszText截取，将所截取到的第一段字符串赋值给strText
int PeekPhraseByCharTokens(LPCSTR& lpszText, yl::string& strText, LPCSTR lpszToken)
{
    assert(lpszText != NULL && lpszToken != NULL);

    LPCSTR lpszWordBegin = NS_xWindow::SkipWhiteSpace(lpszText);
    if (*lpszWordBegin == '\0')
    {
        lpszText = lpszWordBegin;
        return -1;
    }

    LPCSTR lpszWordEnding = lpszWordBegin - 1;
    for (LPCSTR lpszWord = lpszWordBegin; ; lpszWord = (*lpszWord == '\0') ? lpszWord : lpszWord + 1)
    {
        LPCSTR lpszTestToken;
        for (lpszTestToken = lpszToken; *lpszTestToken != '\0' ; lpszTestToken ++)
        {
            if (*lpszWord == *lpszTestToken)
            {
                strText = strText.assign(lpszWordBegin, lpszWordEnding - lpszWordBegin + 1);
                lpszText = *lpszTestToken == '\0' ? lpszWord : lpszWord + 1;
                return lpszTestToken - lpszToken;
            }
        }
        if (*lpszWord == '\0')
        {
            strText = strText.assign(lpszWordBegin, lpszWordEnding - lpszWordBegin + 1);
            lpszText = lpszWord;
            return lpszTestToken - lpszToken;
        }

        // doesn't match now save the last word position
        if (!NS_xWindow::IsWhiteSpace(*lpszWord))
        {
            lpszWordEnding = lpszWord;
        }
    }
    assert(false);
    return -1;
}

LPCSTR PeekContentFromQuotaString(LPCSTR lpszParamText, yl::string& strContent)
{
    assert(lpszParamText[-1] == '"');
    strContent.clear();
    for (; *lpszParamText != '\0'; lpszParamText ++)
    {
        if (*lpszParamText == '"')
        {
            return lpszParamText + 1;
        }
        if (*lpszParamText == '\\')
        {
            lpszParamText ++;
            switch (*lpszParamText)
            {
            case 'n':
                strContent += '\n';
                break;
            case 'r':
                strContent += '\r';
                break;
            case 't':
                strContent += '\t';
                break;
            case 'v':
                strContent += '\v';
                break;
            case 'f':
                strContent += '\f';
                break;
            default:
                strContent += *lpszParamText;
            }
        }
        else
        {
            strContent += *lpszParamText;
        }
    }
    return lpszParamText;
}
END_XWINDOW_NAMESPACE

void xWindowUI_Init()
{
    xTextView::InitOverLengthEffect();
#if !IF_ONLY_USE_GRADATION
    xPixmap::InitImageFormatHandle();
#endif
}
