#ifndef __ELEMENT_MANAGER_H__
#define __ELEMENT_MANAGER_H__

#ifdef IF_FEATURE_XMLBROWSER

#include "ETLLib.hpp"
#include "logicanduicommon.h"
#include <singletonclassdefine.h>
#include <yllist.h>

class CElementBase;
class CElementHaveUIBase;
class CElementStatus;

typedef YLList<CElementHaveUIBase *> typeListUIElement;

// Element逻辑管理
class CElementManager
{
    DECLARE_SINGLETON_CLASS(CElementManager)
public:
    // 加载对象
    // | lpszPath | [in] 文件路径
    // | nLoadOpts | [in] 操作类型
    bool LoadElement(const char * lpszPath, int nLoadOpts, bool bExit);
    // 对象加载图片
    bool LoadImage(int nElementID, const char * lpszPath, int nLoadOpts, void * pExtData = NULL);

    // 加载的回调
    // | nElementID   | [in] 指定对象ID，如果为-1，则表示非元素对象发起的加载任务
    // | nLoadOpts    | [in] 指定加载的类型，如包含XML_PARSE，则说明pData为XMLParseData，进行对象的创建动作。
    // | pTakeoutData | [in] !! 从Action中取回的数据，如果不为空，会负责释放; 原因：避免大数据的拷贝
    bool OnLoadFile(int nLoadOpts, int nElementID, void * pTakeoutData);

    // 获取对象数据
    // 如果objToUIData中未指定ID，则返回当前活动的对象数据
    bool GetToUIData(XMLLogicToUIData & objToUIData);

    // 事件响应
    bool OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData);

    // 从链表中删除Element
    bool DeleteElement(int nElementID);
    // 从链表中删除所有Element
    void DeleteAllElement();

    // 下载声音
    bool LoadWav(const char * lpszPath, int nLoadOpts);

    // 判断当前是否有XB界面的Element
    BOOL IsHaveXBElement();

#if IF_FEATURE_XMLB_SCREENSAVER
    //获取屏保缓存数据
    bool GetSSData(XMLLogicToUIData & objToUIData);
    //清除Screensaver缓存数据
    void DeleteSSElement();
#endif

private:
    // 创建新的Element并加入链表，如果出错则返回错误提示
    CElementBase * CreateElement(XMLParseData * pData);

    /*
     * MAP的基本操作
     */
    // 将Element加入链表
    void AddElement(CElementHaveUIBase * pElement);
    // 从链表获取
    int GetTopElementID();
    // 从链表获取
    CElementHaveUIBase * GetElement(int nElementID);
    // 删除对象
    bool RemoveElement(int nElementID);
#if IF_FEATURE_XMLB_SCREENSAVER
    //获取非screensaver类型的elementid
    int GetElementIDNotSS();
    // 从链表获取screensaver element
    CElementHaveUIBase * GetScreenSaverElement();
    //将Screensaver从m_listUIElement中去除
    bool DeleteSSElementInList();

#endif

#if IF_BUG_32431
    // 是否需要重新创建新元素
    bool IsNeedCreateElement(void * pTakeoutData);
#endif

private:
    // 创建新的Element
    CElementBase * CreateElementIns(XML_ELEMENT_TYPE eType);

private:
    typeListUIElement   m_listUIElement;   // 存储Element的链表(XB界面的Element)
    int                 m_nActiveElemID;    // 当前元素ID
    chCriticalSection   m_cs;           // 链表操作加锁，因为存在多线程操作
    bool m_bisExitUI;                   // 是否退出界面
#if IF_FEATURE_XMLB_SCREENSAVER
    CElementHaveUIBase  * m_pSSElement;     // 屏保element指针
#endif

};

// 获取全局唯一的Element管理对象
#define _ElementManager GET_SINGLETON_INSTANCE(CElementManager)

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __ELEMENT_MANAGER_H__
