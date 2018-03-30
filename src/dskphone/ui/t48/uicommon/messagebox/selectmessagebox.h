#ifndef SELECTMESSAGEBOX_H
#define SELECTMESSAGEBOX_H


#include <QDialog>
#include <QLabel>
#include "baseui/framelist.h"
#include "uikernel/quipushbutton.h"
#include "messageboxbase.h"

#define SELECT_POP_BOX_POS_ITEM_HEIGHT 47

class IFrameListDelegate;

enum
{
    SELECT_POP_BOX_TYPE_NORMAL  = 0,            // 普通选择框(6行)
    SELECT_POP_BOX_TYPE_TINY    = 1,            // Tiny选择框(3行)
};

class CSelectMessageBox : public CMessageBoxBase
{
    Q_OBJECT
public:
    CSelectMessageBox(int nSelectBoxType, IFrameListDelegate * pDelegate = NULL,
                      CMaskFrame * pMaskFrame = 0);
    ~CSelectMessageBox();

    void Init(const QString & strTitle);

    void SetMessageBoxSpecialInfo(VecMsgBoxSoftkey & vecSoftkey);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool event(QEvent * pEvent);

    virtual void PrintMsgBoxInfo(bool bAdd);

private:
    int m_nSelectBoxType;
    bool m_bRefresh;
    CFrameList * m_pFrameList;

    QLabel m_labTitle;

    //选择框内容部分的高度（有两种情况）
    int m_iHight;

};

#endif // SELECTMESSAGEBOX_H
