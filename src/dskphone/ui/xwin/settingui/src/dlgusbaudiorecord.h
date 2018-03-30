#ifndef __DLG_USB_AUDIO_RECORD_H__
#define __DLG_USB_AUDIO_RECORD_H__

#include "basesettingdialog.h"
#include "baseui/include/titlebase.h"


enum
{
    SETTTING_DIALOG_TYPE_AUDIO_RECORD       = 0x02000000,
};

class xImageView;
class xTimerPieceView;

#if (defined IF_USB_SUPPORT) && (defined IF_USB_RECORD)

class CDlgUSBAudioRecord : public xLinearLayout, public CSettingDialogBase
{
public:
    CDlgUSBAudioRecord();
    virtual ~CDlgUSBAudioRecord();

public:
    static CBaseDialog * CreateInstance();
    static CDlgUSBAudioRecord * GetDlgUSBAudioRecord(CSettingDialogBase * pDlg);

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual void OnFocusChanged(bool bFocused);

    virtual bool IsRejectReturnToIdle();

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

public:
    virtual void RefreshUI(bool bReLoadData = false);

    void RefreshRecordStatus(int nStatus);

protected:
    void RefreshRecordInfo();

protected:
    xImageView * m_pIconStatus;
    xTimerPieceView * m_pTimerView;
    bool m_bSelfExit;
    bool m_bReShow;
};
#endif

#endif
