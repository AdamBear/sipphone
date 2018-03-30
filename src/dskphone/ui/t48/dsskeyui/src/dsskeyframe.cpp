#include "dsskeyframe.h"
#include "dsskeybtnbase.h"
#include <QTimer>
// 图片ICON宏定义
// Set需求
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include <QStylePainter>
#include <QStyleOption>
#include "mainwnd/mainwnd.h"
#include "dsskey/include/moddsskey.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "moddsskeyperformance.h"

#define SAFE_DELETE_POINTER(p)      if ((p) != NULL) \
                                    { \
                                    delete (p); \
                                    (p) = NULL; \
                                    }

#define _DsskeyTimerManager (DsskeyTimerManager::GetInstance())

qDsskeyFrame::qDsskeyFrame(QWidget * parent)
    : QFrame(parent)
    , m_pChangePageBtn(NULL)
    , m_pBtnClickAction(NULL)
    , m_pBtnPressAction(NULL)
    , m_pBtnLongPressAction(NULL)
    , m_bCustomDisplay(true)
{
    // to do init m_pChangePageBtn

    connect(&_DsskeyTimerManager, SIGNAL(FlashTimerOut(DSS_KEY_FLASH_TYPE, bool))
            , this, SLOT(OnFlashTimerTimeout(DSS_KEY_FLASH_TYPE, bool)));
    g_pDsskeyUIManager->AddDsskeyFrame(this);
}

qDsskeyFrame::~qDsskeyFrame()
{
    SAFE_DELETE_POINTER(m_pBtnClickAction);
    SAFE_DELETE_POINTER(m_pBtnPressAction);
    SAFE_DELETE_POINTER(m_pBtnLongPressAction);
    ClearButton();
    g_pDsskeyUIManager->DeleteDsskeyFrame(this);
}

void qDsskeyFrame::ClearButton()
{
    YLList<qDsskeyButton *>::iterator it = m_listBtnPointer.begin();
    for (; it != m_listBtnPointer.end(); ++it)
    {
        (*it)->deleteLater();
        *it = NULL;
    }

    m_listBtnPointer.clear();
}

void qDsskeyFrame::AddDsskeyBtn(qDsskeyButton * pDsskeyBtn)
{
    if (NULL != pDsskeyBtn)
    {
        pDsskeyBtn->setParent(this);
        // to do 连接相关的信号槽
        m_listBtnPointer.push_back(pDsskeyBtn);
        connect(pDsskeyBtn, SIGNAL(btnClicked(int)), this, SLOT(OnDsskeyBtnClick(int)));
        connect(pDsskeyBtn, SIGNAL(btnPress(int)), this, SLOT(OnDsskeyPress(int)));
        connect(pDsskeyBtn, SIGNAL(btnLongPress(int)), this, SLOT(OnDsskeyLongPress(int)));
        connect(pDsskeyBtn, SIGNAL(startFlash(DSS_KEY_FLASH_TYPE)), this,
                SLOT(OnStartFlash(DSS_KEY_FLASH_TYPE)));
        connect(pDsskeyBtn, SIGNAL(stopFlash(DSS_KEY_FLASH_TYPE)), this,
                SLOT(OnStopFlash(DSS_KEY_FLASH_TYPE)));
    }
}

// 添加UIPushButton
void qDsskeyFrame::AddUIPushBtn(qUIPushButton * pPushBtn, QObject * obj, const char * szMember)
{
    if (NULL != pPushBtn)
    {
        // 添加普通点击按钮
        pPushBtn->setParent(this);
        m_pChangePageBtn = pPushBtn;
        if (NULL != obj && NULL != szMember)
        {
            // 链接外部响应函数
            connect(pPushBtn, SIGNAL(clicked()), obj, szMember);
        }
    }
}

void qDsskeyFrame::RefreshDsskeyData(const DsskeyDisplayData & objDisplayData)
{
    qDsskeyButton * pBtn = findBtnByIndex(objDisplayData.iIndex);
    if (NULL != pBtn)
    {
        pBtn->UpdateDisplayStatus(objDisplayData);
    }
}

void qDsskeyFrame::RefreshDsskeyData(int iIndex, const DsskeyDisplayData & objDisplayData)
{
    qDsskeyButton * pBtn = findBtnByIndex(iIndex);
    if (NULL != pBtn)
    {
        pBtn->UpdateDisplayStatus(objDisplayData);
    }
}

void qDsskeyFrame::RefreshDsskeyGroundPic(const DsskeyDisplayData & objDisplayData)
{
    qDsskeyButton * pBtn = findBtnByIndex(objDisplayData.iIndex);
    if (NULL != pBtn)
    {
        pBtn->UpdateGroudPic(objDisplayData);
        //pBtn->UpdateDisplayStatus(objDisplayData);
    }
}

void qDsskeyFrame::RefreshAllDsskey()
{
    YLList<qDsskeyButton *>::const_iterator it = m_listBtnPointer.begin();
    DsskeyDisplayData   DisplayData;
    DssKeyLogicData     logicData;
    int                 iDsskeyID;
    bool bret = false;

    for (/* */; it != m_listBtnPointer.end(); ++it)
    {
        qDsskeyButton * pBtn = static_cast<qDsskeyButton *>(*it);
        if (NULL != pBtn)
        {
            iDsskeyID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, pBtn->GetIndex());
#if 1
            dsskey_refreshDSSKeyUI(iDsskeyID);
#else
            bret = dsskey_GetData(iDsskeyID, logicData);
            if (!bret)
            {
                DSSKEYUI_WARN("RefreshAllDsskey : dsskey_GetData Failed.");
                continue;
            }
            //T48 更新数据不需要再判断锁定等操作
            bret = DP_ParseDsskeyPerformance(DisplayData, logicData);
            if (bret == true)
            {
                RefreshDsskeyData(DisplayData);
            }
            else
            {
                DSSKEYUI_DEBUG("RefreshAllDsskey : DP_ParseDsskeyPerformance Failed.");
            }

#if IF_BUG_XMLBROWSER_EXP_LED
            if (logicData.eLightType != LT_NONE)
            {
                dsskeyUI_DsskeyLightCallback(iDsskeyID, dsskeyUI_GetColorType(logicData.eLightType),
                                             dsskeyUI_GetFlushType(logicData.eLightType));
            }
#endif
#endif
        }
    }
}

void qDsskeyFrame::showEvent(QShowEvent * pShowEvent)
{

}

void qDsskeyFrame::hideEvent(QHideEvent * pHideEvent)
{

}

void qDsskeyFrame::AdjustBtnGeometry()
{
    // 排布btn位置
}

void qDsskeyFrame::SetBtnClickAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnClickAction);
    m_pBtnClickAction = pAction;
}

void qDsskeyFrame::SetBtnPressAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnPressAction);
    m_pBtnPressAction = pAction;
}

void qDsskeyFrame::SetBtnLongPressAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnLongPressAction);
    m_pBtnLongPressAction = pAction;
}

void qDsskeyFrame::OnDsskeyBtnClick(int iIndex)
{
    //mainwnd处理完了就不用再处理
    if (_MainWND.OnLineKeyClick(iIndex))
    {
        return;
    }

    if (NULL != m_pBtnClickAction)
    {
        m_pBtnClickAction->OnDsskeyAction(iIndex);
    }
}

void qDsskeyFrame::OnDsskeyPress(int iIndex)
{
    if (NULL != m_pBtnPressAction)
    {
        m_pBtnPressAction->OnDsskeyAction(iIndex);
    }
}

void qDsskeyFrame::OnDsskeyLongPress(int iIndex)
{
    if (NULL != m_pBtnLongPressAction)
    {
        m_pBtnLongPressAction->OnDsskeyAction(iIndex);
    }
}

void qDsskeyFrame::OnStartFlash(DSS_KEY_FLASH_TYPE eFlashType)
{
    DSSKEYUI_DEBUG("OnStartFlash : eFlashType is %d.", eFlashType);
    // 闪烁类型正确才开始定时器
    if (eFlashType > REFRESH_TYPE_NO_FLASH  && (eFlashType < REFRESH_TYPE_END))
    {
        YLList<qDsskeyButton *>::iterator it = m_listBtnPointer.begin();
        for (/* */; it != m_listBtnPointer.end(); ++it)
        {
            if (NULL != (*it) && (*it)->GetFlashType() == eFlashType)
            {
                return;
            }
        }
        _DsskeyTimerManager.StartFlash(eFlashType);
    }
}

void qDsskeyFrame::OnStopFlash(DSS_KEY_FLASH_TYPE eFlashType)
{
    DSSKEYUI_DEBUG("OnStopFlasheFlashType is %d.", eFlashType);
    // 闪烁类型正确才检索
    if (eFlashType > REFRESH_TYPE_NO_FLASH  && (eFlashType < REFRESH_TYPE_END))
    {
        YLList<qDsskeyButton *>::iterator it = m_listBtnPointer.begin();
        for (/* */; it != m_listBtnPointer.end(); ++it)
        {
            if (NULL != (*it) && (*it)->GetFlashType() == eFlashType)
            {
                return;
            }
        }
        _DsskeyTimerManager.StopFlash(eFlashType);
    }
}

void qDsskeyFrame::OnFlashTimerTimeout(DSS_KEY_FLASH_TYPE eFlashType, bool bEnable)
{
    YLList<qDsskeyButton *>::iterator it = m_listBtnPointer.begin();
    for (/* */; it != m_listBtnPointer.end(); ++it)
    {
        if (NULL != (*it) && (*it)->GetFlashType() == eFlashType)
        {
            (*it)->FlashButton(bEnable);
        }
    }
}


qDsskeyButton * qDsskeyFrame::findBtnByIndex(int iBtnIndex)
{
    YLList<qDsskeyButton *>::iterator it = m_listBtnPointer.begin();
    qDsskeyButton * pDsskeyBtn = NULL;
    for (/* */; it != m_listBtnPointer.end(); ++it)
    {
        if (NULL != (*it) && (*it)->GetIndex() == iBtnIndex)
        {
            pDsskeyBtn = (qDsskeyButton *)(*it);
        }
    }
    return pDsskeyBtn;
}

bool qDsskeyFrame::SetDisplayDelegate(int iBtnIndex, DsskeyBtnDisplayDelegete * pDelegate)
{
    bool bRet = false;
    if (IsSupportCustomDisplay())
    {
        qDsskeyButton * pDsskeyBtn = findBtnByIndex(iBtnIndex);
        if (NULL != pDsskeyBtn)
        {
            bRet = pDsskeyBtn->SetDisplayDelegate(pDelegate);
        }
    }
    return bRet;
}

void qDsskeyFrame::DebindDisplayDelegate(int iBtnIndex)
{
    if (IsSupportCustomDisplay())
    {
        qDsskeyButton * pDsskeyBtn = findBtnByIndex(iBtnIndex);
        if (NULL != pDsskeyBtn)
        {
            pDsskeyBtn->DebindDisplayDelegate();
        }
    }
}

//////////////////////// DsskeyTimerManager ///////////////////////////////

DsskeyTimerManager::DsskeyTimerManager()
{
    m_MapFlashTimer.clear();
}

DsskeyTimerManager::~DsskeyTimerManager()
{

}

DsskeyTimerManager & DsskeyTimerManager::GetInstance()
{
    static DsskeyTimerManager s_InstTimerMng;
    return s_InstTimerMng;
}

void DsskeyTimerManager::StartFlash(DSS_KEY_FLASH_TYPE eFlashType)
{
    // 类型错误，直接返回
    if ((eFlashType < REFRESH_TYPE_NO_FLASH) || (eFlashType >= REFRESH_TYPE_END))
    {
        DSSKEYUI_INFO("eFlashType[%d] is not supported!");
        return;
    }

    IterFlashTimer it = m_MapFlashTimer.begin();
    for (/* */; it != m_MapFlashTimer.end(); ++it)
    {
        if ((NULL != it->second) && (it->first == eFlashType))
        {
            if (NULL != it->second->pTimer)
            {
                // 找到，则使用计数加1
                it->second->nCount++;
                DSSKEYUI_INFO("TM :: Start eFlsushType[%d], nCount[%d]", eFlashType, it->second->nCount);
                return;
            }
        }
    }
    FlashTimer * pFlashTimer = new FlashTimer;
    QTimer * pTimer = new QTimer;
    switch (eFlashType)
    {
    case REFRESH_TYPE_SLOW_FLASH:
        {
            // 慢闪动1000ms
            pTimer->setInterval(1000);
            connect(pTimer, SIGNAL(timeout()), this, SLOT(OnSLowTimerOut()));
        }
        break;
    case REFRESH_TYPE_FLASH:
        {
            // 闪动500ms
            pTimer->setInterval(500);
            connect(pTimer, SIGNAL(timeout()), this, SLOT(OnFlashTimerOut()));
        }
        break;
    case REFRESH_TYPE_FAST_FLASH:
        {
            // 闪动500ms
            pTimer->setInterval(500);
            connect(pTimer, SIGNAL(timeout()), this, SLOT(OnFastFlashTimerOut()));
        }
        break;
    case REFRESH_TYPE_FASTER_FLASH:
        {
            // 闪动500ms
            pTimer->setInterval(500);
            connect(pTimer, SIGNAL(timeout()), this, SLOT(OnFasterFlashTimerOut()));
        }
        break;
    case REFRESH_TYPE_ALTERNATE_FLASH:
        {
            // 图标切换闪动 500ms
            pTimer->setInterval(500);
            connect(pTimer, SIGNAL(timeout()), this, SLOT(OnAlternateFlashTimerOut()));
        }
        break;
    default:
        break;
    }

    pTimer->start();
    // 初始化新建的定时器容器
    pFlashTimer->pTimer = pTimer;
    pFlashTimer->nCount = 1;
    m_MapFlashTimer.put(eFlashType, pFlashTimer);
}

void DsskeyTimerManager::StopFlash(DSS_KEY_FLASH_TYPE eFlashType)
{
    IterFlashTimer it = m_MapFlashTimer.begin();
    for (/* */; it != m_MapFlashTimer.end(); ++it)
    {
        if ((NULL != it->second) && (it->first == eFlashType))
        {
            // 当前对应定时器在使用
            if (NULL != it->second->pTimer)
            {
                // 找到，则使用计数减1
                it->second->nCount--;
                DSSKEYUI_INFO("TM :: Stop eFlsushType[%d], nCount[%d]", eFlashType, it->second->nCount);
                if (0 == it->second->nCount)
                {
                    // 停止定时器
                    it->second->pTimer->stop();
                    delete (it->second->pTimer);
                    it->second->pTimer = NULL;
                    it->second->nCount = 0;

                    // 移除FlashTimer
                    delete (it->second);
                    it->second = NULL;
                    m_MapFlashTimer.erase(it);

                    break;
                }
            }
        }
    }
}

void DsskeyTimerManager::OnSLowTimerOut()
{
    static bool bEnable = false;
    // 1000ms亮,1000ms灭
    bEnable = !bEnable;
    emit FlashTimerOut(REFRESH_TYPE_SLOW_FLASH, bEnable);
}

void DsskeyTimerManager::OnFlashTimerOut()
{
    static bool bEnable = false;
    // 500ms亮,500ms灭
    bEnable = !bEnable;
    emit FlashTimerOut(REFRESH_TYPE_FLASH, bEnable);
}

void DsskeyTimerManager::OnFastFlashTimerOut()
{
    static bool bEnable = false;
    // 500ms亮,500ms灭
    bEnable = !bEnable;
    emit FlashTimerOut(REFRESH_TYPE_FAST_FLASH, bEnable);
}

void DsskeyTimerManager::OnFasterFlashTimerOut()
{
    static bool bEnable = false;
    // 500ms亮,500ms灭
    bEnable = !bEnable;
    emit FlashTimerOut(REFRESH_TYPE_FASTER_FLASH, bEnable);
}

void DsskeyTimerManager::OnAlternateFlashTimerOut()
{
    static bool bEnable = false;
    // 500ms亮,500ms灭
    bEnable = !bEnable;
    emit FlashTimerOut(REFRESH_TYPE_ALTERNATE_FLASH, bEnable);
}

qSettingDsskeyFrame::qSettingDsskeyFrame(QWidget * parent)
    : qDsskeyFrame(parent)
{
    // 设置界面不支持自定义显示
    SetCustomDisplayEnable(false);
}
qSettingDsskeyFrame::~qSettingDsskeyFrame()
{

}
void qSettingDsskeyFrame::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;
    QRect rcFrame = frameRect();
    pm = THEME_GET_BMP(PIC_DSS_SET_BACKGROUND);
    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(rcFrame.x() + (rcFrame.width() - pm.width()) / 2,
                     rcFrame.y() + rcFrame.height() / 2 - pm.height() / 2, pm);
    }
}
