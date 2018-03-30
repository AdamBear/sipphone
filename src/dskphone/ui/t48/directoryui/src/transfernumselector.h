#ifndef __TRANSFER_NUM_SELECTOR_H__
#define __TRANSFER_NUM_SELECTOR_H__

#include <ylstring.h>
#include "baseui/framelistdelegate.h"
#include "directoryui/include/directorycommon.h"
#include "messagebox/imsgboxeventfilter.h"

enum TRAN_NUM_TYPE
{
    TRAN_NUM_DIR = 0,
    TRAN_NUM_OTHER = 1,
};

class CTransferNumSelector : public CFrameListDelegate, public IMsgBoxEventFilter
{
public:
    CTransferNumSelector();
    virtual ~CTransferNumSelector(void);

    void Init(TRAN_NUM_TYPE eTranNumType);
    bool GetNumber(yl::string & strNumber) const;

    const yl::string & GetName() const;
    void SetName(const yl::string & strName);

    const yl::string & GetNumber() const;
    void SetNumber(const yl::string & strNumber);

    // IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

    // IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    // IMsgBoxEventFilter
public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress, bool & bFilter);

private:
    int m_nSelIndex;
    yl::string m_strName;

    // 历史记录，联想号码
    yl::string m_strNumber;
    TRAN_NUM_TYPE m_eTranNumType;
};


#endif