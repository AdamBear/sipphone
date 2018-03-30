#include "expmanagerblack.h"
#include "expdefine.h"
#include "devicelib/phonedevice.h"
#include <sys/time.h>
#include <ylstl/ylstringalgorithm.h>
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"
#include "updatelistthread.h"

namespace
{
#define VP_PHONE_THREAD_NAME                    "app_vpPhone"
#define EXP_DEV_NAME                            "/dev/exp0"
#define EXP_LIST_FLUSH_MAX_NUM                  5
#define EXP_LIST_FLUSH_DSSKEY_MAX_NUM       20  //EXPdsskey按键个数
#define EXP_LIST_FLUSH_KEY_MAX_NUM          23    //EXP总按键个数
#define EXP_LIST_FLUSH_DEFAULT_INDEX            (-1)

#define EXP_INIT_DELAY_TIME                             500
}

EXPManagerBlack::EXPManagerBlack()
    : m_vecExpInfoFlush(EXP_SEND_LIST_MAX_COUNT)
    , m_nExpCount(0)
{
    m_vecExpInfoReceive.clear();
    m_vecExpInfoFlush.clear();

    for (int i = 0; i < EXP_MAX_COUNT; ++i)
    {
        m_nExpFirstFlush[i] = 0;
    }
}


EXPManagerBlack::~EXPManagerBlack()
{
    m_mutexUpdate.Unlock();
    m_mutexListReceive.Unlock();
    m_mutexListFlush.Unlock();
}

EXPManagerBlack & GetEXPManagerInstance()
{
    // 用函数内static方式控制全局对象的构造顺序
    static EXPManagerBlack s_EXPManager;
    return s_EXPManager;
}

void EXPManagerBlack::Init()
{
    m_expPainter.Init();

    InitMmap();

    TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_EXP_UPDATE_LIST,
                                   TA_THREAD_EXP_UPDATE_LIST,
                                   CUpdateListThread::OnMessage);

    _ExpUpdateListThread.SetEXPManager(this);
}

void EXPManagerBlack::Uninit()
{
    m_mutexUpdate.Unlock();
    m_mutexListReceive.Unlock();
    m_mutexListFlush.Unlock();

    m_expPainter.Uninit();
}

LRESULT EXPManagerBlack::OnEXPMessage(msgObject & objMessage)
{
    EXP_INFO("EXPManager::OnEXPMessage[%d],wParam[%d],lParam[%d]\n", objMessage.message,
             objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case EXP_MESSAGE_BACKLIGHT:
        OnUpdateLight(objMessage.wParam, objMessage.lParam);

        break;

    case EXP_MESSAGE_COUNT_CHANGE:

        OnExpCountChange(objMessage.wParam);

        break;

    case EXP_MESSAGE_SET_CONTRAST:

        OnUpdateContrast(objMessage.wParam, objMessage.lParam);

        break;

    case EXP_MESSAGE_SWITCH_PAGE:

        OnSwitchPage(objMessage.wParam, objMessage.lParam);

        break;

    default:

        return EXPManagerBase::OnEXPMessage(objMessage);

        break;
    }

    return 0;
}

void EXPManagerBlack::UpdateList()
{
    //EXP_INFO("exp UpdateList loop begin");
    while (true)
    {

        m_mutexListFlush.Lock();
        m_mutexUpdate.Lock();

        /*int nSize = m_vecExpInfoFlush.size();
        int nEXPNum = EXP_LIST_FLUSH_DEFAULT_INDEX;

        UpdateMsgDispose(m_vecExpInfoFlush, nEXPNum);

        for (int i = 0; i <= nEXPNum; ++i)
        {
        if (m_arrbWithData[i])
        {
        for (int j = 1; j < EXP_LIST_FLUSH_KEY_MAX_NUM + 1; ++j)
        {
        if (m_arrFlushInfo[i][j] < nSize && m_arrFlushInfo[i][j] >= 0)
        {
        UpdateDsskeyData(m_vecExpInfoFlush[m_arrFlushInfo[i][j]]);
        }
        }
        }

        EXP_INFO("[Exp-info]Flush ExpList Port[%d] End\n", i);
        }*/
        for (vecExpItemInfo::iterator it = m_vecExpInfoFlush.begin(); it != m_vecExpInfoFlush.end(); it++)
        {
            UpdateDsskeyData(*it);
        }
        EXP_INFO("exp UpdateList: receive buffer seize[%d]", m_vecExpInfoReceive.size());
        m_vecExpInfoFlush.clear();

        m_mutexListFlush.Unlock();

        if (m_vecExpInfoReceive.size() > 0)
        {
            EXP_INFO("exp UpdateList: receive buffer to flush buffer");
            m_mutexListReceive.Lock();

            m_vecExpInfoFlush = m_vecExpInfoReceive;
            m_vecExpInfoReceive.clear();

            m_mutexListReceive.Unlock();
            m_mutexUpdate.Unlock();
        }
        else
        {

            m_mutexUpdate.Unlock();
            return;
        }
    }
    //EXP_INFO("exp UpdateList loop end");

}

void EXPManagerBlack::UpdateDsskeyData(const ExpDisplayItemInfo & objUpdateData)
{

    if (objUpdateData.bPaintIcon && !objUpdateData.bPaintText)
    {
        m_expPainter.PaintLcdIcon(objUpdateData.m_nIndex, objUpdateData.m_nPage
                                  , objUpdateData.m_nRol, objUpdateData.m_nCol, objUpdateData.icon);
    }
    else if (objUpdateData.bPaintText)
    {
        yl::string strExpLabel = (char *)objUpdateData.szLabel;
        yl::wstring wstrLabel = yl::to_utf16(strExpLabel.c_str());
        EXP_INFO("Exp PaintEXPLcdItem: display flags[%x]", objUpdateData.flag);
        m_expPainter.PaintEXPLcdItem(objUpdateData.m_nIndex, objUpdateData.m_nPage,
                                     objUpdateData.nCurrentPage
                                     , objUpdateData.m_nRol, objUpdateData.m_nCol, objUpdateData.icon
                                     , objUpdateData.flag, wstrLabel.length() * 2, (char *)objUpdateData.szLabel);
    }

    if (objUpdateData.bLight)
    {
        m_expPainter.PaintLedStatus(objUpdateData.m_nIndex, objUpdateData.m_nRol
                                    , objUpdateData.m_nCol, objUpdateData.ledStatus);
    }
}

void EXPManagerBlack::OnUpdateLight(int nEXPIndex, int nLightLevel)
{
    EXP_INFO("[Exp-Info]EXPManager::OnUpdateLight  nEXPIndex %d, nLightLevel %d\n", nEXPIndex,
             nLightLevel);

    m_expPainter.SetExpBackLight(nEXPIndex, nLightLevel);
}

void EXPManagerBlack::OnUpdateContrast(int nEXPIndex, int nContrastLevel)
{
    EXP_INFO("[Exp-Info]EXPManager::OnUpdateContrast nEXPIndex %d, nContrastLevel %d\n", nEXPIndex,
             nContrastLevel);

    m_expPainter.SetExpLCDContrast(nEXPIndex, nContrastLevel);
}

void EXPManagerBlack::OnSwitchPage(int nEXPIndex, int nPage)
{
    m_expPainter.SwitchPage(nEXPIndex, nPage);
}

void EXPManagerBlack::OnMmapInfo(int nFileHandle)
{
    if (nFileHandle > 0)
    {
        m_bInit = true;
        m_expPainter.SetMmapInfo(nFileHandle);
    }
}

void EXPManagerBlack::InitMmap()
{
    static int s_nEXPHandle = -1;
    if (s_nEXPHandle < 0)
    {
        s_nEXPHandle = open(EXP_DEV_NAME, O_RDWR);
    }

    EXP_INFO("[Exp-Info]EXPManager::InitMmap expusb_helper_open Ret = %d\n", s_nEXPHandle);

    if (s_nEXPHandle < 0)
    {
        return;
    }

    OnMmapInfo(s_nEXPHandle);
}

void EXPManagerBlack::OnExpCountChange(int nNewCount)
{
    EXP_INFO("[Exp-Info]OnExpCountChange nNewCount %d, m_nExpCount %d\n"
             , nNewCount, m_nExpCount);

    m_nExpCount = nNewCount;

    for (int nEXPIndex = EXP_MAX_COUNT - 1; nEXPIndex >= nNewCount; --nEXPIndex)
    {
        m_nExpFirstFlush[nEXPIndex] = 0;
    }
}

bool EXPManagerBlack::GetFlushedByIndex(int nIndex)
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

