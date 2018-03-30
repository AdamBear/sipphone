#ifndef _TALKUI_SESSION_CACHE_MANAGER_H
#define _TALKUI_SESSION_CACHE_MANAGER_H

#include "talkuilogiccommon.h"

#include "yllist.h"

#include <ylstring.h>

//processorname 转移
//用于talkuilogic内部切换界面处理
//talking 、rtpstatus 和 conf 、rtpstatus 切换不在这， 因为通话状态变化是会导致rtpstatus不显示的
struct tagTalkUIProcessNameTrans
{
    yl::string strProcessorNameSrc;     //源processorname
    yl::string strProcessorNameDst;     //目标processorname

    void Reset()
    {
        strProcessorNameSrc = "";
        strProcessorNameDst = "";
    }

    tagTalkUIProcessNameTrans()
    {
        Reset();
    }

    tagTalkUIProcessNameTrans & operator=(const tagTalkUIProcessNameTrans & objTalkUIProcessorNameTrans)
    {
        if (this == &objTalkUIProcessorNameTrans)
        {
            return *this;
        }

        this->strProcessorNameSrc = objTalkUIProcessorNameTrans.strProcessorNameSrc;
        this->strProcessorNameDst = objTalkUIProcessorNameTrans.strProcessorNameDst;

        return *this;

    }

};
typedef struct tagTalkUIProcessNameTrans CTalkUIProcessNameTrans;


//session缓存数据
struct tagTalkUISessionCacheData
{
    int iSessionID;     //通话的SessionID
    talkui_list<CTalkUIProcessNameTrans> listProcessorNameTrans;        //processorname映射队列

    tagTalkUISessionCacheData()
    {
        Reset();
    }

    void Reset()
    {
        iSessionID = -1;
        listProcessorNameTrans.clear();
    }

    //tagTalkUISessionCacheData& operator=(const tagTalkUISessionCacheData& objTalkUISessionCacheData)
    //{
    //  if(this == &objTalkUISessionCacheData)
    //  {
    //      return *this;
    //  }

    //  this->iSessionID = objTalkUISessionCacheData.iSessionID;

    //  this->vectorProcessorNameTrans.clear();
    //  for(int i=0;i<objTalkUISessionCacheData..vectorProcessorNameTrans.size();i++)
    //  {
    //      this->vectorProcessorNameTrans.push_back(objTalkUISessionCacheData.vectorProcessorNameTrans[i]);
    //  }

    //  return *this;
    //}

    //根据源processorname 获取 目标processorname
    yl::string GetProcessorNameDstByProcessorNameSrc(const yl::string & strProcessorNameSrc)
    {
        if (strProcessorNameSrc.empty())
        {
            return "";
        }

        talkui_list<CTalkUIProcessNameTrans>::iterator iteratorList = listProcessorNameTrans.begin();

        for (; listProcessorNameTrans.end() != iteratorList; iteratorList++)
        {
            CTalkUIProcessNameTrans & objProcessorNameTrans = *iteratorList;

            if (objProcessorNameTrans.strProcessorNameSrc == strProcessorNameSrc)
            {
                return objProcessorNameTrans.strProcessorNameDst;
            }
        }

        return "";
    }

    void AddProcessorNameTrans(const yl::string & strProcessorNameSrc,
                               const yl::string & strProcessorNameDst)
    {
        if (strProcessorNameSrc.empty() || strProcessorNameDst.empty())
        {
            return ;
        }

        talkui_list<CTalkUIProcessNameTrans>::iterator iteratorList = listProcessorNameTrans.begin();

        for (; listProcessorNameTrans.end() != iteratorList; iteratorList++)
        {
            CTalkUIProcessNameTrans & objProcessorNameTrans = *iteratorList;

            if (objProcessorNameTrans.strProcessorNameSrc == strProcessorNameSrc)
            {
                objProcessorNameTrans.strProcessorNameDst = strProcessorNameDst;

                return ;
            }
        }

        CTalkUIProcessNameTrans objProcessNameTransAdd;
        objProcessNameTransAdd.strProcessorNameSrc = strProcessorNameSrc;
        objProcessNameTransAdd.strProcessorNameDst = strProcessorNameDst;

        listProcessorNameTrans.push_back(objProcessNameTransAdd);

    }

    void DeleteProcessorNameTrans(const yl::string & strProcessorNameSrc)
    {
        if (strProcessorNameSrc.empty())
        {
            return ;
        }

        talkui_list<CTalkUIProcessNameTrans>::iterator iteratorList = listProcessorNameTrans.begin();

        for (; listProcessorNameTrans.end() != iteratorList; iteratorList++)
        {
            CTalkUIProcessNameTrans & objProcessorNameTrans = *iteratorList;

            if (objProcessorNameTrans.strProcessorNameSrc == strProcessorNameSrc)
            {
                listProcessorNameTrans.erase(iteratorList);
                break;
            }
        }


    }

};
typedef struct tagTalkUISessionCacheData CTalkUISessionCacheData;


class CTalkUISessionCacheManager
{
public:
    static CTalkUISessionCacheManager & GetInstance();

protected:
    CTalkUISessionCacheManager();

public:
    virtual ~CTalkUISessionCacheManager();

public:
    bool Init();
    void Uninit();


public:
    yl::string GetProcessorNameTrans(int iSessionID, const yl::string & strProcessorNameSrc);
    void AddProcessorNameTrans(int iSessionID, const yl::string & strProcessorNameSrc,
                               const yl::string & strProcessorNameDst);
    void DeleteProcessorNameTrans(int iSessionID, const yl::string & strProcessorNameSrc);


public:
    //根据sessionID 清除session数据缓存
    void ClearSessionCache(int iSessionID);
    //清除所有的session数据缓存
    void ClearAllSessionCache();

public:
    talkui_list<CTalkUISessionCacheData> m_listSessionCacheData;        //session缓存数据


};

#define g_objTalkUISessionCacheManager CTalkUISessionCacheManager::GetInstance()



#endif


