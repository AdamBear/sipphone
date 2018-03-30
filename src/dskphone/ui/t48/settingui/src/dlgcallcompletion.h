#ifndef DLGCALLCOMPLETION_H
#define DLGCALLCOMPLETION_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include "setting/src/advancedcallfeaturecontroller.h"

class qCheckBox;
struct AdvancedCallFeatureCallCompletionData;
///////////////////////////////////////////////////////////////////////////////
class CDlgCallCompletion : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgCallCompletion(QWidget * parent = 0);
    ~CDlgCallCompletion();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCallCompletion();
    }


public:
    //读取页面显示值
    bool GetPageData(AdvancedCallFeatureCallCompletionData * pData);
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //页面向其数据Mode取数据再次刷新到界面上
    virtual void Refresh();

private:
    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();
    //用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
    virtual void ReleaseData();
    //于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
    virtual void OnViewShow();

    /*private:*/
public:
    //记录加载数据时，设置combobox的索引值
    int m_iIndex;
    AdvancedCallFeatureCallCompletionData m_callCompletionData;

    qCheckBox   *   m_qCheckBoxCallCompletion;
};

#endif // DLGCALLCOMPLETION_H
