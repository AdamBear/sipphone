#ifndef __MESSAGE_BOX_CACHE_H_
#define __MESSAGE_BOX_CACHE_H_

#include "messageboxbase.h"
#include "messageboxfactory.h"
#include "ylhashmap.h"

typedef YLHashMap<int, CMessageboxFactory *>     MAP_Cache_MsgBox;
typedef MAP_Cache_MsgBox::iterator                                                  iterCacheMap;

class CMessageBoxCache
{
public:
    CMessageBoxCache();
    ~CMessageBoxCache();

public:
    CMessageBoxBase * GetMessageBoxByType(MESSAGEBOX_TYPE eMsgType);
    void PutMessageBox(CMessageBoxBase * pMessageBox);

protected:
    MAP_Cache_MsgBox m_mapCache;

};

//获取全局唯一的弹出框管理对象
#define  _MessageBoxCache GetMessageBoxCacheInstance()
CMessageBoxCache & GetMessageBoxCacheInstance();

#endif //__MESSAGE_BOX_CACHE_H_

