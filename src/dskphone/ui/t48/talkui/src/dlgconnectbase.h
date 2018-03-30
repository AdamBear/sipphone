#ifndef _DLG_CONNECT_BASE_H_
#define _DLG_CONNECT_BASE_H_

//mod-base
#include "dlgnormaltalkbase.h"


class CDlgConnectBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    CDlgConnectBase();
    virtual ~CDlgConnectBase();

public:

    // 初始化
    void Init();

    // 释放数据，将窗口清空回复到初始化状态
    void Uninit();

    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    bool IsShowTip();

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);

    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    //是否显示加密图标
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    //这边通道键就按照basetalkui默认的处理逻辑进行处理
    //通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

protected:
    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    // 初始化界面数据
    void InitData();

    // 获取配置softkey的文件名和结点名
    void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus);

    //// 调整控件布局
    //virtual void AdjustLayout();

};

#endif // _DLG_CONNECT_BASE_H_
