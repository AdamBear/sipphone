#ifndef DLGUSBBROWSEPHOTO_H
#define DLGUSBBROWSEPHOTO_H

#include "settingui/src/cdlgbasesubpage.h"
#include "usbphotolistdelegate.h"
#include <QDialog>

class CDlgUSBBrowsePhoto : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgUSBBrowsePhoto(QWidget * parent = 0);
    ~CDlgUSBBrowsePhoto();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgUSBBrowsePhoto();
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
    //界面刷新
    virtual void UpdateWnd();
    //界面结束处理
    virtual void Uninit();
    // 状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);
    // 加载图片列表
    bool LoadPhotoList(int nDataIndex, int nDataCount);
    // 事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    //执行操作
    virtual bool OnAction(const QString & strAction);
    // 接受到逻辑层发送消息
    void OnReceiveMsg(msgObject & objMessage);
    // 重新加载录音列表
    void ReloadPhotoList();

public Q_SLOTS:
    void OnShow();
    void OnLoadPhotoList();
    void OnItemDeleteAction(CListItem * pItem);
    void OnLoadTimeOut();

protected:
    virtual void showEvent(QShowEvent * e);

public:
    USBPhotoListDelegate   *    m_pUSBPhotoListDelegate;
private:

    bool                        m_bReloadPhotoList;
    QTimer                      m_timerLoad;
    bool                        m_bLoadMinTimeOut;
};

#endif //DLGUSBBROWSEPHOTO_H
