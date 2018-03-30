#ifndef _DLG_CONNECT_UI_H_
#define _DLG_CONNECT_UI_H_

//mod-base
#include "dlgconnectbase.h"


//前向声明
class qMultiPicAnimation;

class CDlgConnectUI: public CDlgConnectBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgConnectUI();
    }

public:
    CDlgConnectUI();
    virtual ~CDlgConnectUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    //设置区域
    virtual void AdjustLayout();

protected:
    //构造函数调用初始化数据
    void InitData();

private:
    qMultiPicAnimation * m_pPicAnimation;
};

#endif // _DLG_CONNECT_UI_H_
