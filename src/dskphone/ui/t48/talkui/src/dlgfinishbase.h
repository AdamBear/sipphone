#ifndef _DLG_FINISH_BASE_H_
#define _DLG_FINISH_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"


//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"
#include "commonwidget.h"

class CDlgFinishedBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    //构造函数
    CDlgFinishedBase();

    //析构函数
    virtual ~CDlgFinishedBase();

public:
    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 初始化数据
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;

    //是否显示加密图标,rectEncrypt hold the location of encrypt label
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);

    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    // 是否有显示提示信息
    virtual bool IsShowTip();

    // 事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    //数字键处理
    virtual bool OnDigitkeyPress(int iKey);

protected:
    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    // 初始化页面的数据
    void InitData();

    //// 更新自动重拨提示语
    //void UpdateAutoRedialNote();

    // 获取配置softkey的文件名和结点名
    void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus);

    void GetBluetoothSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

protected:
    //Finish界面使用
    // 更新自动重拨提示语
    virtual void UpdateAutoRedialNote() = 0;

private Q_SLOTS:
    void OnAutoRedial();

protected:
    // 退出计时器
    QTimer   *  m_pAutoRedialTimer;

    // 保存自动重拨倒计时超时秒数
    int         m_iRedialInterval;

    //剩余的重拨次数
    int         m_iRemainRedialTimes;
};

#endif // _DLG_FINISHUI_H_
