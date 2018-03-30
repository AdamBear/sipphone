#ifndef __TALK_UI_FACTORY_H__
#define __TALK_UI_FACTORY_H__


#include <yllist.h>
#include <ylstring.h>
using namespace std;


class CDlgBaseTalkUI;

class CTalkUIFactory
{
public:
    static CTalkUIFactory & GetInstance();

protected:
    CTalkUIFactory(void);

public:
    virtual ~CTalkUIFactory(void);

public:
    //初始化
    bool Init();
    //反初始化
    void UnInit();

public:
    //该函数调用 GetTalkUIByData 或 GetTalkUIByDataNoDelete 获取通话子界面
    //在该函数内部判断是要调用哪个函数来实现
    CDlgBaseTalkUI * GetTalkUIBySessionIDAndUIName(int iSessionID, yl::string strTalkUIName);

protected:
    //根据SessionID和TalkUIName 获取TalkUI实例 这边要删除与SessionID相关的其他TalkUI实例
    CDlgBaseTalkUI * GetTalkUIByData(int iSessionID, yl::string strTalkUIName);

    //该函数暂时没有使用
    //根据SessionID和TalkUIName 获取TalkUI实例 这边不删除
    CDlgBaseTalkUI * GetTalkUIByDataNoDelete(int iSessionID, yl::string strTalkUIName);

protected:
    //该函数用于在获取通话子窗口的时候，清除同ID中不需要缓存的子窗口
    void CheckTalkUIBySessionID(int iSessionID);

public:
    //根据传入的SessionID和TalkUIName查找对应的通话子界面
    CDlgBaseTalkUI * FindTalkUIBySessionId(int iSessionId, yl::string strTalkUIName = "");

    //清除与SessionID相关的TalkUI实例
    void ClearTalkUIBySessionID(int iSessionID);

    //把session对应的数据设置为无效  即sessionid设置为 错误的
    void SetSessionDataInvalid(int iSessionID);

    //清除错误SessionID 的 通话界面
    void ClearInvalidTalkUI();

    //清除所有TalkUI实例
    void ClearAllTalkUI();

    int GetTalkUICount();

public:
    //主要是提供给  拨号界面使用的 t48  使用 该接口用于 CTalkUIManager::ExitDialingUI()
    CDlgBaseTalkUI * GetTalkUIByTalkUIName(yl::string strTalkUIName);
    //根据对话框的名称清除对应的对话框 主要是用于进入拨号界面的时候 清除其他sessionID对应的拨号界面
    void ClearTalkUIByTalkUIName(yl::string strTalkUIName);

protected:
    //从缓存中获取TalkUI实例
    CDlgBaseTalkUI * GetTalkUIFromCache(int iSessionID, yl::string strTalkUIName);


protected:
    //创建CDlgBaseTalkUI实例
    CDlgBaseTalkUI * CreateTalkUI(int iSessionID, yl::string strTalkUIName);
    //释放CDlgBaseTalkUI实例
    void ReleaseTalkUI(CDlgBaseTalkUI * pTalkUI);

    bool IsSessionIDValid(int iSessionID);

protected:
    YLList<CDlgBaseTalkUI *> m_listTalkUI;

};

// 获取全局唯一的TalkUI管理对象
#define _TalkUIFactory CTalkUIFactory::GetInstance()


#endif
