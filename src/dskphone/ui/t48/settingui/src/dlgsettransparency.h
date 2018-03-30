#ifndef __DLG_SET_TRANSPARENCY_H__
#define __DLG_SET_TRANSPARENCY_H__

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"


class CDlgSetTransparency : public CDlgBaseSubPage
{
public:
    CDlgSetTransparency(QWidget * pParent = NULL);
    virtual ~CDlgSetTransparency();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetTransparency();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

private:
    void InitData();
    void InitListItemData();

    void FillTransSelectType();

private:
    QComboBox * m_pTransparencyType;

    int m_nTransType;
};

#endif // #ifndef __DLG_SET_TRANSPARENCY_H__
