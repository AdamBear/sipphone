#ifndef _DLG_EDIT_MULTICAST_PAGING_GROUP_H_
#define _DLG_EDIT_MULTICAST_PAGING_GROUP_H_

#include "cdlgbasesubpage.h"
#include "feature/include/featurecommon.h"
class QLabel;
class QLineEdit;

class CDlgMulticastPagingGroupEdit : public CDlgBaseSubPage
{
public:
    CDlgMulticastPagingGroupEdit(QWidget * parent = 0);
    void InitData();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMulticastPagingGroupEdit();
    }

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);
    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

private:
    PagingGroupInfo    *    m_pGroupInfo;
    QLabel   *  m_pLabelOrder;
    QLineEdit * m_pEditGroupAddres;
    QLineEdit * m_pEditLabel;
    QLineEdit * m_pEditChannel;

};

#endif  //_DLG_EDIT_MULTICAST_PAGING_GROUP_H_
