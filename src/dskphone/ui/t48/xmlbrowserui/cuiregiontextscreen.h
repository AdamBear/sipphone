#ifndef CUI_REGION_TEXT_SCREEN
#define CUI_REGION_TEXT_SCREEN

#include "cuiregionbase.h"

// 前向声明
class AutoResizeLab;
class CScrollPage;
class CBaseLabel;
class QScrollBar;

class CUIRegionTextScreen : public CUIRegionBase
{
    Q_OBJECT
public:
    CUIRegionTextScreen(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionTextScreen();

    // 根据数据更新界面
    virtual void Update(void * pData);

    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

protected:
    AutoResizeLab    *   m_pLabTitle;   // 显示Title的label
    CScrollPage   *   m_pScrollPage; // 显示文本的Table
    AutoResizeLab    *   m_pLabText;    // 显示文本的label
};

#endif // CUI_REGION_TEXT_SCREEN
