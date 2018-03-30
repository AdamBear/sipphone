#include "talkuidtmf.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "dsklog/log.h"

#include "talkuicommonfunction.h"


namespace talkui
{



//DTMF条自动隐藏时间
#define DTMFLINE_AUTO_HIDE_TIME     5000
#define DTMF_DELAY_TIME             400             //延时加密时间400毫秒

//密码字符
const yl::string g_strPasswordKey("*");

CTalkUIDtmf::CTalkUIDtmf()
    : m_pEditTextView(NULL)     //编辑框
//是否以密码模式显示
    , m_bIsPassword(false)
//如果以密码模式显示，是否延时加密
    , m_bDelayToStar(false)
{
    InitWidget();

}

CTalkUIDtmf::~CTalkUIDtmf()
{
    //停止两个定时器
    StopSetToSarTimer();

    StopAutoHideTimer();
}

//加载控件
void CTalkUIDtmf::SetDtmfWidget(xEditTextView * pEditTextView)
{
    //先停止两个定时器
    StopAutoHideTimer();
    StopSetToSarTimer();

    //断开之前的槽
    if (NULL != m_pEditTextView)
    {
        SLOT_DISCONNECT(m_pEditTextView, signalTextConfirmChange, this,
                        talkui::CTalkUIDtmf::slotEditChanged);
    }

    //赋值当前编辑框
    m_pEditTextView = pEditTextView;

    if (NULL != m_pEditTextView)
    {
        SLOT_CONNECT(m_pEditTextView, signalTextConfirmChange, this, talkui::CTalkUIDtmf::slotEditChanged);
    }

    //隐藏控件
    Hide();

    //清除数据
    ClearData();
}

//释放控件
void CTalkUIDtmf::FreeWidget()
{
    //先停止两个定时器
    StopAutoHideTimer();
    StopSetToSarTimer();

    //先隐藏
    Hide();

    //断开之前的槽
    if (NULL != m_pEditTextView)
    {
        SLOT_DISCONNECT(m_pEditTextView, signalTextConfirmChange, this,
                        talkui::CTalkUIDtmf::slotEditChanged);
    }

    m_pEditTextView = NULL;
}

bool CTalkUIDtmf::GetDtmfVisible()
{
    if (NULL == m_pEditTextView)
    {
        return false;
    }

    return m_pEditTextView->isVisible();
}

//设置DTMF栏可见性
void CTalkUIDtmf::SetDtmfLineVisible(bool bVisible)
{
    if (bVisible == IsVisible())
    {
        //可见性一致，不设置
        return;
    }

    //先停止 自动隐藏定时器
    StopAutoHideTimer();

    if (bVisible)
    {
        //在每次显示时都读取配置，判断其输入模式
        m_bIsPassword = (configParser_GetConfigInt(kszEnableHideDTMF) == 1);
        m_bDelayToStar = (configParser_GetConfigInt(kszEnableDTMFDelay) == 1);

        TALKUI_INFO("m_bIsPassword(%d)", m_bIsPassword);
        TALKUI_INFO("m_bDelayToStar(%d)", m_bDelayToStar);

        //显示 dtmf 条
        Show();

        //设置焦点
        SetFocus();

        //启动隐藏定时器
        StartAutoHideTimer();

        //调用 显示、隐藏函数
        OnVisibleChange(true);
    }
    else
    {
        //关闭自动变为密码的定时器
        StopSetToSarTimer();

        if (m_bIsPassword)
        {
            //把dtmf内容 全变成密码
            SetDtmfStringPassword();
        }

        //停止自动隐藏定时器
        StopAutoHideTimer();

        //隐藏
        Hide();

        //调用 显示、隐藏函数
        OnVisibleChange(false);
    }
}

//删除所有字符
void CTalkUIDtmf::ClearData()
{
    //清除自动变*的定时器
    StopSetToSarTimer();

    SetDtmfText("");

}

//按键事件
void CTalkUIDtmf::OnKeyPress(int iKey)
{
    //不可见时 不处理
    if (!IsVisible())
    {
        return ;
    }

    yl::string strKey = GetDtmfStringByKeyCode(iKey);

    //获取到的dtmf 不为空
    if (!strKey.empty())
    {
        //需要加密
        if (m_bIsPassword)
        {
            //需要延时输入
            if (m_bDelayToStar)
            {
                //先停止定时器
                StopSetToSarTimer();
                //把之前输入的 都设置成密码
                SetDtmfStringPassword();
                //把字符插入到dtmf条中
                InsertDtmfString(strKey);
                //在启动定时器
                StartSetToStarTimer();

            }
            else        //不需要延时输入 直接输入就行
            {
                //已经赋值成密码字符
                InsertDtmfString(g_strPasswordKey);
            }

        }
        else    //不需要加密直接插入编辑框中
        {
            InsertDtmfString(strKey);
        }

        return ;
    }

    //获取到字符串为空 那么直接隐藏 dtmf 条
    SetDtmfLineVisible(false);

    return ;


}

////鼠标事件
//void CTalkUIDtmf::OnMousePress(int x, int y)
//{
//  //非点击在自身子控件，则隐藏显示
//  SetDtmfLineVisible(false);
//
//}

//编辑框内容变化
void CTalkUIDtmf::slotEditChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                  LPVOID pData)
{
    // 当输入内容改变的时候,重置隐藏计时器

    StartAutoHideTimer();


}

//自动隐藏定时器到达
void CTalkUIDtmf::OnAutoHideTimeOut(xThreadTimer* timer, LPVOID pExtraData)
{
    //不管如何 先关闭定时器
    StopAutoHideTimer();

    //隐藏dtmf 条
    SetDtmfLineVisible(false);
}

//延时加密
void CTalkUIDtmf::OnSetToStar(xThreadTimer* timer, LPVOID pExtraData)
{
    //不管如何 先关闭定时器
    StopSetToSarTimer();

    //如果不是要使用密码 那么直接返回
    if (!m_bIsPassword)
    {
        return;
    }

    //将当前字符全部设置为*
    SetDtmfStringPassword();
}

// 重新启动定时器
void CTalkUIDtmf::StartAutoHideTimer()
{
    //先停止 自动隐藏的定时器
    StopAutoHideTimer();

    //只有dtmf 条 显示的时候 才需要启动定时器
    if (IsVisible())
    {
        m_timerAutoHide.SetTimer(DTMFLINE_AUTO_HIDE_TIME, TIMER_METHOD(this,
                                 talkui::CTalkUIDtmf::OnAutoHideTimeOut));
    }



}

//停止自动隐藏定时器
void CTalkUIDtmf::StopAutoHideTimer()
{
    if (m_timerAutoHide.IsTimerRuning())
    {
        m_timerAutoHide.KillTimer();
    }
}

//启动 密码dtmf定时器
void CTalkUIDtmf::StartSetToStarTimer()
{
    StopSetToSarTimer();

    m_timerDelayToStar.SetTimer(DTMF_DELAY_TIME, TIMER_METHOD(this, talkui::CTalkUIDtmf::OnSetToStar));
}

//关闭 密码dtmf 定时器
void CTalkUIDtmf::StopSetToSarTimer()
{
    if (m_timerDelayToStar.IsTimerRuning())
    {
        m_timerDelayToStar.KillTimer();
    }
}

//初始化控件
void CTalkUIDtmf::InitWidget()
{

}

//设置dtmf 文本
void CTalkUIDtmf::SetDtmfText(const yl::string & strDtmfText)
{
    if (NULL != m_pEditTextView)
    {
        m_pEditTextView->SetInitText(strDtmfText);
    }
}

//插入新的dtmf字符串
void CTalkUIDtmf::InsertDtmfString(const yl::string & strInsertDtmf)
{
    //插入新字符串为空 直接返回
    if (strInsertDtmf.empty())
    {
        return ;
    }

    yl::string strOldDtmf = GetDtmfText();
    yl::string strNewDtmf = strOldDtmf + strInsertDtmf;

    SetDtmfText(strNewDtmf);

}

//将当前字符全部设置为*
void CTalkUIDtmf::SetDtmfStringPassword()
{
    //将当前字符全部设置为*
    yl::string strDtmfText = GetDtmfText();

    int iDtmfTextLen = strDtmfText.length();
    if (iDtmfTextLen > 0)
    {
        yl::string strNewText(iDtmfTextLen, '*');

        //为了提升效率 不相同才要设置
        if (strNewText != strDtmfText)
        {
            SetDtmfText(strNewText);
        }
    }
}

//获取dtmf 文本
yl::string CTalkUIDtmf::GetDtmfText()
{
    if (NULL != m_pEditTextView)
    {
        return m_pEditTextView->GetText();
    }

    return "";
}

//是否有显示
bool CTalkUIDtmf::IsVisible()
{
    if (NULL != m_pEditTextView)
    {
        return m_pEditTextView->isVisible();
    }

    return false;
}

//显示
void CTalkUIDtmf::Show()
{
    if (NULL != m_pEditTextView)
    {
        m_pEditTextView->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

//隐藏
void CTalkUIDtmf::Hide()
{
    if (NULL != m_pEditTextView)
    {
        m_pEditTextView->hide(true);
    }
}

//设置焦点
void CTalkUIDtmf::SetFocus()
{
    if (NULL != m_pEditTextView)
    {
        m_pEditTextView->setFocus();
    }
}

//显示、隐藏 变化
void CTalkUIDtmf::OnVisibleChange(bool bVisible)
{
    //发送 信号
    signalVisiableChange.emitSignal((WPARAM)bVisible, NULL, 0, NULL);
}


}           //namespace talkui

