#ifndef _TALKUI_DTMF_H
#define _TALKUI_DTMF_H

#include "imeedit/ximeedit.h"

namespace talkui
{


class CTalkUIDtmf : public xSlotHandler, public xTimerHandler
{
public:
    CTalkUIDtmf();
    virtual ~CTalkUIDtmf();

public:
    //加载控件
    void SetDtmfWidget(xEditTextView * pEditTextView);

    //释放控件
    void FreeWidget();

public:
    bool GetDtmfVisible();
    //设置DTMF栏可见性
    void SetDtmfLineVisible(bool bVisible);

    //删除所有字符
    void ClearData();

public:
    //按键事件
    void OnKeyPress(int iKey);

    //触摸屏 需要判断 鼠标事件  判断鼠标点击的位置与dtmf条位置是否相同
    ////鼠标事件
    //void OnMousePress(int x, int y);

public:
    //编辑框内容变化
    void slotEditChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    //自动隐藏定时器到达
    void OnAutoHideTimeOut(xThreadTimer* timer, LPVOID pExtraData);

    //延时加密
    void OnSetToStar(xThreadTimer* timer, LPVOID pExtraData);

protected:
    // 重新启动定时器
    void StartAutoHideTimer();

    //停止自动隐藏定时器
    void StopAutoHideTimer();

    //启动 密码dtmf定时器
    void StartSetToStarTimer();

    //关闭 密码dtmf 定时器
    void StopSetToSarTimer();

private:
    //初始化控件
    void InitWidget();

protected:
    //设置dtmf 文本
    void SetDtmfText(const yl::string & strDtmfText);
    //插入新的dtmf字符串
    void InsertDtmfString(const yl::string & strInsertDtmf);
    //将当前字符全部设置为*
    void SetDtmfStringPassword();
    //获取dtmf 文本
    yl::string GetDtmfText();

    //是否有显示
    bool IsVisible();

    //显示
    void Show();
    //隐藏
    void Hide();

    //设置焦点
    void SetFocus();

    //显示、隐藏 变化
    void OnVisibleChange(bool bVisible);

public:
    //显示/隐藏 的信号
    xSignal signalVisiableChange;




private:
    //自动隐藏定时器
    xThreadTimer m_timerAutoHide;       //定时器

    //是否以密码模式显示
    bool        m_bIsPassword;

    //如果以密码模式显示，是否延时加密
    bool        m_bDelayToStar;


    //自动加密的定时器
    xThreadTimer m_timerDelayToStar;        //定时器


protected:
    xEditTextView * m_pEditTextView;    //输入框


};


}       //namespace talkui


#endif
