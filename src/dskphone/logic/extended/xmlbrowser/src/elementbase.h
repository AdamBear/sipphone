#ifndef __ELEMENT_BASE_H__
#define __ELEMENT_BASE_H__

#ifdef IF_FEATURE_XMLBROWSER

#include "xmlbrowserparser.h"

// 实现数据转换，需要在各CElementBase的子类头文件中声明以下宏
#define IMPLEMENT_GETXDATA(datatype) \
        datatype& _GetXData() \
        {\
            datatype* pData = static_cast<datatype*>(this->GetData());\
            if (pData != NULL)\
            {\
                return *pData;\
            }\
            static datatype s_stData;\
            return s_stData;\
        }

//////////////////////////////////////////////////////////////////////////
// Element基类
class CElementBase
{
    friend class CElementStatus;
public:
    CElementBase();
    virtual ~CElementBase();

    // 初始化数据
    inline void InitData(XMLParseData * pParseData);

    // 创建之后的处理
    virtual bool AfterCreate() = 0;
    // 销毁之前的处理
    virtual void BeforeDestroy() = 0;
    // 是否播放提示音
    virtual bool IsNeedPlayBeep() = 0;

    // 获取指定控件的号码,无号码则返回空
    virtual yl::string GetItemNumberByIndex(UINT uIndex)
    {
        return "";
    }

    // 事件处理
    virtual bool OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd, bool & bUpdate,
                          bool bDestroyOnExit = false)
    {
        return false;
    }

    // 获取Element类型
    inline XML_ELEMENT_TYPE GetType();

    // 获取元素ID
    inline int GetID()
    {
        return m_nElementID;
    }

    // !! 获取对象数据指针
    inline void * GetData();

    int GetDsskeyId()
    {
        return m_pParseData->nDsskeyId;
    }

#if IF_BUG_32431
    inline yl::string GetAutoRefreshURL();
#endif

protected:
    // 判断并处理合法的URL
    void ProcessLegalURL(yl::string & strURL);

protected:
    int                m_nElementID;    // 对象ID

private:
    // 取出数据, StatusElement 有用
    inline void * TakeoutData();
    // 释放数据
    inline void ReleaseData();

private:
    XMLParseData   *   m_pParseData;    // 解析后的数据
};

// 初始化数据
inline void CElementBase::InitData(XMLParseData * pParseData)
{
    if (m_pParseData != NULL)
    {
        ReleaseData();
    }
    m_pParseData = pParseData;
}

// 获取对象类型
inline XML_ELEMENT_TYPE CElementBase::GetType()
{
    if (m_pParseData != NULL)
    {
        return m_pParseData->eType;
    }
    return XMT_UNKNOWN;
}

// 获取对象数据指针
inline void * CElementBase::GetData()
{
    if (m_pParseData != NULL)
    {
        return m_pParseData->pData;
    }
    return NULL;
}

// 取出数据, StatusElement 有用
inline void * CElementBase::TakeoutData()
{
    void * pTakeout = GetData();
    if (m_pParseData != NULL)
    {
        m_pParseData->pData = NULL;
    }
    return pTakeout;
}

// 释放数据
inline void CElementBase::ReleaseData()
{
    if (m_pParseData != NULL)
    {
        ReleaseParseData(m_pParseData);
        m_pParseData = NULL;
    }
}

#if IF_BUG_32431
inline yl::string CElementBase::GetAutoRefreshURL()
{
    if (m_pParseData != NULL)
    {
        return m_pParseData->sRefreshData.strURL;
    }
    return yl::string();
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __ELEMENT_BASE_H__
