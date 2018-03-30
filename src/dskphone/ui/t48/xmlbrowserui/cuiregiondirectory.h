#ifndef REGION_DIRECTORY_H
#define REGION_DIRECTORY_H

#include "cuiregiontextmenu.h"

#include "baseui/iscrollnotify.h"

class CPageCtrl;

//class CUIRegionDirectory
class CUIRegionDirectory : public CUIRegionBase, public IScrollNotify
{
    Q_OBJECT

public:
    CUIRegionDirectory(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionDirectory();

    //必须实现的函数
    void Update(void * pData);

    bool eventFilter(QObject * pObj, QEvent * pEvent);

    int GetCurrentItem();

    // 通知向前滚动
    virtual void OnNotifyScrollPrevious();
    // 通知向后滚动
    virtual void OnNotifyScrollNext();

#if IF_FEATURE_XMLDIRECTORY
    void Search();
    // 处理UI的Action事件
    virtual bool OnXMLUIAction(XB_SOFTKEY_TYPE eType);
#endif

protected slots:
    void OnPageChanged(int nCurrentPage, int nSumPage);

    //某个Item被点击/按下Ok
    void OnClickItem(int nIndex);

    void OnCurrentItemChange(int iIndex);

#if 1//IF_FEATURE_XMLDIRECTORY
    void OnKeyWordChanged(const QString & strText);

    void OnTimerOut();
#endif

private:
    // 获取页面的默认softkey数据
    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

private:
    CTextMenuItemIS  *  m_pLabTitle;

    CFrmTextMenu * m_pFrmDirectory;
    CPageCtrl * m_pPageCtrl;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    bool              m_bDefSoftKey; //是否是默认softkey
    VecSoftkeyParam m_vecGlobalSoftkey;
#if IF_FEATURE_XMLDIRECTORY
    VecSoftkeyParam   m_vecSearchSoftkey;
#endif
#endif

#if IF_FEATURE_XMLDIRECTORY
    QLineEdit    *    m_pInput;      // input控件
    QTimer            m_SearchTimer;
    int               m_iSearchDelay;
#endif
};

#endif //REGION_DIRECTORY_H

