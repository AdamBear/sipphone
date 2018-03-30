#include <QtGui/QLabel>
#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "uimanager/uimanager_inc.h"

#define IF_TESTMODE_DISPLAY_FIRM      0      //是否显示固定版本

class CDlgTestModeStatus : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgTestModeStatus(QWidget * parent = 0);
    ~CDlgTestModeStatus();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgTestModeStatus();
    }

public:
    //加载数据
    virtual bool LoadPageData();

    //获取数据状态是否改变
    virtual bool IsStatusChanged();

    //保存数据
    virtual bool SavePageData();

    //界面初始化
    void InitPageData();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

private:
    virtual bool OnAction(const QString & strAction);

private:
    QLabel * m_pLabRPSStatus;           //RPS 状态
    QLabel * m_pLabResetStatus;         //是否恢复出厂设置
#if IF_TESTMODE_DISPLAY_FIRM
    QLabel * m_pLabFirmware;            //固定版本号
#endif
};
