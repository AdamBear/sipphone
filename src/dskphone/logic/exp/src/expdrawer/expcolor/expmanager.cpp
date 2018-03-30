#include "expmanager.h"
#include "expdefine.h"

#include "devicelib/phonedevice.h"
#include "expusb_api.h"

#include <sys/time.h>

namespace
{
#define VP_PHONE_THREAD_NAME                    "app_vpPhone"

#define EXP_LIST_FLUSH_MAX_NUM                  5
#define EXP_LIST_FLUSH_DSSKEY_MAX_NUM       20  //EXPdsskey按键个数
#define EXP_LIST_FLUSH_KEY_MAX_NUM          23    //EXP总按键个数
#define EXP_LIST_FLUSH_DEFAULT_INDEX            (-1)

#define EXP_INIT_DELAY_TIME                             500
}

EXPManager::EXPManager()
    : m_strEditingText("")
    , m_nDownDsskeyID(0)
    , m_nLastIndex(0)
    , m_nSameIndexCount(0)
    , m_vecExpInfoFlush(EXP_SEND_LIST_MAX_COUNT)
    , m_nExpCount(0)
{
    EXP_INFO("[Exp-Info]EXPManager start\n");

    m_vecExpInfoFlush.clear();

    for (int i = 0; i < EXP_MAX_COUNT; ++i)
    {
        m_nExpFirstFlush[i] = 0;
    }
}


EXPManager::~EXPManager()
{

    m_mutexUpdate.Unlock();
    m_mutexListFlush.Unlock();
}

EXPManager & GetEXPManagerInstance()
{
    // 用函数内static方式控制全局对象的构造顺序
    static EXPManager s_EXPManager;
    return s_EXPManager;
}

void EXPManager::Init()
{
    EXP_INFO("[Exp-Info]EXPManager Init\n");

    m_expPainter.Init();
    m_expRefresher.Init();

    InitMmap();

#ifdef EXP_PAINT_TEST
    ExpBgData objBGData;
    strcpy(objBGData.szBackGround, PIC_DEFAULT_EXP_BACKGROUND);
    objBGData.nTransparentLevel = 1;
    OnUpdateBGData(objBGData, 1);
#endif
    SetDelayTimeNum(EXP_INIT_DELAY_TIME);
    SetDelayTimer();
}

void EXPManager::Uninit()
{
    m_mutexUpdate.Unlock();
    m_mutexListFlush.Unlock();

    m_expPainter.Uninit();
    m_expRefresher.Uninit();
}

LRESULT EXPManager::OnEXPMessage(msgObject & objMessage)
{
    EXP_INFO("EXPManager::OnEXPMessage[%d],wParam[%d],lParam[%d]\n", objMessage.message,
             objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case EXP_MESSAGE_BG_CHANGE:
        {
            //总是处理背景变化
            ExpBgData * pBackGroundData = NULL;
            pBackGroundData = (ExpBgData *)objMessage.GetExtraData();

            if (NULL != pBackGroundData)
            {
                OnUpdateBGData(*pBackGroundData, objMessage.wParam);
            }
        }
        break;

    case EXP_MESSAGE_BG_REFRESH:
        {
            if (!m_bInit)
            {
                break;
            }

//          OnRefreshBG(objMessage.wParam, objMessage.lParam);
        }
        break;

    case EXP_MESSAGE_BACKLIGHT:
        {
            //总是要处理亮度消息
            int * pnLightLevel = NULL;
            pnLightLevel = (int *)objMessage.GetExtraData();

            if (NULL != pnLightLevel)
            {
                OnUpdateLight(objMessage.wParam, objMessage.lParam, *pnLightLevel);
            }
        }
        break;

    case EXP_MESSAGE_RESET_LIGHT:
        {
            if (!m_bInit)
            {
                break;
            }

            ResetAllLight(objMessage.wParam, objMessage.lParam);
        }
        break;

    case EXP_MESSAGE_EDITING_TEXT:
        {
            char * pStrLabel = NULL;
            pStrLabel = (char *)objMessage.GetExtraData();

            if (NULL != pStrLabel)
            {
                UpdateEditingText(pStrLabel);
            }
        }
        break;

    case EXP_MESSAGE_COUNT_CHANGE:
        {
            OnExpCountChange(objMessage.wParam);
        }
        break;

    case EXP_MESSAGE_PROCESS_RELOAD:
        {
            if (0 == objMessage.wParam)
            {
                m_bInit = false;
            }
            else if (1 == objMessage.wParam)
            {
                InitMmap();
                SetDelayTimer();
            }
        }
        break;

    case EXP_MESSAGE_SCREENSAVER:
        {
            EnterScreenSaver(1 == objMessage.lParam);
        }
        break;

    case EXP_MESSAGE_FILL_COLOR:
        {
            if (!m_bInit)
            {
                break;
            }

            EXP_FILL_COLOR * pColorData = NULL;
            pColorData = (EXP_FILL_COLOR *)objMessage.GetExtraData();

            if (NULL != pColorData)
            {
                EXPFillWithColor(true, *pColorData, objMessage.lParam);
            }
        }
        break;
    case EXP_MESSAGE_FILL_PICTURE:
        {
            if (!m_bInit)
            {
                break;
            }

            const char * pStrPicture = (const char *)objMessage.GetExtraData();

            if (NULL != pStrPicture)
            {
                EXPFillWithPicture(true, pStrPicture, objMessage.lParam);
            }
        }
        break;
    case EXP_MESSAGE_DRAW_TEXT:
        {
            if (!m_bInit)
            {
                break;
            }

            const char * pStrText = (const char *)objMessage.GetExtraData();

            if (NULL != pStrText)
            {
                EXPDrawText(pStrText, objMessage.lParam);
            }
        }
        break;

    case EXP_MESSAGE_SHOW_LOGO:
        {
            if (!m_bInit)
            {
                break;
            }

            EXPShowLogo(objMessage.lParam);
        }
        break;

    default:
        return EXPManagerBase::OnEXPMessage(objMessage);

        break;
    }

    return 0;
}

void EXPManager::UpdateList()
{
    EXP_INFO("[Exp-Test] UpdateList Begin\n");
    while (true)
    {
        m_mutexListFlush.Lock();
        m_mutexUpdate.Lock();

        int nSize = m_vecExpInfoFlush.size();
        int nEXPNum = EXP_LIST_FLUSH_DEFAULT_INDEX;

        UpdateMsgDispose(m_vecExpInfoFlush, nEXPNum);
        for (int i = 0; i <= nEXPNum; ++i)
        {
            EXP_INFO("[Exp-info]Flush ExpList Port[%d] Begin\n", i);
            //如果有刷背景的消息，说明是一整页
            if (m_arrRefreshBGFlush[i] || IsFullPageNeedRefresh(i))
            {
                if (m_expPainter.ReloadCanvas())
                {
                    for (int j = 1; j < EXP_LIST_FLUSH_KEY_MAX_NUM + 1; ++j)
                    {

                        if (m_arrFlushInfo[i][j] < nSize && m_arrFlushInfo[i][j] >= 0)
                        {
                            UpdateDsskeyData(m_vecExpInfoFlush[m_arrFlushInfo[i][j]], false, true, false);
                        }
                    }
                }
                m_expRefresher.SyncImageToMem(m_expPainter.GetCanvas());
                if (m_bInit)
                {
                    m_expRefresher.RefreshScreen(i);
                }
                for (int j = 1; j < EXP_LIST_FLUSH_KEY_MAX_NUM + 1; ++j)
                {
                    if (m_arrFlushInfo[i][j] < nSize && m_arrFlushInfo[i][j] >= 0)
                    {
                        UpdateDsskeyData(m_vecExpInfoFlush[m_arrFlushInfo[i][j]], false, false, true);
                    }
                }

                if (0 == m_nExpFirstFlush[i])
                {
                    m_nExpFirstFlush[i] = 1;

                    EXP_FILL_COLOR expBlack;
                    expBlack.nRed = 0;
                    expBlack.nGreen = 0;
                    expBlack.nBlue = 0;
                    expBlack.bGradient = false;

                    m_mutexUpdate.Unlock();

                    EXPFillWithPicture(false, PIC_DEFAULT_EXP_SCREENSAVER, i, 2, 0);
                    EXPFillWithColor(false, expBlack, i, 3, 0);

                    m_mutexUpdate.Lock();
                }

                m_arrRefreshBGFlush[i] = false;
            }
            //有数据，但不足20个
            else if (m_arrbWithData[i])
            {
                for (int j = 1; j < EXP_LIST_FLUSH_KEY_MAX_NUM + 1; ++j)
                {
                    if (m_arrFlushInfo[i][j] < nSize && m_arrFlushInfo[i][j] >= 0)
                    {
                        UpdateDsskeyData(m_vecExpInfoFlush[m_arrFlushInfo[i][j]], true);
                    }
                }
            }

            EXP_INFO("[Exp-info]Flush ExpList Port[%d] End\n", i);
        }

        m_vecExpInfoFlush.clear();

        m_mutexListFlush.Unlock();

        if (m_vecExpInfoReceive.size() > 0)
        {
            m_mutexListReceive.Lock();

            m_vecExpInfoFlush = m_vecExpInfoReceive;
            m_vecExpInfoReceive.clear();

            m_mutexListReceive.Unlock();
            m_mutexUpdate.Unlock();
        }
        else
        {
            m_mutexUpdate.Unlock();
            break;
        }
    }
    EXP_INFO("[Exp-Test] UpdateList End\n");

}

void EXPManager::UpdateDsskeyData(const ExpDisplayItemInfo & objUpdateData,
                                  bool bFlushAtOnce /* = true */, bool bFlushLabel /* = true */, bool bFlushLight /* = true */)
{
    if (!m_bInit)
    {
        return;
    }

    /*
    bool bPaintFull = false;
    if (0 == m_nDownDsskeyID)
    {
        if (objUpdateData.bDown)
        {
            m_nDownDsskeyID = objUpdateData.m_nDsskeyID;
            bPaintFull = true;
        }
    }
    else
    {
        if (m_nDownDsskeyID == objUpdateData.m_nDsskeyID
            && !objUpdateData.bDown)
        {
            bPaintFull = true;
            m_nDownDsskeyID = 0;
        }
    }
    */
    if (bFlushLabel)
    {
        bool bPaintFull = true;

        //只有当需要刷新内容且不是翻页键时，才进行绘制
        //如果行数等于EXP_BTN_ROW_COUNT，则说明是翻页键
        if (objUpdateData.m_nRol != EXP_BTN_ROW_COUNT && (objUpdateData.bPaintIcon
                || objUpdateData.bPaintText))
        {
            //先更新单个按钮为无内容背景
            if (m_expPainter.ReloadCanvas(objUpdateData.m_nRol, objUpdateData.m_nCol, bPaintFull))
            {
                yl::string strText = objUpdateData.szLabel;
                yl::string strIcon = objUpdateData.szIcon;

                EXP_INFO("[Exp-Info]Update DsskeyData Text[%s],Iconpath[%s],IconType[%d], bFlushAtOnce[%d]\n",
                         strText.c_str(), strIcon.c_str(), objUpdateData.icon, bFlushAtOnce);

                //绘制按钮内容
                int nFlag = objUpdateData.m_nCol == 1 ? objUpdateData.flag : DSSKEY_ALIGN_NORMOL;
                m_expPainter.PaintBtn(objUpdateData.m_nRol
                                      , objUpdateData.m_nCol
                                      , objUpdateData.bDown
                                      , objUpdateData.m_eBgType
                                      , objUpdateData.bPaintIcon
                                      , objUpdateData.bPaintText
                                      , objUpdateData.bEditing ? DK_ICON_EDITING : objUpdateData.icon
                                      , objUpdateData.bEditing ? m_strEditingText : strText
                                      , strIcon
                                      , objUpdateData.bEditing ? PRESENCE_STATUS_NONE : objUpdateData.nStatus, nFlag);

                if (bFlushAtOnce)
                {
                    m_expRefresher.SyncImageToMem(m_expPainter.GetCanvas());
                    //单独刷新该按钮
                    m_expRefresher.RefreshButton(
                        objUpdateData.m_nIndex
                        , objUpdateData.m_nPage
                        , objUpdateData.m_nRol
                        , objUpdateData.m_nCol
                        , objUpdateData.bPaintIcon
                        , objUpdateData.bPaintText
                        , bPaintFull);
                }
            }
        }
    }

    if (bFlushLight)
    {
        //刷新灯
        if (objUpdateData.bLight)
        {
//          EXPCOLOR_INFO("[Exp-Info]Update DsskeyLight m_nIndex[%d],m_nRol[%d],,m_nCol[%d],ledStatus[%d]\n",
//              objUpdateData.m_nIndex, objUpdateData.m_nRol, objUpdateData.m_nCol, objUpdateData.ledStatus);

            m_expRefresher.RefreshLight(objUpdateData.m_nIndex, objUpdateData.m_nRol, objUpdateData.m_nCol,
                                        objUpdateData.ledStatus);
        }
    }
}

void EXPManager::OnUpdateBGData(ExpBgData pBGData, int nEXPCount)
{
    m_mutexUpdate.Lock();

    yl::string strBackGround = pBGData.szBackGround;
    m_expPainter.SetBackGround(strBackGround, pBGData.nTransparentLevel);

    m_mutexUpdate.Unlock();
}

void EXPManager::OnRefreshBG(int nEXPCount, int nEXPIndex)
{
    if (nEXPIndex >= nEXPCount
            || nEXPIndex >= EXP_MAX_COUNT)
    {
        return;
    }

    m_mutexListReceive.Lock();

    m_mutexListReceive.Unlock();
}

void EXPManager::OnUpdateLight(int nEXPCount, int nEXPIndex, int nLightLevel)
{
    EXP_INFO("[Exp-Info]EXPManager::OnUpdateLight nEXPCount %d, nEXPIndex %d, nLightLevel %d\n"
             , nEXPCount, nEXPIndex, nLightLevel);

    m_expRefresher.RefreshBackLight(nEXPIndex, nLightLevel);
    if (0 == nLightLevel)
    {
        EnterStatusPowerSaving(nEXPIndex);
    }
    else
    {
        ExitStatusPowerSaving(nEXPIndex);
    }
}

void EXPManager::OnMmapInfo(int nFileHandle, char * pFBMmap, unsigned int nFBMmapLength)
{
    EXP_INFO("[Exp-Info]EXPManager::OnMmapInfo %d, %d\n"
             , nFileHandle, nFBMmapLength);

    if (nFileHandle > 0)
    {
        m_bInit = true;
        m_expRefresher.SetMmapInfo(nFileHandle, pFBMmap, nFBMmapLength);
    }
}

void EXPManager::ResetAllLight(int nEXPCount, int nIndex)
{
    for (int nRow = 0; nRow < EXP_BTN_ROW_COUNT; ++nRow)
    {
        m_expRefresher.RefreshLight(nIndex, nRow, 0, 0);
        m_expRefresher.RefreshLight(nIndex, nRow, 1, 0);
    }

//  m_expRefresher.RefreshLight(nIndex, EXP_BTN_ROW_COUNT, 0, 0);
//  m_expRefresher.RefreshLight(nIndex, EXP_BTN_ROW_COUNT, 1, 0);
//  m_expRefresher.RefreshLight(nIndex, EXP_BTN_ROW_COUNT, 2, 0);
}

void EXPManager::UpdateEditingText(char * pStrLabel)
{
    m_strEditingText = pStrLabel;
}

void EXPManager::RefreshMemPage(int nIndex, int nMemPage)
{
    m_expRefresher.RefreshMemPage(nIndex, nMemPage);
}

void EXPManager::EXPFillWithColor(bool bUseLocal, EXP_FILL_COLOR objExpColor, int nIndex /*= 0*/,
                                  int iCanvasIndex /* = 1 */, int iFlushRightNow /* = 1 */)
{
    m_mutexUpdate.Lock();

    int nStartIndex = nIndex;
    int nTotalCount = 1;
    if (EXP_MAX_COUNT == nIndex)
    {
        nStartIndex = 0;
        nTotalCount = EXP_MAX_COUNT;
    }

    for (int i = nStartIndex; i < nStartIndex + nTotalCount; ++i)
    {
        if (bUseLocal)
        {
            m_expPainter.EXPFillWithColor(objExpColor);
            m_expRefresher.SyncImageToMem(m_expPainter.GetCanvas());
            m_expRefresher.RefreshScreen(i, iCanvasIndex, iFlushRightNow);
        }
        else
        {
#ifdef EXP_PAINT_ROTATE
            CMyImage qImageColor(EXP_SCREEN_HEIGHT, EXP_SCREEN_WIDTH, MYPAINTER_FORMAT_16);
            CMyPainter qPainterCav(qImageColor);

            qPainterCav.Translate(EXP_SCREEN_HEIGHT, 0);
            qPainterCav.Rotate(EXP_ROTATE_ANGLE);
#else
            CMyImage qImageColor(EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT, MYPAINTER_FORMAT_16);
            CMyPainter qPainterCav(qImageColor);
#endif

            m_expPainter.EXPFillWithColor(objExpColor, qPainterCav);
            m_expRefresher.SyncImageToMem(qImageColor);
            m_expRefresher.RefreshScreen(i, iCanvasIndex, iFlushRightNow);
        }
    }

    m_mutexUpdate.Unlock();
}

void EXPManager::EXPFillWithPicture(bool bUseLocal, const char * pStrPicture, int nIndex /*= 0*/,
                                    int iCanvasIndex /*= 1*/, int iFlushRightNow /*= 1*/)
{
    m_mutexUpdate.Lock();

    yl::string strPicture = pStrPicture;

    int nStartIndex = nIndex;
    int nTotalCount = 1;
    if (EXP_MAX_COUNT == nIndex)
    {
        nStartIndex = 0;
        nTotalCount = EXP_MAX_COUNT;
    }

    for (int i = nStartIndex; i < nStartIndex + nTotalCount; ++i)
    {
        if (bUseLocal)
        {
            m_expPainter.EXPFillWithPicture(strPicture);
            m_expRefresher.SyncImageToMem(m_expPainter.GetCanvas());
            m_expRefresher.RefreshScreen(i, iCanvasIndex, iFlushRightNow);
        }
        else
        {
#ifdef EXP_PAINT_ROTATE
            CMyImage qImagePic(EXP_SCREEN_HEIGHT, EXP_SCREEN_WIDTH, MYPAINTER_FORMAT_16);
            CMyPainter qPainterCav(qImagePic);

            qPainterCav.Translate(EXP_SCREEN_HEIGHT, 0);
            qPainterCav.Rotate(EXP_ROTATE_ANGLE);
#else
            CMyImage qImagePic(EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT, MYPAINTER_FORMAT_16);
            CMyPainter qPainterCav(qImagePic);
#endif

            m_expPainter.EXPFillWithPicture(strPicture, qPainterCav);
            m_expRefresher.SyncImageToMem(qImagePic);
            m_expRefresher.RefreshScreen(i, iCanvasIndex, iFlushRightNow);
        }
    }

    m_mutexUpdate.Unlock();
}

void EXPManager::EXPDrawText(const char * pStrText, int nIndex /*= 0*/)
{
    m_mutexUpdate.Lock();

    yl::string strText = pStrText;

    m_expPainter.EXPDrawText(strText);
    m_expRefresher.SyncImageToMem(m_expPainter.GetCanvas());
    m_expRefresher.RefreshScreen(nIndex);

    m_mutexUpdate.Unlock();
}

void EXPManager::EXPShowLogo(int nIndex /*= 0*/)
{
    m_expRefresher.RefreshLogo(nIndex);
}

void EXPManager::InitMmap()
{
    static int s_nEXPHandle = -1;
    if (s_nEXPHandle < 0)
    {
        s_nEXPHandle = expusb_helper_open(EXPUSB_FB);
    }

    EXP_INFO("[Exp-Info]EXPManager::InitMmap expusb_helper_open result[%d]", s_nEXPHandle);

    if (s_nEXPHandle < 0)
    {
        return;
    }

    fb_fix_screeninfo objFBInfo;
    int nRet = expusb_helper_fb_fscreeninfo(s_nEXPHandle, 0, &objFBInfo);
    if (nRet < 0)
    {
        EXP_WARN("[Exp-Info]EXPManager::InitMmap expusb_helper_fb_fscreeninfo Fail Ret = %d\n", nRet);
        return;
    }

    static unsigned int nFBMmapLength = objFBInfo.smem_len;

    static char * pFBMmap = NULL;

    if (NULL != pFBMmap)
    {
        EXP_INFO("[Exp-Info]EXPManager::InitMmap expusb_helper_munmap\n");
        expusb_helper_munmap(pFBMmap, nFBMmapLength);
    }

    pFBMmap = (char *)expusb_helper_mmap(s_nEXPHandle, nFBMmapLength);
    if (NULL == pFBMmap)
    {
        EXP_WARN("[Exp-Info]EXPManager::InitMmap expusb_helper_mmap Fail\n");
        return;
    }

    OnMmapInfo(s_nEXPHandle, pFBMmap, nFBMmapLength);
}

void EXPManager::EnterScreenSaver(bool bEnter)
{
    EXP_INFO("[Exp-Info]EnterScreenSaver bEnter %d\n", bEnter);

    if (bEnter)
    {
        EnterStatusScreenSaver();
    }
    else
    {
        ExitStatusScreenSaver();
    }
}

bool EXPManager::IsFullPageNeedRefresh(int nExpIndex) const
{
    if (nExpIndex < 0 || nExpIndex >= EXP_LIST_FLUSH_MAX_NUM
            || m_arrFlushInfo[nExpIndex][0] + 1 !=
            EXP_LIST_FLUSH_DSSKEY_MAX_NUM) //因初始值为EXP_LIST_FLUSH_DEFAULT_INDEX，故得加一补回
    {
        return false;
    }

    int nSize = m_vecExpInfoFlush.size();

    for (int j = 1; j < EXP_LIST_FLUSH_KEY_MAX_NUM + 1; ++j)
    {
        int nFlushInfoIndex = m_arrFlushInfo[nExpIndex][j];
        if (nFlushInfoIndex < nSize && nFlushInfoIndex >= 0)
        {
            if (!m_vecExpInfoFlush[nFlushInfoIndex].bLight
                    || !m_vecExpInfoFlush[nFlushInfoIndex].bPaintIcon
                    || !m_vecExpInfoFlush[nFlushInfoIndex].bPaintText)
            {
                return false;
            }
        }
    }

    return true;
}

void EXPManager::OnExpCountChange(int nNewCount)
{
    EXP_INFO("[Exp-Info]OnExpCountChange nNewCount %d, m_nExpCount %d\n"
             , nNewCount, m_nExpCount);

    m_nExpCount = nNewCount;

    for (int nEXPIndex = EXP_MAX_COUNT - 1; nEXPIndex >= nNewCount; --nEXPIndex)
    {
        m_nExpFirstFlush[nEXPIndex] = 0;
    }
}

bool EXPManager::GetFlushedByIndex(int nIndex)
{
    if (EXP_MAX_COUNT <= nIndex)
    {
        return false;
    }
    else
    {
        return 1 == m_nExpFirstFlush[nIndex];
    }
}

