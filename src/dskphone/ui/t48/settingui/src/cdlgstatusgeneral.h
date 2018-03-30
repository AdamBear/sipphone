#ifndef CDLGSTATUSGENERAL_H
#define CDLGSTATUSGENERAL_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "uimanager/uimanager_inc.h"
#include "keymap.h"
#include "globalinputhook/inputhook_include.h"
#include "uikernel/qscrolltextlabel.h"

//!!!!!换为BaseDialog
class CDlgStatusGeneral : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgStatusGeneral(QWidget * parent = 0);
    ~CDlgStatusGeneral();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgStatusGeneral();
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
    QLabel * m_pLabIPV4Value;
    qScrollTextLabel * m_pLabIPV6Value;
    QLabel * m_pLabMACValue;
    QLabel * m_pLabWifiMAC;
    QLabel * m_pLabFirmwareValue;
#if IF_BUG_34374
    qScrollTextLabel * m_pLabCfgVerValue;
#endif
    //!!!!! 需要从逻辑读取数据
    int         m_nMode;
};

#endif // CDLGSTATUSGENERAL_H
