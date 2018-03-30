/************************************************************************
 * FileName  : CDlgPickupMenu.h (header file)
 * Purpose   :
 * Date      : 2013/01/15 20:41:19
 ************************************************************************/

#ifndef _CDLGPICKUPMENU_H_
#define _CDLGPICKUPMENU_H_

#include "settingui/src/cdlgbasesubpage.h"

class CDlgPickupMenu : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgPickupMenu();
    }

public:
    CDlgPickupMenu(QWidget * parent = 0);

    virtual ~CDlgPickupMenu();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void SetData(void * pData = NULL);

    // 读取页面数据
    virtual bool LoadPageData();

    //http://10.3.5.199/Bug.php?BugID=65420
    //实际上  数据是不可能为空的
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    int GetCurrentIndex();

    // 子界面响应Back按钮的操作
    virtual void OnBackButtonClick();

    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

public Q_SLOTS:
    void OnProcessPageAction(const QString & strAction);

private:
    virtual bool OnAction(const QString & strAction);
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据

private:
    //处理pickup事件
    void OnPickUpLine();
    //处理dial事件
    void OnDial();
    //处理new call事件
    void OnNewCall();
    //处理cancle事件
    void OnCancel();
    void InitData();
};

#endif
