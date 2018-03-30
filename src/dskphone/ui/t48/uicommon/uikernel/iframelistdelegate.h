#ifndef __I_FRAME_LIST_DELEGATE_H__
#define __I_FRAME_LIST_DELEGATE_H__

class CFrameListBase;
class CListItem;

class IFrameListDelegate
{
public:
    IFrameListDelegate() {};
    virtual ~IFrameListDelegate() {};

public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext()                               = 0;
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious()                          = 0;
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList)     = 0;
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)     = 0;
    // 是否已经绑定特定的列表框
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList)  = 0;
    // 数据代理解除绑定
    virtual void OnFrameListDetach(CFrameListBase * pFrameList)  = 0;
    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList)  = 0;
};
typedef IFrameListDelegate * IFrameListDelegatePtr;

#endif
