#include "xmlbrowseruicommon.h"
#include "translateiddefine.h"
#include "commonapi/stringhelperapi.h"
#include "xmlbrowserui/src/xmlbrowseruidefine.h"

#ifndef IME_ACTION
#define IME_ACTION "IME"
#endif

int TypeToAlignmentFlag(int eAlignType)
{
    int nAlign = 0;
    //横向
    if (eAlignType & DT_CENTER)
    {
        nAlign = nAlign | align_h_center;
    }
    else if (eAlignType & DT_RIGHT)
    {
        nAlign = nAlign | align_right;
    }
    else
    {
        nAlign = nAlign | align_left;
    }

    //纵向
    if (eAlignType & DT_VCENTER)
    {
        nAlign = nAlign | align_v_center;
    }
    else if (eAlignType & DT_TOP)
    {
        nAlign = nAlign | align_top;
    }
    else
    {
        nAlign = nAlign | align_bottom;
    }

    return nAlign;
}


int TypeToFontPointSize(X_FONT_TYPE eFontSize)
{
    switch (eFontSize)
    {
    case X_FONT_SMALL:
        return X_FONT_SMALL_SIZE;
    case X_FONT_NORMAL:
        return X_FONT_NORMAL_SIZE;
    case X_FONT_DOUBLE:
        return X_FONT_DOUBLE_SIZE;
    case X_FONT_LARGE:
        return X_FONT_LARGE_SIZE;
    default:
        break;
    }

    return X_FONT_NORMAL_SIZE;
}

xColor TypeToColor(TEXT_FONT_COLOR eColorType)
{
    // 根据类型返回相应的颜色
    switch (eColorType)
    {
    case TEXT_FONT_COLOR_WHITE:
        return xColor(255, 255, 255);
    case TEXT_FONT_COLOR_BLACK:
        return xColor(0, 0, 0);
    case TEXT_FONT_COLOR_RED:
        return xColor(255, 0, 0);
    case TEXT_FONT_COLOR_GREEN:
        return xColor(0, 255, 0);
    case TEXT_FONT_COLOR_BROWN:
        return xColor(128, 128, 0);
    case TEXT_FONT_COLOR_BLUE:
        return xColor(0, 0, 255);
    case TEXT_FONT_COLOR_MAGENTA:
        return xColor(255, 0, 255);
    case TEXT_FONT_COLOR_CYAN:
        return xColor(0, 255, 255);
    case TEXT_FONT_COLOR_LIGHTGRAY:
        return xColor(0xD3, 0xD3, 0xD3);
    case TEXT_FONT_COLOR_DARKGRAY:
        return xColor(128, 128, 128);
    case TEXT_FONT_COLOR_LIGHTRED:
        return xColor(255, 102, 102);
    case TEXT_FONT_COLOR_LIGHTGREEN:
        return xColor(0x90, 0xEE, 0x90);
    case TEXT_FONT_COLOR_YELLOW:
        return xColor(255, 255, 0);
    case TEXT_FONT_COLOR_LIGHTBLUE:
        return xColor(132, 112, 255);
    case TEXT_FONT_COLOR_LIGHTMAGENTA:
        return xColor(255, 119, 255);
    case TEXT_FONT_COLOR_LIGHTCYAN:
        return xColor(0xE0, 0xFF, 0xFF);
    default:
        break;
    }

    return xColor(255, 255, 255);
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
            objParam.strURI = "SoftKey:Select";
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
yl::string ToUISoftkeyAction(const SoftkeyParam & objParam)
{
    if (XB_ST_CHANGEMODE == objParam.eType)
    {
        return IME_ACTION;
    }
    else
    {
        //http://10.3.5.199/Bug.php?BugID=48126
        if (objParam.strURI.empty())
        {
            return "";
        }
        else
        {
            return commonAPI_FormatString("%d_%s", (int)objParam.eType, objParam.strURI.c_str());
        }
    }
}

SoftkeyParam ToLogicSoftkeyAction(const yl::string & strUIAction)
{
    SoftkeyParam tempParam;

    if (IME_ACTION == strUIAction)
    {
        tempParam.eType = XB_ST_CHANGEMODE;
        tempParam.strURI = "";
    }
    else
    {
        int iPos = strUIAction.find_first_of('_');
        if (yl::string::npos != iPos)
        {
            tempParam.eType = (XB_SOFTKEY_TYPE)atoi(strUIAction.substr(0, iPos).c_str());
            tempParam.strURI = strUIAction.substr(iPos + 1, strUIAction.length() - 1);
        }
    }

    return tempParam;
}
