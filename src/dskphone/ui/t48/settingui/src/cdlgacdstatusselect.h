/************************************************************************
 * FileName  : CDlgACDStatusSelect.h (header file)
 * Purpose   :
 * Date      : 2012/12/22 11:44:31
 ************************************************************************/

#ifndef _CDLGACDSTATUSSELECT_H_
#define _CDLGACDSTATUSSELECT_H_

#include "cdlgbasesubpage.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"

class CDlgACDStatusSelect : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgACDStatusSelect();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    CDlgACDStatusSelect(QWidget * parent = 0);

    virtual ~CDlgACDStatusSelect();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual void SetData(void * pData /* = NULL */);

    //收到logic层返回的消息
    void OnReceiveMsg(const msgObject & objMessage);

    void SelectIndexByAction(QString strAction);

private:
    void InitData();
private:
    ACD_STATUS m_eACDStatus;
};

#endif
