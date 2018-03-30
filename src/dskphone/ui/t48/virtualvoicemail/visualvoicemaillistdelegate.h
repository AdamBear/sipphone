#ifndef _VISUAL_VOICEMAIL_LLIST_DELEGATE_H
#define _VISUAL_VOICEMAIL_LLIST_DELEGATE_H

#include "settingui/src/subpagelistdelegate.h"

// 文本信息链表.
typedef QVector<QLabel *> VecpLabel;

class CVisualVoiceMailListDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    CVisualVoiceMailListDelegate();

#ifndef _T48
    CVisualVoiceMailListDelegate(CFrameList * pList);
#endif

    ~CVisualVoiceMailListDelegate();

    void SetItemPat(QWidget * pPat)
    {
        m_pItemPat = pPat;
    }
    void HideItem();

    void PreCreateItem();

    virtual void BindFramelist(CFrameList * pList);
    virtual void UnBindFramelist();

    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    virtual int GetMaxPagePageItemCount();

public:
    // 加载录音列表
    bool LoadRecordList(int nDataIndex, int nDataCount);

private:
    CFrameList   *  m_pVisualVoiceMailFramelist;
    QWidget      *      m_pItemPat;
    int                 m_nTotalItemCount;

    VecpLabel           m_vecpLabel;
};

#endif //_VISUAL_VOICEMAIL_LLIST_DELEGATE_H
