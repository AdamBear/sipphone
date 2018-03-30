#include "exprefresher.h"
#include "expmanager.h"

EXPRefresher::EXPRefresher()
    : m_nFileHandle(-1)
    , m_pFBMmap(0)
    , m_nFBMmapLength(0)
{

}


EXPRefresher::~EXPRefresher()
{

}

void EXPRefresher::Init()
{

}

void EXPRefresher::Uninit()
{

}

void EXPRefresher::SetMmapInfo(int nFileHandle, char * pFBMmap, unsigned int nFBMmapLength)
{
    m_nFileHandle = nFileHandle;
    m_pFBMmap = pFBMmap;
    m_nFBMmapLength = nFBMmapLength;
}

void EXPRefresher::SyncImageToMem(CMyImage & imgCanvas)
{
    memcpy(m_pFBMmap, imgCanvas.Getbits(), m_nFBMmapLength);
}

void EXPRefresher::RefreshButton(int nIndex, int nPage, int nRow, int nCol
                                 , bool bRefreshIcon, bool bRefreshText, bool bRefreshFull/* = false*/)
{
    if (m_nFileHandle < 0
            || !bRefreshIcon && !bRefreshText
            /*|| _EXPManager.GetFlushedByIndex(nIndex)*/)
    {
        return;
    }

    struct expusb_fillrect rect;
    rect.port = nIndex;

    rect.page = 1;
    rect.display = _EXPManager.IsInNormal() ? 1 : 0;
    if (0 == rect.display)
    {
        EXP_INFO("[Exp-Info] EXPRefresher::RefreshButton nIndex[%d] nRow[%d] nCol[%d].\n", nIndex, nRow,
                 nCol);
    }

    if (bRefreshIcon && bRefreshText)
    {
        if (bRefreshFull)
        {
#ifdef EXP_PAINT_ROTATE
            rect.x = EXP_SCREEN_HEIGHT - s_DsskeyFullBtnTop[nRow] - EXP_FULL_BTN_HEIGHT;
            rect.y = 0 == nCol ? EXP_FULL_LEFT_BTN_LEFT : EXP_FULL_RIGHT_BTN_LEFT;
            rect.w = EXP_FULL_BTN_HEIGHT;
            rect.h = EXP_FULL_BTN_WIDTH;
#else
            rect.x = 0 == nCol ? EXP_FULL_LEFT_BTN_LEFT : EXP_FULL_RIGHT_BTN_LEFT;
            rect.y = s_DsskeyFullBtnTop[nRow];
            rect.w = EXP_FULL_BTN_WIDTH;
            rect.h = EXP_FULL_BTN_HEIGHT;
#endif
        }
        else
        {
#ifdef EXP_PAINT_ROTATE
            rect.x = EXP_SCREEN_HEIGHT - s_DsskeyBtnTop[nRow] - EXP_BTN_FLUSH_HEIGHT;
            rect.y = 0 == nCol ? EXP_LEFT_BTN_LEFT : EXP_RIGHT_BTN_LEFT;
            rect.w = EXP_BTN_FLUSH_HEIGHT;
            rect.h = EXP_BTN_FLUSH_WIDTH;
#else
            rect.x = 0 == nCol ? EXP_LEFT_BTN_LEFT : EXP_RIGHT_BTN_LEFT;
            rect.y = s_DsskeyBtnTop[nRow];
            rect.w = EXP_BTN_FLUSH_WIDTH;
            rect.h = EXP_BTN_FLUSH_HEIGHT;
#endif
        }

    }
    else if (bRefreshIcon)
    {
#ifdef EXP_PAINT_ROTATE
        rect.x = EXP_SCREEN_HEIGHT - s_DsskeyIconTop[nRow] - ICON_HEIGHT - STATUS_ICON_OFFSET_V;
        rect.y = 0 == nCol ? ICON_LEFT_PART_LEFT : ICON_RIGHT_PART_LEFT;
        rect.w = ICON_HEIGHT + STATUS_ICON_OFFSET_V;
        rect.h = ICON_WIDTH + STATUS_ICON_OFFSET_H;
#else
        rect.x = 0 == nCol ? ICON_LEFT_PART_LEFT : ICON_RIGHT_PART_LEFT;
        rect.y = s_DsskeyIconTop[nRow];
        rect.w = ICON_WIDTH + STATUS_ICON_OFFSET_H;
        rect.h = ICON_HEIGHT + STATUS_ICON_OFFSET_V;
#endif
    }
    else if (bRefreshText)
    {
#ifdef EXP_PAINT_ROTATE
        rect.x = EXP_SCREEN_HEIGHT - s_DsskeyTextTop[nRow] - TEXT_HEIGHT;
        rect.y = 0 == nCol ? TEXT_LEFT_PART_LEFT : TEXT_RIGHT_PART_LEFT;
        rect.w = TEXT_HEIGHT;
        rect.h = TEXT_WIDTH;
#else
        rect.x = 0 == nCol ? TEXT_LEFT_PART_LEFT : TEXT_RIGHT_PART_LEFT;
        rect.y = s_DsskeyTextTop[nRow];
        rect.w = TEXT_WIDTH;
        rect.h = TEXT_HEIGHT;
#endif
    }

    expusb_helper_fb_fillrect(m_nFileHandle, rect);
}

void EXPRefresher::RefreshScreen(int nIndex, int iCanvasIndex /* = 1 */,
                                 int iFlushRightNow /* = 1 */)
{
    if (m_nFileHandle < 0)
    {
        return;
    }

    EXP_INFO("[Exp-Info] EXPRefresher::RefreshScreen Port[%d] Page[%d] Full, iFlushRightNow[%d].\n",
             nIndex, iCanvasIndex, iFlushRightNow);
    for (int i = 0; i < EXP_REFRESH_RETRY_TIME; ++i)
    {
        if (!_EXPManager.GetExpInited()/* || _EXPManager.GetFlushedByIndex(nIndex)*/)
        {
            EXP_INFO("[Exp-Info] EXPRefresher::RefreshScreen Not Sync %d, %d, %d"
                     , _EXPManager.GetExpInited(), nIndex, _EXPManager.GetFlushedByIndex(nIndex));
            return;
        }

        EXP_INFO("[Exp-info] EXPRefresher::RefreshScreen Normal %d, iCanvasIndex %d\n",
                 _EXPManager.IsInNormal(), iCanvasIndex);

        if (!_EXPManager.IsInNormal()
                && 1 == iCanvasIndex)
        {
            iFlushRightNow = 0;
        }
        int nResult = expusb_helper_fb_flush(m_nFileHandle, nIndex, iCanvasIndex, iFlushRightNow);

        if (nResult < 0)
        {
            EXP_INFO("[Exp-Err] EXPRefresher::RefreshScreen Time [%d], Port[%d] Full fail.\n", i, nIndex);
        }
        else
        {
            EXP_INFO("[Exp-Info] EXPRefresher::RefreshScreen Time [%d], Port[%d] Full success.\n", i, nIndex);
            break;
        }

        etlSleep(10 * 1000);
    }
}

void EXPRefresher::RefreshLight(int nIndex, int nRow, int nCol, int nLightColor)
{
    if (m_nFileHandle < 0)
    {
        return;
    }

    expusb_led objLedInfo;
    objLedInfo.row = nRow + 1;
    objLedInfo.col = nCol + 1;
    objLedInfo.port = nIndex;

    LightAdapt(objLedInfo, nLightColor);

    EXP_INFO("[Exp-Info]EXPRefresher::RefreshLight ExpPor[%d], Row[%d], Col[%d], Type[%d] Time[%d]\n",
             nIndex, nRow, nCol, objLedInfo.mask, objLedInfo.flash_timer);

    expusb_helper_led_set(m_nFileHandle, objLedInfo);
}

void EXPRefresher::LightAdapt(expusb_led & objLedInfo, int nLightColor)
{
    LIGHT_Type eLightType = (LIGHT_Type)nLightColor;

    switch (eLightType)
    {
    case LT_GREEN_ON:
        objLedInfo.mask = LED_GREEN;
        objLedInfo.flash_timer = 0;
        break;
    case LT_RED_ON:
        objLedInfo.mask = LED_RED;
        objLedInfo.flash_timer = 0;
        break;
    case LT_RED_GREEN_ON:
        objLedInfo.mask = LED_YELLOW;
        objLedInfo.flash_timer = 0;
        break;
    case LT_RED_FLASH_1:
        objLedInfo.mask = LED_RED_F;
        objLedInfo.flash_timer = 2;
        break;
    case LT_RED_FLASH_2:
        objLedInfo.mask = LED_RED_F;
        objLedInfo.flash_timer = 3;
        break;
    case LT_RED_FLASH_3:
        objLedInfo.mask = LED_RED_F;
        objLedInfo.flash_timer = 5;
        break;
    case LT_RED_FLASH_4:
        objLedInfo.mask = LED_RED_F;
        objLedInfo.flash_timer = 10;
        break;
    case LT_GREEN_FLASH_1:
        objLedInfo.mask = LED_GREEN_F;
        objLedInfo.flash_timer = 2;
        break;
    case LT_GREEN_FLASH_2:
        objLedInfo.mask = LED_GREEN_F;
        objLedInfo.flash_timer = 3;
        break;
    case LT_GREEN_FLASH_3:
        objLedInfo.mask = LED_GREEN_F;
        objLedInfo.flash_timer = 5;
        break;
    case LT_GREEN_FLASH_4:
        objLedInfo.mask = LED_GREEN_F;
        objLedInfo.flash_timer = 10;
        break;
    case LT_RED_GREEN_FLASH_1:
        objLedInfo.mask = LED_YELLOW_F;
        objLedInfo.flash_timer = 2;
        break;
    case LT_RED_GREEN_FLASH_2:
        objLedInfo.mask = LED_YELLOW_F;
        objLedInfo.flash_timer = 3;
        break;
    case LT_RED_GREEN_FLASH_3:
        objLedInfo.mask = LED_YELLOW_F;
        objLedInfo.flash_timer = 5;
        break;
    case LT_RED_GREEN_FLASH_4:
        objLedInfo.mask = LED_YELLOW_F;
        objLedInfo.flash_timer = 10;
        break;
    default:
        objLedInfo.mask = LED_CLOSE;
        objLedInfo.flash_timer = 0;
        break;
    }
}

void EXPRefresher::RefreshBackLight(int nIndex, int nBackLightLevel)
{
    if (m_nFileHandle < 0)
    {
        return;
    }

    int nRet = expusb_helper_setbklight(m_nFileHandle, nIndex, nBackLightLevel);
    EXP_INFO("[Exp-Info]EXPRefresher::RefreshBackLight nIndex %d, nBackLightLevel %d, nRet %d\n"
             , nIndex, nBackLightLevel, nRet);
}

void EXPRefresher::RefreshLogo(int nIndex)
{
    if (m_nFileHandle < 0)
    {
        return;
    }

    expusb_helper_fb_logodisp(m_nFileHandle, nIndex);
}

void EXPRefresher::RefreshMemPage(int nIndex, int nMemPage)
{
    if (m_nFileHandle < 0)
    {
        EXP_ERR("[Exp-Info]RefreshMemPage m_nFileHandle %d\n", m_nFileHandle);
        return;
    }

    EXP_INFO("[Exp-Info]RefreshMemPage nIndex %d, nMemPage %d\n"
             , nIndex, nMemPage);

    if (EXP_MAX_COUNT <= nIndex)
    {
        for (int i = 0; i < EXP_MAX_COUNT; ++i)
        {
            if (_EXPManager.GetFlushedByIndex(i) || 1 != nMemPage)
            {
                int nRet = expusb_helper_fb_pagedisp(m_nFileHandle, i, nMemPage);
                EXP_INFO("[Exp-Info]RefreshMemPage Multy i %d, nRet %d\n"
                         , i, nRet);
            }
            else
            {
                EXP_INFO("[Exp-Info]RefreshMemPage Multy Not i %d, Flushed %d, nMemPage %d\n"
                         , i, _EXPManager.GetFlushedByIndex(i), nMemPage);
            }
        }
    }
    else if (_EXPManager.GetFlushedByIndex(nIndex) || 1 != nMemPage)
    {
        int nRet = expusb_helper_fb_pagedisp(m_nFileHandle, nIndex, nMemPage);
        EXP_INFO("[Exp-Info]RefreshMemPage Only nIndex %d, nRet %d\n"
                 , nIndex, nRet);
    }
    else
    {
        EXP_INFO("[Exp-Info]RefreshMemPage Multy Only Not nIndex %d, Flushed %d, nMemPage %d\n"
                 , nIndex, _EXPManager.GetFlushedByIndex(nIndex), nMemPage);
    }
}

