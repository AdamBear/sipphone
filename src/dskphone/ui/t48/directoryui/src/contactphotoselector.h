
#ifndef __CONTACT_PHOTO_SELECTOR_H__
#define __CONTACT_PHOTO_SELECTOR_H__

#include <ylstring.h>
#include <ylvector.h>
#include "baseui/framelistdelegate.h"

class CContactPhotoListItem;
class CContactPhotoSelector : public CFrameListDelegate
{
public:
    CContactPhotoSelector(void);
    virtual ~CContactPhotoSelector(void);

public:
    void Init(const yl::string & strPhoto);
    int GetRowCount() const;
    yl::string GetPhoto();

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

protected:
    void OnChooseImage(CFrameList * pFrameList, CContactPhotoListItem * pItem, int nIndex);
    void OnSelectImage(CFrameList * pFrameList, CContactPhotoListItem * pItem, int nIndex);
    void OnUnSelectImage(CFrameList * pFrameList, CContactPhotoListItem * pItem);

protected:
    typedef YLVector<yl::string> VEC_PHOTO_STRING;

    VEC_PHOTO_STRING m_vecPhotoName;
    VEC_PHOTO_STRING m_vecPhotoPath;
    int m_nSelIndex;
    // 头像绝对路径
    yl::string m_strCurPhoto;
};

#endif
