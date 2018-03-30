#ifndef Q_WIDGET_VIEW
#define Q_WIDGET_VIEW

#include <vector>
#include <list>
#include <ylstring.h>
#include <QFrame>
#include <QVariant>
#include "quipushbutton.h"

// 前向声明
class qFramePaint;
class qWidgetListItemData;
class AnimationData;

// 插入Item的类型
enum ITEM_TYPE
{
    ITEM_TYPE_BASE,
    ITEM_TYPE_LABEL,
    ITEM_TYPE_BUTTON,
    ITEM_TYPE_TIPBTN,
};

// 插入Item的动画
enum INSERT_ITEM_ANIMATION
{
    INSERT_ITEM_NONE,
    INSERT_ITEM_MOVE,
};

// 插入Item时其他Item的动画
enum OTHERS_ITEM_ANIMATION
{
    OTHERS_ITEM_NONE,
    OTHERS_ITEM_MOVE,
};

// 动画的信息
struct AnimationData
{
    INSERT_ITEM_ANIMATION  eInsertItemAnimation;
    OTHERS_ITEM_ANIMATION  eOtherItemAnimation;

    AnimationData()
    {
        eInsertItemAnimation = INSERT_ITEM_NONE;
        eOtherItemAnimation  = OTHERS_ITEM_NONE;
    }
};

struct qWidgetListItemDataBase
{
    ITEM_TYPE     eItemType;// Item的类型
    QString       strId;
    AnimationData objAnimationData;// 动画信息
    QSize         itemSize;     // item的size
    QVariant      appendData;   // 附加数据

    qWidgetListItemDataBase()
    {
        eItemType = ITEM_TYPE_BASE;
    }
};

// Item为lab时的数据
struct qWidgetListItemDataLab : public qWidgetListItemDataBase
{
    QString strPicPath;

    qWidgetListItemDataLab()
    {
        eItemType = ITEM_TYPE_LABEL;
    }
};

// Item为button时的数据
struct qWidgetListItemDataButton : public qWidgetListItemDataBase
{
    QString     strAction;
    QObject  *  actionReceiver;
    const char * actionMember;

    QString strNormalPic;
    QString strEnablePic;
    QString strDisablePic;

    qWidgetListItemDataButton()
    {
        eItemType = ITEM_TYPE_BUTTON;
        actionReceiver = NULL;
        actionMember = NULL;
    }
};

struct qWidgetListItemDataTipButton : public qWidgetListItemDataButton
{
    QString     strLabel;

    qWidgetListItemDataTipButton()
    {
        eItemType = ITEM_TYPE_TIPBTN;
        actionReceiver = NULL;
        actionMember = NULL;
        strLabel = "";
    }
};

struct OffSetData
{
    int  iXOffSet;
    int  iYOffSet;

    OffSetData()
    {
        iXOffSet = 0;
        iYOffSet = 0;
    }
};

struct qWidgetListItemInfo
{
    qWidgetListItemDataBase objData;
    QWidget        *        pWidget;

    qWidgetListItemInfo()
    {
        pWidget = NULL;
    }
};

//提示型button控件
class CTipButton : public qUIPushButton
{
    Q_OBJECT

public:
    CTipButton(QWidget * parent);
    ~CTipButton();

protected:
    //绘图函数
    void paintEvent(QPaintEvent * pPaintEvent);
};

// 重命名内部数据
typedef std::list<qWidgetListItemInfo>  listWidget;
typedef listWidget::iterator            IterWidget;

class qWidgetList : public QFrame
{
    Q_OBJECT

public:
    // Item的对齐方式
    enum ITEM_ALIGN_TYPE
    {
        ALIGN_TYPE_LEFT,
        ALIGN_TYPE_RIGHT,
        ALIGN_TYPE_TOP,
        ALIGN_TYPE_BOTTOM,
    };

public:
    qWidgetList(QWidget * parent, ITEM_ALIGN_TYPE eAlignType, QRect rect);
    ~qWidgetList();

    // 判断某个Item是否存在
    bool IsItemExist(const yl::string & strId);

    // 添加Item
    void AddItem(const qWidgetListItemDataBase * pItem, int iIndex = -1);

    // 删除Item
    void DeleteItem(const AnimationData & objAnimationData, int iIndex = -1);

    // 删除对应ID的Item
    void DeleteItem(const AnimationData & objAnimationData, const yl::string & strId);

    // 获取有几项
    int GetIconNum();

    // 清除控件数据
    void Clear();

private:
    //改变大小事件
    virtual void resizeEvent(QResizeEvent * pResizeEvent);

    // 根据动画的类型,将iterBegin以后的Item移动iMoveRange的距离
    void MoveItem(IterWidget iterBegin, int iMoveRange,
                  const AnimationData & objAnimationData);

    // 按OffSetData移动Item
    void MoveItem(IterWidget iterBegin, const OffSetData & objOffsetData);

    // 根据移动的次数计算每次移动的距离
    void CountMoveOffset(int iMoveRange, int iMoveTime);

    // 计算所有类型的长度
    int CountAllWidgetLength();

    // 显示插入的Item
    void ShowInsertItem(QWidget * pInsertWidget, const AnimationData & objAnimationData);

    // 根据Type创建控件
    QWidget * CreateWidgetByType(const qWidgetListItemDataBase * pItem);

    // 删除对应迭代器的Item
    void DeleteItem(const AnimationData & objAnimationData, IterWidget it);

protected:
    ITEM_ALIGN_TYPE m_eAlignType;   // Item对齐的方式
    listWidget      m_listWidget;   // 保存widget指针的list
    QFrame          m_MoveFrame;    // 移动的widget

    static const int MOVE_TIME = 4; // 移动的次数
    static const int MOVE_INTERVAL = 100;// 移动的时间间隔
    static const int LIST_MAX_SIZE = 20; // 最大size
    QTimer     *     m_pMoveTimer;  // 控制空间移动间隔的计时器
    std::list<OffSetData> m_listOffsetData; // 每次Item移动的距离

private slots:
    void OnMoveTimerOut();
};

#endif // Q_WIDGET_VIEW
