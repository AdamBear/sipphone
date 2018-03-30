#include "uicommon.h"
#include "uikernel/languagehelper.h"
#include "translateiddefine.h"
#include "baseui/t4xpicpath.h"

Qt::AlignmentFlag TypeToAlignmentFlag(int eAlignType)
{
    int qtAlign = 0x0000;
    //横向
    if (eAlignType & DT_CENTER)
    {
        qtAlign = qtAlign | Qt::AlignHCenter;
    }
    else if (eAlignType & DT_RIGHT)
    {
        qtAlign = qtAlign | Qt::AlignRight;
    }
    else
    {
        qtAlign = qtAlign | Qt::AlignLeft;
    }

    //纵向
    if (eAlignType & DT_VCENTER)
    {
        qtAlign = qtAlign | Qt::AlignVCenter;
    }
    else if (eAlignType & DT_TOP)
    {
        qtAlign = qtAlign | Qt::AlignTop;
    }
    else
    {
        qtAlign = qtAlign | Qt::AlignBottom;
    }

    return (Qt::AlignmentFlag)qtAlign;
}

int TypeToFontPointSize(X_FONT_TYPE eFontSize)
{
    switch (eFontSize)
    {
    case X_FONT_SMALL:
        return 12;
    case X_FONT_NORMAL:
        return 18;
    case X_FONT_DOUBLE:
        return 24;
    case X_FONT_LARGE:
        return 28;
    default:
        break;
    }

    return 18;
}

QColor TypeToColor(TEXT_FONT_COLOR eColorType)
{
    // 根据类型返回相应的颜色
    switch (eColorType)
    {
    case TEXT_FONT_COLOR_WHITE:
        return QColor(255, 255, 255);
    case TEXT_FONT_COLOR_BLACK:
        return QColor(0, 0, 0);
    case TEXT_FONT_COLOR_RED:
        return QColor(255, 0, 0);
    case TEXT_FONT_COLOR_GREEN:
        return QColor(0, 255, 0);
    case TEXT_FONT_COLOR_BROWN:
        return QColor(128, 128, 0);
    case TEXT_FONT_COLOR_BLUE:
        return QColor(0, 0, 255);
    case TEXT_FONT_COLOR_MAGENTA:
        return QColor(255, 0, 255);
    case TEXT_FONT_COLOR_CYAN:
        return QColor(0, 255, 255);
    case TEXT_FONT_COLOR_LIGHTGRAY:
        return QColor(0xD3, 0xD3, 0xD3);
    case TEXT_FONT_COLOR_DARKGRAY:
        return QColor(128, 128, 128);
    case TEXT_FONT_COLOR_LIGHTRED:
        return QColor(255, 102, 102);
    case TEXT_FONT_COLOR_LIGHTGREEN:
        return QColor(0x90, 0xEE, 0x90);
    case TEXT_FONT_COLOR_YELLOW:
        return QColor(255, 255, 0);
    case TEXT_FONT_COLOR_LIGHTBLUE:
        return QColor(132, 112, 255);
    case TEXT_FONT_COLOR_LIGHTMAGENTA:
        return QColor(255, 119, 255);
    case TEXT_FONT_COLOR_LIGHTCYAN:
        return QColor(0xE0, 0xFF, 0xFF);
    default:
        break;
    }

    return QColor(255, 255, 255);
}

// 根据softkey的类型转化为softkey
SoftkeyParam TypeToSoftInfo(XB_SOFTKEY_TYPE eType)
{
    SoftkeyParam objParam;
    objParam.eType = eType;

    switch (eType)
    {
    case XB_ST_EXIT:
        {
            objParam.strName = TRID_EXIT;
            objParam.strURI = "SoftKey:Exit";
        }
        break;
    case XB_ST_SELECT:
        {
            objParam.strName = TRID_SELECT;
            objParam.strURI = "SoftKey:Exit";
        }
        break;
    case XB_ST_SUBMIT:
        {
            objParam.strName = TRID_SUBMIT;
            objParam.strURI = "SoftKey:Submit";
        }
        break;
    case XB_ST_DOT:
        {
            objParam.strName = TRID_DOT;
            objParam.strURI = "SoftKey:Dot";
        }
        break;
    case XB_ST_BACKSPACE:
        {
            objParam.strName = TRID_BACKSPACE;
            objParam.strURI = "SoftKey:BackSpace";
        }
        break;
    case XB_ST_NEXTSPACE:
        {
            objParam.strName = TRID_NEXTSPACE;
            objParam.strURI = "SoftKey:NextSpace";
        }
        break;
    case XB_ST_DIAL:
        {
            objParam.strName = TRID_DIAL;
            objParam.strURI = "SoftKey:Dial";
        }
        break;
    case XB_ST_PREVIOUS:
        {
            objParam.strName = TRID_PREVIOUS;
            objParam.strURI = "SoftKey:Previous";
        }
        break;
    case XB_ST_NEXT:
        {
            objParam.strName = TRID_NEXT;
            objParam.strURI = "SoftKey:Next";
        }
        break;
    case XB_ST_SEARCH:
        {
            objParam.strName = TRID_SEARCH;
            objParam.strURI = "SoftKey:Search";
        }
        break;
    default:
        {
            objParam.strName = "";
            objParam.strURI = "SoftKey:Exit";
        }
        break;
    }

    return objParam;
}

QString SoftkeyTypeToPicPath(XB_SOFTKEY_TYPE eType)
{
    switch (eType)
    {
    case XB_ST_EXIT:
        return PIC_SOFTKEY_EXIT;
    case XB_ST_DIAL:
        return PIC_SOFTKEY_SEND;
    case XB_ST_SUBMIT:
        return PIC_SOFTKEY_SUBMIT;
    case XB_ST_NEXT:
        return PIC_SOFTKEY_NEXT;
    case XB_ST_PREVIOUS:
        return PIC_SOFTKEY_PREVIOUS;
    case XB_ST_BACKSPACE:
        return PIC_SOFTKEY_DELETE;
    case XB_ST_NEXTSPACE:
        return PIC_SOFTKEY_NEXT_SAPCE;
    case XB_ST_SELECT:
        return PIC_SOFTKEY_SUBMIT;
    case XB_ST_SEARCH:
        return PIC_SOFTKEY_CONTACT_SEARCH;
    default:
        return "";
    }
}
