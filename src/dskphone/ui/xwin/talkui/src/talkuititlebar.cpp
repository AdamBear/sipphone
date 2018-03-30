#include "talkuititlebar.h"
#include "xwindow/xWindowManager.h"
#include "talkuicommonfunction.h"
#include "talkuixmlitemname.h"
#include "talkuipath.h"

#if IF_TALKUI_USB_ENABLE
#include "record/include/modrecord.h"
#endif

namespace talkui
{

CTalkUITitleBar::CTalkUITitleBar()
    : m_pMyView(NULL)
    , m_iIconCount(0)//图标个数
    , m_pChannelIcon(NULL)
    , m_pTitleContent(NULL)
    , m_pPageNum(NULL)
#if IF_TALKUI_USB_ENABLE
    , m_pRecordWidget(NULL)
    , m_pRecordIcon(NULL)
    , m_pRecordTime(NULL)
#endif
{

}

CTalkUITitleBar::~CTalkUITitleBar()
{

}

void CTalkUITitleBar::LoadWidget(xView * pLayout)
{
    if (NULL == pLayout)
    {
        return ;
    }

    m_pMyView = (xView *)pLayout->getViewById(TALKUI_TITLEBAR_ID);      //自身布局

    //通道图标
    m_pChannelIcon = (xImageView *)pLayout->getViewById(
                         TALKUI_TITLEBAR_CHANNELICON_ID);    //通道图标

    ////显示的title
    m_pTitleContent = (xTextView *)pLayout->getViewById(
                          TALKUI_TITLEBAR_TITLECONTENT_ID);       //显示的通话title

#if IF_TALKUI_USB_ENABLE
    //录音图标
    m_pRecordWidget = static_cast<xView *>(pLayout->getViewById(TALKUI_TITLEBAR_RECORD_WIDGET));
    //录音图标
    m_pRecordIcon = static_cast<xImageView *>(pLayout->getViewById(TALKUI_TITLEBAR_RECORD_ICON));    //通道图标
    //录音时间
    m_pRecordTime = static_cast<xTimerPieceView *>(pLayout->getViewById(TALKUI_TITLEBAR_RECORD_TIME));
#endif

    char szXMLNode[256] = { 0 };

    //图标数组
    for (int i = 0;; i++)
    {
        sprintf(szXMLNode, "%s%d", TALKUI_TITLEBAR_ICON_ID, i);
        xImageView * pIcon = (xImageView *)(pLayout->getViewById(szXMLNode));     //通话图标
        if (NULL != pIcon)
        {
            m_vectorIconView.push_back(pIcon);
        }
        else        //没有获取到控件 退出循环
        {
            break;
        }
    }
    //  talkui_vector<xImageView*> m_vectorIcons;

    //显示的页码
    m_pPageNum = (xTextView *)pLayout->getViewById(TALKUI_TITLEBAR_PAGENUM_ID);
}

//初始化
void CTalkUITitleBar::Init()
{
    //设置title 为空
    SetTitle("");

    //清除所有图标
    ClearAllIcons();

    //隐藏页码
    HidePageNum();

#if IF_TALKUI_USB_ENABLE
    StopRecord();
#endif
}

//反初始化
void CTalkUITitleBar::Uninit()
{
#if IF_TALKUI_USB_ENABLE
    if (NULL != m_pRecordTime)
    {
        m_pRecordTime->StopTimer();
        m_pRecordTime->ResetTimer();
    }
#endif
}

void CTalkUITitleBar::Show()
{
    if (NULL != m_pMyView)
    {
        m_pMyView->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

void CTalkUITitleBar::Hide()
{
    if (NULL != m_pMyView
            && m_pMyView->isVisible())
    {
        m_pMyView->hide(true);
    }
}

//设置通道图标
void CTalkUITitleBar::SetChannelIcon(const yl::string & strChannelIcon, bool bShow/* = true*/)
{
    if (NULL != m_pChannelIcon)
    {
        if (bShow)
        {
            if (!m_pChannelIcon->isVisible())
            {
                m_pChannelIcon->show();
            }
            m_pChannelIcon->setPixmap(strChannelIcon);
        }
        else
        {
            m_pChannelIcon->hide(true);
        }
    }
}

//设置显示title
void CTalkUITitleBar::SetTitle(const yl::string & strTitle)
{
    if (NULL != m_pTitleContent)
    {
        m_pTitleContent->SetText(strTitle);
    }
}

void CTalkUITitleBar::SetTitleScroll(bool bScroll)
{
    if (NULL != m_pTitleContent)
    {
        if (bScroll)
        {
            m_pTitleContent->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pTitleContent->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }
}

//设置显示页码
void CTalkUITitleBar::SetPageNum(int iPageIndex, int iPageCount)
{
    if (NULL != m_pPageNum)
    {
        char szPageNum[16] = {0};
        sprintf(szPageNum, "%d/%d", iPageIndex, iPageCount);
        yl::string strPageNum(szPageNum);

        m_pPageNum->SetText(strPageNum);

        m_pPageNum->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

//隐藏页码 也就是不需要显示页码
void CTalkUITitleBar::HidePageNum()
{
    if (NULL != m_pPageNum)
    {
        m_pPageNum->hide(true);
    }
}

//清除所有图标  这边不包含通道图标
void CTalkUITitleBar::ClearAllIcons()
{
    int iVectorIconSize = m_vectorIconView.size();

    for (int i = 0; i < iVectorIconSize; i++)
    {
        xImageView * pIcon = m_vectorIconView[i];
        if (NULL != pIcon)
        {
            pIcon->hide(true);      //把位置空出来
        }
    }

    //清除所有图标路径
    m_listIconString.clear();

    m_iIconCount = 0;       //图标个数
}

//添加图标  这边不包含通道图标
void CTalkUITitleBar::AddIcon(const yl::string & strIcon)
{
    //已经显示不下图标了 直接返回
    if (m_iIconCount >= m_vectorIconView.size())
    {
        return ;
    }

    talkui_list<yl::string>::iterator listitor = m_listIconString.begin();

    for (; m_listIconString.end() != listitor; listitor++)
    {
        yl::string & strSaveIcon = *listitor;

        //有相同的就直接返回 不需要添加了
        if (strSaveIcon == strIcon)
        {
            return;
        }
    }

    xPixmap pixmapIcon = GetPixmapByString(strIcon);

    //获取不到图标直接返回
    if (pixmapIcon.isNull())
    {
        return ;
    }

    //插入到数组列表末尾
    m_listIconString.push_back(strIcon);

    //显示在 图标控件上
    xImageView * pIcon = m_vectorIconView[m_iIconCount];
    if (NULL != pIcon)
    {
        pIcon->setPixmap(pixmapIcon);
        pIcon->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    //显示的图标个数一定要加1
    m_iIconCount++;
}

//删除图标  不包含通道图标
void CTalkUITitleBar::DeleteIcon(const yl::string & strIcon)
{
    int iListCount = m_listIconString.size();
    int iIconIndex = 0;
    talkui_list<yl::string>::iterator listitor = m_listIconString.begin();

    for (; m_listIconString.end() != listitor; listitor++, iIconIndex++)
    {
        yl::string & strSaveIcon = *listitor;

        //有相同的就退出
        if (strSaveIcon == strIcon)
        {
            break;
        }
    }

    //越界 没有找到相同的元素
    if (iIconIndex >= iListCount)
    {
        return ;
    }

    //先删除图标路径
    m_listIconString.erase(listitor);

    //获取图标控件个数
    int iImageViewCount = m_vectorIconView.size();
    //先隐藏所有图标
    for (int i = 0; i < iImageViewCount; i++)
    {
        xImageView * pIcon = m_vectorIconView[i];
        if (NULL != pIcon)
        {
            pIcon->hide(true);      //把位置空出来
        }
    }

    talkui_list<yl::string>::iterator listitorAdd = m_listIconString.begin();
    int iAddIndex = 0;
    //重新设置图标
    m_iIconCount = 0;
    for (; m_listIconString.end() != listitorAdd; listitorAdd++, iAddIndex++)
    {
        if (iAddIndex < iImageViewCount)
        {
            yl::string & strAddIcon = *listitorAdd;
            xPixmap pixmapIcon = GetPixmapByString(strAddIcon);
            xImageView * pIconAdd = m_vectorIconView[iAddIndex];
            if (NULL != pIconAdd)
            {
                pIconAdd->setPixmap(pixmapIcon);
                pIconAdd->show(xView::SHOW_CMD_BRING_TO_TOP);
                m_iIconCount++;
            }
        }
        else
        {
            break;
        }
    }
}

#if IF_TALKUI_USB_ENABLE
void CTalkUITitleBar::StartRecord()
{
    if (NULL != m_pRecordTime)
    {
        m_pRecordTime->show();
        m_pRecordTime->PauseTimer();
        m_pRecordTime->ResumeTimer();
    }

    if (NULL != m_pRecordWidget)
    {
        m_pRecordWidget->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    if (NULL != m_pRecordIcon)
    {
        m_pRecordIcon->setPixmap(PIC_RECORD_NORMAL);
        m_pRecordIcon->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

void CTalkUITitleBar::PauseRecord()
{
    if (NULL != m_pRecordTime)
    {
        m_pRecordTime->show();
        m_pRecordTime->PauseTimer();
    }

    if (NULL != m_pRecordWidget)
    {
        m_pRecordWidget->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    if (NULL != m_pRecordIcon)
    {
        m_pRecordIcon->setPixmap(PIC_RECORD_PAUSE);
        m_pRecordIcon->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

void CTalkUITitleBar::StopRecord()
{
    if (NULL != m_pRecordTime)
    {
        m_pRecordTime->StopTimer();
        m_pRecordTime->hide(true);
    }

    if (NULL != m_pRecordWidget)
    {
        m_pRecordWidget->hide(false);
    }

    if (NULL != m_pRecordIcon)
    {
        m_pRecordIcon->hide(true);
    }
}

void CTalkUITitleBar::SyncRecordTime()
{
    if (NULL == m_pRecordTime)
    {
        return;
    }

#if IF_TALKUI_USB_ENABLE
    m_pRecordTime->StopTimer();

    UINT uHour = 0;
    UINT uMinute = 0;
    UINT uSecond = 0;
    modRecord_GetRecordTime(uHour, uMinute, uSecond, RECORD_AUDIO);
    unsigned int nTime = uHour * 3600 + uMinute * 60 + uSecond;
    m_pRecordTime->SetStartDuration(nTime);

    if (modRecord_IsRecording(RECORD_AUDIO))
    {
        StartRecord();
    }
    else if (modRecord_IsRecordPause(RECORD_AUDIO))
    {
        PauseRecord();
    }
#endif
}

#endif

}       //namespace talkui


