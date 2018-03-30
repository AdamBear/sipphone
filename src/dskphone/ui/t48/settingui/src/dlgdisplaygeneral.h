#ifndef DLGDISPLAYGENERAL_H
#define DLGDISPLAYGENERAL_H

#include <QDialog>
#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "setting/src/displaycontroller.h"

struct DisplayGeneralData;
///////////////////////////////////////////////////////////////////////////////
class CDlgDisplayGeneral : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgDisplayGeneral(QWidget * parent = 0);
    ~CDlgDisplayGeneral();

private:


public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //页面向其数据Mode取数据再次刷新到界面上
    virtual void Refresh();

    // 初始化数据
    void Init();

    // 释放数据
    void Uninit();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDisplayGeneral();
    }

protected:
    void showEvent(QShowEvent * pEvent);
    void hideEvent(QHideEvent * pEvent);

private:
    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();
    //用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
    virtual void ReleaseData();
    //于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
    virtual void OnViewShow();
    //比较页面前后数据
    bool ComparePageData(const DisplayGeneralData & beforePageData,
                         const DisplayGeneralData & afterPageData) const;
    //取得当前页面数据
    DisplayGeneralData & GetCurrentPageData(DisplayGeneralData & currentPageData) const;

    //设置当前页面数据
    void SetCurrentPageData(DisplayGeneralData & currentPageData);

    // 事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 重设背景的亮度
    void ResetBacklight(int iLightLevel = -1);

private slots:
    void OnActiveLevelChange(int iCmbIndex);

private:
    bool m_bLoadData;// 判断是否加载完数据的标志

    QComboBox * m_pCmbBoxBacklightTime;
    QComboBox * m_pCmbBoxActiveLevel;
    QComboBox * m_pCmbBoxInactiveLevel;
};

#endif // DLGDISPLAYGENERAL_H
