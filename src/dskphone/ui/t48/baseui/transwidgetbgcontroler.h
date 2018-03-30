#ifndef __TRANSPARENT_WIDGET_BACKGROUND_CONTROLER__
#define __TRANSPARENT_WIDGET_BACKGROUND_CONTROLER__

#include <singletonclassdefine.h>

enum TransparentType
{
    TRANSPARENT_TYPE_LOW_TRANS = 1,
    TRANSPARENT_TYPE_TWENTY_PERCENT,
    TRANSPARENT_TYPE_FORTY_PERCENT,
    TRANSPARENT_TYPE_SIXTY_PERCENT,
    TRANSPARENT_TYPE_EIGHTY_PERCENT,
    TRANSPARENT_TYPE_HEIGHT_TRANS,
};

class CTransWidgetBgControler
{
    DECLARE_SINGLETON_CLASS(CTransWidgetBgControler)

public:
    static yl::string GetTransValueByType(int nType);

    void UpdateTransType();
    int GetTransparencyType();

    void SetCurDlgUseTransparency(bool bUseTrans);
    bool IsCurDlgUseTransparency();

    yl::string GetTitleBarBg();
    yl::string GetNormalDsskeyBg();
    yl::string GetEnableDsskeyBg();
    yl::string GetDisableDsskeyBg();
    yl::string GetHeightlightDsskeyBg();
    yl::string GetUsingDsskeyBg();
    yl::string GetGreenDsskeyBg();
    yl::string GetRedDsskeyBg();
    yl::string GetOrangeDsskeyBg();

protected:
    void ChangeAllTransWidgetBg();

    void GetLowTransPicture();
    void GetTwentyPercentTransPicture();
    void GetFortyPercentTransPicture();
    void GetSixtyPercentTransPicture();
    void GetEightyPercentTransPicture();
    void GetHeightTransPicture();

private:
    bool                m_bCurDlgUseTransparency;
    int                 m_nTransType;

    yl::string          m_strTitleBarBg;
    yl::string          m_strDssBgNormal;
    yl::string          m_strDssBgEnable;
    yl::string          m_strDssBgDisable;
    yl::string          m_strDssBgHeightlight;
    yl::string          m_strDssBgUsing;
    yl::string          m_strDssBgGreen;
    yl::string          m_strDssBgRed;
    yl::string          m_strDssBgOrange;
};

#define g_TransWidgetBgControler GET_SINGLETON_INSTANCE(CTransWidgetBgControler)

#endif // #ifndef __TRANSPARENT_WIDGET_BACKGROUND_CONTROLER__
