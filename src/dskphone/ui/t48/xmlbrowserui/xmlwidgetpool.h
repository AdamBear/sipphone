#ifndef XML_WIDGET_POOL
#define XML_WIDGET_POOL

#include <list>
#include <QWidget>
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"

// 前向声明
class QWidget;

struct WidgetInfo
{
    QWidget  * pWidget;
    bool       bUsed;  // 标记Widget是否被使用

    WidgetInfo()
    {
        pWidget = NULL;
        bUsed   = false;
    }
};
typedef std::list<WidgetInfo> ListWidgetInfo;

class CXMLWidgetPool
{
public:
    // Gets the CXMLWidgetPool instance.
    static CXMLWidgetPool * GetInstance();

    // Release the CXMLWidgetPool instance.
    static void ReleaseInstance();

    // 从控件池中取出控件
    template<class typeWidget>
    typeWidget * GetWidgetFromPool(QWidget * pParent = 0, bool bCreate = false)
    {
        typeWidget * pTypeWidget = NULL;
        if (!bCreate)
        {
            //遍历链表,当控件池里有空闲的需要的的控件时就取出,否则就创建,并且放在链表中
            ListWidgetInfo::iterator it = m_listWidgetInfo.begin();
            for (; it != m_listWidgetInfo.end(); ++it)
            {
                if (!it->bUsed) // 判断控件是否被使用
                {
                    if (it->pWidget != NULL)
                    {
                        if (it->pWidget->parent() != NULL)// 处于未使用状态的控件不应该有父控件
                        {
                            // 给出警告
                            chWARNING2(false, "this unUse widget have parent!");
                            XMLBROWSERUI_INFO("widgetName(%s), widgetParentName(%s)",
                                              it->pWidget->objectName().toUtf8().data(),
                                              it->pWidget->parent()->objectName().toUtf8().data());
                        }
                        else
                        {
                            // 强制转化指针类型,判断控件是否符合
                            pTypeWidget = dynamic_cast<typeWidget *>(it->pWidget);
                        }
                    }

                    // 判断是否找到相应的控件
                    if (pTypeWidget != NULL)
                    {
                        it->bUsed = true;
                        pTypeWidget->setParent(pParent);
                        break;
                    }
                }
            }
        }

        // 控件池中没有相应的控件自己创建
        if (pTypeWidget == NULL)
        {
            pTypeWidget = new typeWidget(pParent);

            // 判断创建是否成功
            if (pTypeWidget == NULL)
            {
                chWARNING2(false, "CXMLWidgetPool::GetWidgetFromPool Create widget error!");
            }
            else
            {
                // 保存创建的控件信息
                WidgetInfo objWgtInfo;
                objWgtInfo.bUsed = true;
                objWgtInfo.pWidget = pTypeWidget;
                m_listWidgetInfo.push_back(objWgtInfo);
            }
        }

        // 打印控件池的Size防止控件过多
        XMLBROWSERUI_INFO("WidgetPoolSize(%d)", m_listWidgetInfo.size());
        return pTypeWidget;
    }

    // 将控件放回控件池
    void PutWidget(QWidget * pWidget);

    // 将widget的所有子控件放回控件池
    void PutChildrenWidget(QWidget * pParentWidget);

    // 清空所有的控件
    void ClearWidgetPool();

private:
    CXMLWidgetPool();
    ~CXMLWidgetPool();

private:
    static CXMLWidgetPool * m_pOneSelf;
    ListWidgetInfo  m_listWidgetInfo;
};

#define g_pXMLWidgetPool (CXMLWidgetPool::GetInstance())

#endif // XML_WIDGET_POOL
