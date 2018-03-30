#ifndef DLGUSBBROWSERECORD_H
#define DLGUSBBROWSERECORD_H

#include "settingui/src/cdlgbasesubpage.h"
#include "usbrecordlistdelegate.h"
#include <QDialog>

class CDlgUSBBrowseRecord : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgUSBBrowseRecord(QWidget * parent = 0);
    ~CDlgUSBBrowseRecord();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgUSBBrowseRecord();
    }

public:
    // 加载数据
    virtual bool LoadPageData();
    // 获取数据状态是否改变
    virtual bool IsStatusChanged();
    // 保存数据
    virtual bool SavePageData();
    // 界面数据初始化
    void InitPageData();
    //界面初始化
    virtual void Init();
    //界面结束处理
    virtual void Uninit();
    // 状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);
    // 回到主界面
    virtual bool PorcessHomeBtnClick();
    // 加载录音列表
    bool LoadRecordList(int nDataIndex, int nDataCount);
    // 播放状态改变
    void PlayStatusChange(bool bPlay);
    // 播放失败
    void PlayFail();
    // 事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    // 页面改变
    void PageChange(bool bHide);
    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;
    //执行操作
    virtual bool OnAction(const QString & strAction);
    // 接受到逻辑层发送消息
    void OnReceiveMsg(msgObject & objMessage);

private:
    // 弹框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // 重新加载录音列表
    void ReloadRecordList();

public Q_SLOTS:
    void OnShow();
    void OnLoadRecordList();
    void OnItemDeleteAction(CListItem * pItem);
    void OnLoadTimeOut();

protected:
    virtual void showEvent(QShowEvent * e);

private:
    USBRecordListDelegate   *   m_pUSBRecordListDelegate;
    YLVector<RecordFileInfo>    m_vecRecordList;
    bool                        m_bReloadRecordList;
    QTimer                      m_timerLoad;
    bool                        m_bLoadMinTimeOut;
};

#endif
