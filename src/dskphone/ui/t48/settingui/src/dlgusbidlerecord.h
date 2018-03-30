#ifndef DLGUSBIDLERECORD_H
#define DLGUSBIDLERECORD_H

#include "settingui/src/cdlgbasesubpage.h"
#include "uicommon/uikernel/qtimepiece.h"
#include <QDialog>

class CDlgUSBIdleRecord : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgUSBIdleRecord(QWidget * parent = 0);
    ~CDlgUSBIdleRecord();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgUSBIdleRecord();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //界面初始化
    void InitPageData();
    //获取Softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);
    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);
    // 回到主界面
    virtual bool PorcessHomeBtnClick();
    //Action操作
    virtual bool OnAction(const QString & strAction);
    //开始录音
    void StartUSBRecord();
    //暂停录音
    void PauseUSBRecord();
    //停止录音
    void StopUSBRecord();

private:
    //录音时间提示控件
    QTimePiece      m_RecordTime;
    QLabel          m_RecordIcon;
};

#endif
