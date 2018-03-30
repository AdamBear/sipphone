#ifndef __SEARCH_TABLE_WIDGET_H__
#define __SEARCH_TABLE_WIDGET_H__

#include "baseui/framelist.h"
#include "baseui/framelistdelegate.h"
#include "baseui/searchlistitem.h"
#include "contacts/directory/include/commondata.h"

/////////////////////////数据定义/////////////////////////

// 搜索完成后选择Item的模式
enum SELECT_DATA
{
    SELECT_DATA_CUSTOMLIST,     // 自定义
    SELECT_DATA_UPSERSEARCH,    // 超级搜索
    SELECT_DATA_MERGE,          // 两者合并
};

// 搜索的数据项
struct SearchItemData
{
    SearchItemData()
    {
        strDisplay = "";
        strNumber = "";
        strContactPic = "";
        strPresencePic = "";
        varData = QVariant();
    }
    // 显示数据
    QString strDisplay;
    // 号码数据
    QString strNumber;
    // 联系人头像
    QString     strContactPic;
    // 联系人状态
    QString     strPresencePic;

    // 自定义数据
    QVariant varData;

    bool operator!=(const SearchItemData & data)
    {
        if (strDisplay != data.strDisplay
                || strNumber != data.strNumber
                || strContactPic != data.strContactPic
                || strPresencePic != data.strPresencePic)
        {
            return true;
        }

        return false;
    }
};

// 用户自定义项列表
typedef std::list<SearchItemData> ListCustomItem;

/////////////////////////代理类/////////////////////////
class CSearchTypeDelegate : public CFrameListDelegate
{
public:
    CSearchTypeDelegate();
    virtual ~CSearchTypeDelegate(void);

public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    // 设置用户自定义项列表
    void SetCustomList(const ListCustomItem & listCustomItem);

    // 判断自定义数据是否改变，有改变则返回true
    bool IsCustomListChanged(ListCustomItem & listCustomItem);

    inline void SetSelectData(SELECT_DATA nType)
    {
        m_dataSelect = nType;
    }

    SearchItemData GetSearchData(int iDataIndex);

protected:
    yl::string GetSearchString();

    int GetShowData(ListCustomItem& objData, int nDataIndex, int nDataCount);

    // 获取自定义的数据
    int GetCustomListData(ListCustomItem& objData, int nDataIndex, int nDataCount);

    // 获取超级搜索的数据
    int GetSuperSearchData(ListCustomItem& objData, int nDataIndex, int nDataCount);

    int GetMergeData(ListCustomItem& objData, int nDataIndex, int nDataCount);

    void InitSearchItem(CSearchListItemPtr pSearchItem, const SearchItemData& objItemData, int nIndex);

private:
    SELECT_DATA     m_dataSelect;

    // 用户自定义项列表
    ListCustomItem  m_listCustomItem;

};

/////////////////////////列表类//////////////////////////
class CSearchTable : public CFrameList
{
    Q_OBJECT

public:
    CSearchTable(QWidget* parent);
    ~CSearchTable();

    // 按照所选字符串进行搜索
    void Search(LPCSTR lpszSearchText);

    // 搜索逻辑会卡住 所以这边判断一下key值
    void ShowSearchPageByKeyAnIndex(const yl::string& strKey, int nPageIndex);

    yl::string GetSearchString();

    // 直接设置用户自定义项列表
    void SetCustomList(const ListCustomItem & listCustomItem);

    // 判断自定义数据是否改变，有改变则返回true
    bool IsCustomListChanged(ListCustomItem & listCustomItem);

    // 清空用户自定义项列表
    void ClearCustomList();

    // 清空数据
    void ClearData(bool bClearCustomList = true);

    // 设置是否合并穿梭和搜索数据内容
    void SetMergeData(bool bMergeData);

    bool IsSelectItem();

    SearchItemData GetCurrentItemContent();

    void SendClickEvent(int iDataIndex, const QString& strNumber, const QVariant& varData);

protected:
    // 控件初始化
    void InitWidget();

signals:
    // 点击某一项，发出的信号
    void clickItem(const QString & strNumber, const QVariant & varData);

private slots:
    // 定时器到时后的搜索.
    void OnTimeroutSearch();

private:
    // 选中的列表位置
    int m_iDataIndex;

    // 搜索字符串
    QString m_strSearch;

    // 延迟搜索的定时器.
    QTimer m_tmrSearch;

    // 方便访问直接保存一个指针
    CSearchTypeDelegate* m_pSearchDelegate;
};


#endif //__SEARCH_RESULT_WIDGET_H__
