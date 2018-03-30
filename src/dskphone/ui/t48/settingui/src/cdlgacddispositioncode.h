/************************************************************************
 * FileName  : CDlgAcdDisptCode.h (header file)
 * Purpose   :
 * Date      : 2012/12/23 16:10:48
 ************************************************************************/

#ifndef _CDLGACDDISPOSITIONCODE_H_
#define _CDLGACDDISPOSITIONCODE_H_

#include "cdlgbasesubpage.h"
#include <vector>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

class CDlgACDDispositionCode : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgACDDispositionCode();
    }

public:
    CDlgACDDispositionCode(QWidget * parent = NULL);

    virtual ~CDlgACDDispositionCode();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual bool SaveCompleted();       // 保存成功后的其他操作
    virtual void SetData(void * pData  /*= NULL*/);
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    void SetCallID(int iCallID);

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

private:
    void PushbackItem(const yl::string & strCode, const yl::string & strName);

private:
    QLineEdit * m_pCodeEdit;
    std::vector<yl::string> m_codeVec;
    int m_nCallId;
};

#endif
