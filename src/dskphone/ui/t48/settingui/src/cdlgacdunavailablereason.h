/************************************************************************
 * FileName  : CDlgACDUnavailableReason.h (header file)
 * Purpose   :
 * Date      : 2012/12/22 14:42:38
 ************************************************************************/

#ifndef _CDLGUNAVAILABLEREASON_H_
#define _CDLGUNAVAILABLEREASON_H_

#include "cdlgbasesubpage.h"
#include <vector>

class CDlgACDUnavailableReason : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgACDUnavailableReason();
    }

private:
    typedef std::vector<QLabel *>    CodeLabelVector;

public:
    CDlgACDUnavailableReason(QWidget * parent = NULL);

    virtual ~CDlgACDUnavailableReason();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual bool SaveCompleted();       // 保存成功后的其他操作
    virtual void SetData(void * pData  = NULL);
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

private:
    void PushbackResonItem(const yl::string & strResnCode, const yl::string & strResnName);

private:
    QLineEdit * m_pCodeEdit;
    std::vector<yl::string> m_codeVec;
};

#endif
