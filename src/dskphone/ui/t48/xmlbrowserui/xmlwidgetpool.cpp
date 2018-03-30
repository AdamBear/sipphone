#include "xmlwidgetpool.h"

CXMLWidgetPool * CXMLWidgetPool::m_pOneSelf = NULL;

// Gets the CXMLWidgetPool instance.
CXMLWidgetPool * CXMLWidgetPool::GetInstance()
{
    if (NULL == m_pOneSelf)
    {
        m_pOneSelf = new CXMLWidgetPool();
    }

    return m_pOneSelf;
}

// Release the CXMLWidgetPool instance.
void CXMLWidgetPool::ReleaseInstance()
{
    if (m_pOneSelf != NULL)
    {
        delete m_pOneSelf;
        m_pOneSelf = NULL;
    }
}

CXMLWidgetPool::CXMLWidgetPool()
{

}

CXMLWidgetPool::~CXMLWidgetPool()
{
    ClearWidgetPool();
}

// 将控件放回控件池
void CXMLWidgetPool::PutWidget(QWidget * pWidget)
{
    if (pWidget == NULL)
    {
        return;
    }

    // 遍历链表释放相关的控件
    ListWidgetInfo::iterator it = m_listWidgetInfo.begin();
    for (; it != m_listWidgetInfo.end(); ++it)
    {
        if (it->pWidget == pWidget)
        {
            it->bUsed = false;// 设置控件为非使用状态
            it->pWidget->setParent(0);// 设置控件的父控件为空
            continue;
        }
    }

    XMLBROWSERUI_INFO("CXMLWidgetPool::PutWidget Pool Size(%d)", m_listWidgetInfo.size());
}

// 将widget的所有子控件放回控件池
void CXMLWidgetPool::PutChildrenWidget(QWidget * pParentWidget)
{
    // 入口参数判断
    if (NULL == pParentWidget)
    {
        return;
    }

    // 遍历链表释放相关的控件
    ListWidgetInfo::iterator it = m_listWidgetInfo.begin();
    for (; it != m_listWidgetInfo.end(); ++it)
    {
        if (isChildOf(it->pWidget, pParentWidget))
        {
            it->bUsed = false;// 设置控件为非使用状态
            it->pWidget->setParent(0);// 设置控件的父控件为空
        }
    }
}

// 清空所有的控件
void CXMLWidgetPool::ClearWidgetPool()
{
    // 遍历链表释放相关的控件
    ListWidgetInfo::iterator it = m_listWidgetInfo.begin();
    for (; it != m_listWidgetInfo.end(); ++it)
    {
        // 释放未使用的控件资源
        // 以使用的控件交由其父窗口处理
        if (it->pWidget != NULL
                && !it->bUsed)
        {
            it->pWidget->setParent(0);
            it->pWidget->deleteLater();
            it->pWidget = NULL;
        }
    }

    // 清空链表
    m_listWidgetInfo.clear();
}
