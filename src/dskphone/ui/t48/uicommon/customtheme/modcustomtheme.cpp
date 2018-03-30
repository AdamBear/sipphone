#include "modcustomtheme.h"
#include "customthememanager.h"

void CustomTheme_Init()
{
    g_pCustomThemeManager->Init();
}

const CustomFontData & CustomTheme_GetCustomFontDataByKey(int nKey)
{
    return g_pCustomThemeManager->GetCustomFontDataByKey(nKey);
}

int CustomTheme_GetCustomFontSizeByKey(int nKey)
{
    return g_pCustomThemeManager->GetCustomFontSizeByKey(nKey);
}

QColor CustomTheme_GetCustomFontColorByKey(int nFontKey, int nFontStatus/* = FONT_NORMAL*/)
{
    return g_pCustomThemeManager->GetCustomFontColorByKey(nFontKey, nFontStatus);
}

bool CustomTheme_IsShowFontShadowByKey(int nFontKey)
{
    return g_pCustomThemeManager->IsShowFontShadowByKey(nFontKey);
}

bool CustomTheme_SetWidgetFontProperty(QWidget * pWidget, int nFontKey, bool bBold/* = false*/,
                                       QFont::Weight objWeight/* = QFont::Normal*/, int nFontStatus/* = FONT_NORMAL*/)
{
    if (NULL == pWidget)
    {
        return false;
    }

    const CustomFontData & stCustomFontData = CustomTheme_GetCustomFontDataByKey(nFontKey);

    QPalette pal = pWidget->palette();

    if (FONT_ACTIVE == nFontKey)
    {
        pal.setColor(QPalette::WindowText, stCustomFontData.m_colorActive);
    }
    else
    {
        pal.setColor(QPalette::WindowText, stCustomFontData.m_colorNormal);
    }

    pWidget->setPalette(pal);

    QFont objFont = pWidget->font();
    objFont.setPointSize(stCustomFontData.m_nFontSize);
    objFont.setWeight(objWeight);
    objFont.setBold(bBold);
    pWidget->setFont(objFont);

    return true;
}

