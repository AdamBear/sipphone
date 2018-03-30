
#ifndef __FRAME_LIST_DELEGATE_BASE_H__
#define __FRAME_LIST_DELEGATE_BASE_H__

#include "uikernel/iframelistdelegate.h"

class CFrameList;
class IFrameListDelegate;

class CFrameListDelegateBase
{
protected:
    CFrameListDelegateBase(void);
    virtual ~CFrameListDelegateBase(void);

public:
    // 设置代理的列表框
    void SetFrameList(CFrameList * pFrameList);

    // 获取代理
    CFrameList * GetFrameList() const;

    // 设置起始数据序号
    void SetPageDataIndex(int nDataIndex);

    // 设置默认焦点
    void SetDefaultFocus(bool bDefaultFocus);

    // 设置焦点序号
    void SetFocusIndex(int nIndex);

    void ResetIndex();
    void UpdateIndex();

    int GetPageDataIndex() const;

    int GetFocusIndex() const;

    // 刷新列表框
    virtual void RefreshFrameList();

protected:
    // 是否绑定列表框
    bool IsAttach(CFrameListBase * pFrameList);
    // 绑定列表框
    void Attach(CFrameListBase * pFrameList);
    // 取消绑定列表框
    void Detach(CFrameListBase * pFrameList);

    // 获取真正代理指针
    virtual IFrameListDelegatePtr GetDelegate() const           = 0;

public:
    // 释放
    bool Release();

protected:
    // 列表框实例指针
    CFrameList * m_pFrameList;
    // 列表框起始数据序号
    int m_nPageDataIndex;
    // 是否有焦点
    bool m_bItemFocus;
    // 是否默认焦点
    bool m_bDefaultFocus;
    // 焦点序号
    int m_nFocusIndex;
};

#endif
