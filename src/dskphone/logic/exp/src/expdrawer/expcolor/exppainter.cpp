#include "exppainter.h"
#include "expdefine.h"

#include "devicelib/phonedevice.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

namespace
{
#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
#define CACHE_MAX_SIZE 40
#define CACHE_ELASTICITY 8
#define IMAGE_LIMIT_WIDTH_HEIGHT    50
#endif
}

EXPPainter::EXPPainter()
    :
#ifdef EXP_PAINT_ROTATE
    m_imgCanvas(EXP_SCREEN_HEIGHT, EXP_SCREEN_WIDTH, MYPAINTER_FORMAT_16)
#else
    m_imgCanvas(EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT, MYPAINTER_FORMAT_16)
#endif
#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
    , m_ExpImageCache(CACHE_MAX_SIZE, CACHE_ELASTICITY)
#endif
    , m_painterCanvas(m_imgCanvas)
{
    m_painterCanvas.SetFontSize(TEXT_FONT_SIZE);

#ifdef EXP_PAINT_ROTATE
    m_painterCanvas.Translate(EXP_SCREEN_HEIGHT, 0);
    m_painterCanvas.Rotate(EXP_ROTATE_ANGLE);
#endif

    m_painterCanvas.SetAntiAlias(true);
}

EXPPainter::~EXPPainter()
{

}

void EXPPainter::Init()
{
    m_PicCache.Init();
}

void EXPPainter::Uninit()
{
    m_PicCache.Uninit();
}

bool EXPPainter::SetBackGround(const yl::string & strBackGround, int nTransparentLevel)
{
    return m_PicCache.SetBackGround(strBackGround, nTransparentLevel);
}

bool EXPPainter::ReloadCanvas()
{
    m_PicCache.ReloadCanvas(m_painterCanvas);
    return true;
}

bool EXPPainter::ReloadCanvas(int nRow, int nCol, bool bLoadFull/* = false*/)
{
    if (nRow < 0 || nRow >= EXP_BTN_ROW_COUNT
            || nCol < 0 || nCol >= EXP_BTN_COL_COUNT)
    {
        return false;
    }

    if (bLoadFull)
    {
        m_PicCache.ReloadCanvas(m_painterCanvas
                                , CMyRect(0 == nCol ? EXP_FULL_LEFT_BTN_LEFT : EXP_FULL_RIGHT_BTN_LEFT
                                          , s_DsskeyFullBtnTop[nRow]
                                          , EXP_FULL_BTN_WIDTH
                                          , EXP_FULL_BTN_HEIGHT));
    }
    else
    {
        m_PicCache.ReloadCanvas(m_painterCanvas
                                , CMyRect(0 == nCol ? EXP_LEFT_BTN_LEFT : EXP_RIGHT_BTN_LEFT
                                          , s_DsskeyBtnTop[nRow]
                                          , EXP_BTN_FLUSH_WIDTH
                                          , EXP_BTN_FLUSH_HEIGHT));
    }

    return true;
}

void EXPPainter::PaintIcon(int nRow, int nCol, int nIconType, int nFlag /*= DSSKEY_ALIGN_NORMOL*/)
{
    //特殊情况：恰好等于DK_ICON_Default说明当前不需要画图标
    if (nIconType <= DK_ICON_Default || nIconType >= DK_ICON_CUSTOM)
    {
        return;
    }

    //绘制图标
    m_painterCanvas.DrawImage(CMyRect(0 == nCol ? ICON_LEFT_PART_LEFT : DSSKEY_AlIGN_RIGHT == nFlag ? ICON_RIGHT_PART_RIGHT : ICON_RIGHT_PART_LEFT
                                      , s_DsskeyIconTop[nRow]
                                      , ICON_WIDTH
                                      , ICON_HEIGHT)
                              , m_PicCache.GetIconImage(nIconType));
}

void EXPPainter::PaintIcon(int nRow, int nCol, const yl::string & strIcon, int nFlag /*= DSSKEY_ALIGN_NORMOL*/)
{
#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
    CMyImage imgIcon;
    GetImageByPath(imgIcon, strIcon);
#else
    CMyImage imgIcon(strIcon);
    EXP_INFO("load imgIcon\n");
    if (imgIcon.isNull())
    {
        EXP_INFO("imgIcon.isNull()\n");
        return;
    }
#endif

    //绘制图标
    m_painterCanvas.DrawImage(CMyRect(0 == nCol ? ICON_LEFT_PART_LEFT : DSSKEY_AlIGN_RIGHT == nFlag ? ICON_RIGHT_PART_RIGHT : ICON_RIGHT_PART_LEFT
                                      , s_DsskeyIconTop[nRow]
                                      , ICON_WIDTH
                                      , ICON_HEIGHT)
                              , imgIcon);
}

void EXPPainter::PaintStatus(int nRow, int nCol, int nStatus, int nFlag /*= DSSKEY_ALIGN_NORMOL*/)
{
    //特殊情况：恰好等于PRESENCE_STATUS_NONE说明当前不需要画图标
    if (nStatus <= PRESENCE_STATUS_NONE || nStatus >= PRESENCE_STATUS_MAX)
    {
        return;
    }

    //是否显示新状态图标
    bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;

    //绘制图标
    if (bNewIconMode)
    {
        m_painterCanvas.DrawImage(CMyRect(0 == nCol ? STATUS_NEW_ICON_LEFT_PART_LEFT :
                                          DSSKEY_AlIGN_RIGHT == nFlag ? STATUS_NEW_ICON_RIGHT_PART_RIGHT : STATUS_NEW_ICON_RIGHT_PART_LEFT
                                          , s_DsskeyNewStatusTop[nRow]
                                          , STATUS_ICON_WIDTH
                                          , STATUS_ICON_HEIGHT)
                                  , m_PicCache.GetStatusImage(nStatus, bNewIconMode));
    }
    else
    {
        m_painterCanvas.DrawImage(CMyRect(0 == nCol ? STATUS_ICON_LEFT_PART_LEFT :
                                          DSSKEY_AlIGN_RIGHT == nFlag ? STATUS_ICON_RIGHT_PART_RIGHT : STATUS_ICON_RIGHT_PART_LEFT
                                          , s_DsskeyStatusTop[nRow]
                                          , STATUS_ICON_WIDTH
                                          , STATUS_ICON_HEIGHT)
                                  , m_PicCache.GetStatusImage(nStatus, bNewIconMode));
    }

}

void EXPPainter::PaintText(int nRow, int nCol, const yl::string & strText, int nFlag /*= DSSKEY_ALIGN_NORMOL*/)
{
    yl::string strCutText = m_painterCanvas.CutOffText(strText, TEXT_WIDTH);

    //绘制文字阴影
    m_painterCanvas.SetPenColor(MYPAINTER_COLOR_BLACK);
    m_painterCanvas.DrawText(CMyRect((0 == nCol ? TEXT_LEFT_PART_LEFT : DSSKEY_AlIGN_RIGHT == nFlag ? TEXT_RIGHT_PART_RIGHT : TEXT_RIGHT_PART_LEFT) + 1
                                     , s_DsskeyTextTop[nRow] + 1
                                     , TEXT_WIDTH
                                     , TEXT_HEIGHT)
                             , strCutText
                             , true, true, nFlag);

    //绘制文字
    m_painterCanvas.SetPenColor(MYPAINTER_COLOR_WHITE);
    m_painterCanvas.DrawText(CMyRect(0 == nCol ? TEXT_LEFT_PART_LEFT : DSSKEY_AlIGN_RIGHT == nFlag ? TEXT_RIGHT_PART_RIGHT : TEXT_RIGHT_PART_LEFT
                                     , s_DsskeyTextTop[nRow]
                                     , TEXT_WIDTH
                                     , TEXT_HEIGHT)
                             , strCutText
                             , true, true, nFlag);
}

void EXPPainter::PaintBtn(int nRow, int nCol, bool bDown, int nDsskeyBgType
                          , bool bPaintIcon, bool bPaintText
                          , int nIconType, const yl::string & strText
                          , const yl::string & strIcon, int nStatus, int nFlag/* = DSSKEY_ALIGN_NORMOL*/)
{
    if (nRow < 0 || nRow >= EXP_BTN_ROW_COUNT
            || nCol < 0 || nCol >= EXP_BTN_COL_COUNT)
    {
        return;
    }

    // 绘制背景
    CMyRect rcButton(0 == nCol ? EXP_FULL_LEFT_BTN_LEFT : EXP_FULL_RIGHT_BTN_LEFT,
                     s_DsskeyFullBtnTop[nRow], EXP_FULL_BTN_WIDTH, EXP_FULL_BTN_HEIGHT);

    switch (nDsskeyBgType)
    {
    case EXP_DSS_GROUNDPHOTO_MODE_NORMAL:
        {
            if (bDown)
            {
                m_PicCache.PaintDownButton(m_painterCanvas, rcButton);
            }
        }
        break;
    case EXP_DSS_GROUNDPHOTO_MODE_HIGHLIGHT:
        {
            m_PicCache.PaintHeightLightButton(m_painterCanvas, rcButton);
        }
        break;
    case EXP_DSS_GROUNDPHOTO_MODE_USING:
        {
            m_PicCache.PaintUsingButton(m_painterCanvas, rcButton);
        }
        break;
    default:
        break;
    }

    //再绘制图标和文字
    if (bPaintIcon)
    {
        if (DK_ICON_CUSTOM == nIconType && !strIcon.empty())
        {
            PaintIcon(nRow, nCol, strIcon, nFlag);
        }
        else
        {
            PaintIcon(nRow, nCol, nIconType, nFlag);
        }

        PaintStatus(nRow, nCol, nStatus, nFlag);
    }
    if (bPaintText)
    {
        PaintText(nRow, nCol, strText, nFlag);
    }
}

void EXPPainter::EXPFillWithColor(const EXP_FILL_COLOR & objExpColor)
{
    EXPFillWithColor(objExpColor, m_painterCanvas);
}

void EXPPainter::EXPFillWithColor(const EXP_FILL_COLOR & objExpColor, CMyPainter & painterCanvas)
{
    painterCanvas.Save();

    if (objExpColor.bGradient)
    {
        painterCanvas.DrawGradient(CMyRect(0
                                           , 0
                                           , EXP_SCREEN_WIDTH
                                           , EXP_SCREEN_HEIGHT)
                                   , objExpColor.nRed, objExpColor.nGreen, objExpColor.nBlue);
    }
    else
    {
        painterCanvas.DrawColor(CMyRect(0
                                        , 0
                                        , EXP_SCREEN_WIDTH
                                        , EXP_SCREEN_HEIGHT)
                                , objExpColor.nRed, objExpColor.nGreen, objExpColor.nBlue);
    }

    painterCanvas.Restore();
}

void EXPPainter::EXPFillWithPicture(const yl::string & pStrPicture)
{
    EXPFillWithPicture(pStrPicture, m_painterCanvas);
}

void EXPPainter::EXPFillWithPicture(const yl::string & pStrPicture, CMyPainter & painterCanvas)
{
    CMyImage imgPic;
    if (!imgPic.LoadImage(pStrPicture))
    {
        return;
    }

    painterCanvas.DrawImage(CMyRect(0
                                    , 0
                                    , EXP_SCREEN_WIDTH
                                    , EXP_SCREEN_HEIGHT)
                            , imgPic);
}

void EXPPainter::EXPDrawText(const yl::string & pStrText)
{
    m_painterCanvas.Save();

    m_painterCanvas.SetFontSize(TEXT_FONT_TESTMODE_SIZE);

    //绘制文字阴影
    m_painterCanvas.SetPenColor(MYPAINTER_COLOR_BLACK);

    m_painterCanvas.DrawText(CMyRect(1
                                     , 1
                                     , EXP_SCREEN_WIDTH
                                     , EXP_SCREEN_HEIGHT)
                             , pStrText
                             , false, false);

    m_painterCanvas.SetPenColor(MYPAINTER_COLOR_WHITE);

    m_painterCanvas.DrawText(CMyRect(0
                                     , 0
                                     , EXP_SCREEN_WIDTH
                                     , EXP_SCREEN_HEIGHT)
                             , pStrText
                             , false, false);

    m_painterCanvas.SetFontSize(TEXT_FONT_SIZE);

    m_painterCanvas.Restore();
}

#if EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR
void EXPPainter::GetImageByPath(CMyImage & objTargetImage, const yl::string & strPath)
{
    if (!m_ExpImageCache.tryGet(strPath, objTargetImage))
    {
        EXP_INFO("[Exp-test] CMyImage Not Cache, and Load It[%s].\n", strPath.toUtf8().data());
        objTargetImage.load(strPath);
        QSize szImage = objTargetImage.size();

        if (szImage.width() <= IMAGE_LIMIT_WIDTH_HEIGHT && szImage.height() <= IMAGE_LIMIT_WIDTH_HEIGHT)
        {
            m_ExpImageCache.insert(strPath, objTargetImage);
        }
    }
}
#endif
