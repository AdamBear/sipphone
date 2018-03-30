#ifndef CUIREGIONTEXTSTATUS_H
#define CUIREGIONTEXTSTATUS_H

#include "cuiregionbase.h"
#include <list>
using namespace std;


// 前向声明
class AutoResizeListView;

class CUIRegionTextStatus : public CUIRegionBase
{
    Q_OBJECT

public:
    CUIRegionTextStatus(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CUIRegionTextStatus();

    // 根据数据更新界面
    virtual void Update(void * pData);

    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 设置控件的位置
    void SetStatusFrameGeometry(QRect objRect);

    // 是否换行
    // bWrap = true 为换行
    // bWrap = false 为不换行
    void SetWrapText(bool bWrap);

private:
    AutoResizeListView * m_pAutoResizeView;
};

#endif // CUIREGIONTEXTSTATUS_H
