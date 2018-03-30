/************************************************************************
 * FileName  : CDlgACDInitialState.h (header file)
 * Purpose   :
 * Date      : 2012/12/22 11:44:31
 ************************************************************************/

#ifndef _CDLGACDINITIALSTATE_H_
#define _CDLGACDINITIALSTATE_H_

#include "cdlgbasesubpage.h"

class CDlgACDInitialState : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgACDInitialState();
    }
public:
    CDlgACDInitialState(QWidget * parent = 0);

    virtual ~CDlgACDInitialState();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual void SetData(void * pData /* = NULL */);
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

private:
    void InitData();

private:
    QComboBox * m_pInitialCmb;
};

#endif
