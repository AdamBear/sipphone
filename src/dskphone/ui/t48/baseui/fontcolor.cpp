#include "fontcolor.h"
#include "customtheme/modcustomtheme.h"
#include <ETLLib/ETLLib.hpp>

void SetWgtFontColor(QWidget * pWgt, FONT_TYPE eType, Qt::GlobalColor qtColor/* = Qt::color0*/)
{
    static FontColorManager objFontColorManager;
    objFontColorManager.SetWidgetColor(pWgt, eType, qtColor);
}

FontColorManager::FontColorManager()
{

}

FontColorManager::~FontColorManager()
{

}

void FontColorManager::SetWidgetColor(QWidget * pWgt, FONT_TYPE eType,
                                      Qt::GlobalColor qtColor/* = Qt::color0*/)
{
    // 入口参数判断
    if (NULL == pWgt)
    {
        return;
    }

    // 设置控件上文字的颜色
    QPalette objQPalette;
    if (qtColor != Qt::color0)
    {
        objQPalette.setColor(GetColorRoleByFontType(eType), qtColor);
    }
    else
    {
        objQPalette.setColor(GetColorRoleByFontType(eType),
                             GetColorByFontType(eType));
    }
    pWgt->setPalette(objQPalette);
}

void SetWidgetColor(QWidget * pWgt, QPalette::ColorRole qtColorRole, QColor objColor)
{
    if (NULL == pWgt)
    {
        return;
    }

    QPalette objQPalette;
    objQPalette.setColor(qtColorRole, objColor);
    pWgt->setPalette(objQPalette);
}

QPalette::ColorRole FontColorManager::GetColorRoleByFontType(FONT_TYPE eType)
{
    switch (eType)
    {
    case FONT_TYPE_TEXT:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TITLE:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_MENU:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_BUTTON:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TALK_TITLE:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TALK_TEXT:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TALK_BUTTON:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TALK_MENU:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_TEXT_SELECTED:
        {
            return QPalette::WindowText;
        }
    case FONT_TYPE_FOREGROUND_TEXT:
        {
            return QPalette::Text;
        }
    case FONT_TYPE_FOREGROUND_TEXT_SELECTED:
        {
            return QPalette::Text;
        }
    default:
        {
            chWARNING2(false, "FontColorManager::GetColorRoleByFontType eType error!");
            return QPalette::WindowText;
        }
    }
}

QColor FontColorManager::GetColorByFontType(FONT_TYPE eType)
{
    int nFontKeyId = CUSTOM_FONT_ID_COMMON_TEXT;
    int nFontStatus = FONT_NORMAL;

    switch (eType)
    {
    case FONT_TYPE_TEXT:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_TEXT;
        }
        break;
    case FONT_TYPE_TITLE:
        {
            nFontKeyId = CUSTOM_FONT_ID_TITLEBAR_NORMAL;
        }
        break;
    case FONT_TYPE_MENU:
        {
            nFontKeyId = CUSTOM_FONT_ID_MENU_NORMAL;
        }
        break;
    case FONT_TYPE_BUTTON:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_BUTTON;
        }
        break;
    case FONT_TYPE_TALK_TITLE:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_TEXT;
        }
        break;
    case FONT_TYPE_TALK_TEXT:
        {
            nFontKeyId = CUSTOM_FONT_ID_TALK_NORMAL;
        }
        break;
    case FONT_TYPE_TALK_BUTTON:
        {
            nFontKeyId = CUSTOM_FONT_ID_TALK_NORMAL;
        }
        break;
    case FONT_TYPE_TALK_MENU:
        {
            nFontKeyId = CUSTOM_FONT_ID_MENU_NORMAL;
        }
        break;
    case FONT_TYPE_TEXT_SELECTED:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_TEXT;
            nFontStatus = FONT_ACTIVE;
        }
        break;
    case FONT_TYPE_FOREGROUND_TEXT:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_FORGROUND_TEXT;
        }
        break;
    case FONT_TYPE_FOREGROUND_TEXT_SELECTED:
        {
            nFontKeyId = CUSTOM_FONT_ID_COMMON_FORGROUND_TEXT;
            nFontStatus = FONT_ACTIVE;
        }
        break;
    default:
        {
            chWARNING2(false, "FontColorManager::GetColorByFontType eType error!");
        }
        break;
    }

    return CustomTheme_GetCustomFontColorByKey(nFontKeyId, nFontStatus);
}
