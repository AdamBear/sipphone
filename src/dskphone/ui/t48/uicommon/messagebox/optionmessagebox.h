#ifndef OPTIONMESSAGEBOX_H
#define OPTIONMESSAGEBOX_H

#include "messageboxbase.h"
#include "baseui/framelist.h"
#include "baseui/framelistdelegate.h"

struct OptionMsgBoxItem
{
    QString     m_strTitle;
    QString     m_strAction;
};
typedef QVector<OptionMsgBoxItem>       vecOptionMsgBoxItem;

class COptionMessageBox : public CMessageBoxBase, public CFrameListDelegate
{
    Q_OBJECT

public:
    COptionMessageBox(CMaskFrame * pMaskFrame = 0);
    virtual ~COptionMessageBox();

    void Init(const vecOptionMsgBoxItem & vecOptionItem);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool event(QEvent * pEvent);

    virtual bool IsShowSoftkey()
    {
        return true;
    }

    virtual void AdjustLayout();

    virtual void SetDefaultFocus(bool bFocus, int nIndex);

    void SetTimeToIdle(int iTimeToExit);

    virtual void PrintMsgBoxInfo(bool bAdd);

protected:
    void paintEvent(QPaintEvent * pPaintEvent);

protected:
    void GetSelectInfoToExtraData();

    /////////////////framelist delegate//////////////////////////
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    ////////////////////////////////////////////////////////////////

private:
    CFrameList           *          m_pOptionFrameList;
    vecOptionMsgBoxItem     m_vecOptionItem;

    bool                                    m_bFocus;
    int                                 m_nIndex;

    yl::string                          m_strBackGround;
};

#endif // SELECTMESSAGEBOX_H
