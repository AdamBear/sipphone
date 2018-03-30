//mod-Qt
#include <qstyleoption.h>
#include <qpainter.h>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <QtGui>

//mod-this(PaltformStyle)
#include "t4xstyle.h"
#include "qtcommon/qmisc.h"

//mod-Define
#include "uicommon.h"
#include "uikernel/languagehelper.h"
#include "t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qswitchpagelistview.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include <ylstl/ylstringalgorithm.h>
#include "commonunits/commonconfig.h"
#include "qiconview.h"
#include "language/input/include/modInput.h"

const int I_SWITCH_PAGE_LIST_TEXT_OFFSET = 15;// SwitchPageList向左偏移的距离

const int knComboBoxImageWidth = 51;  // 组合框中的箭头图片向左偏移大小.

#define COMBOX_EDIT_ARROW_SPACE 6

const int knScrollBarEdgePixelWidth = 2;// 滚动条的背景图片边缘像素宽度

const int MaxScrollItemPerPage = 4;         //滚动条最大个数设置

//const QColor SUBMENU_LINE_COLOR = QColor(162, 161, 161, 255);

const int MENUVIEW_SEPARATE_LINE_HEIGHT = 2;

#if TEST_TRAN_LEN
#include <fstream>
#include "qiconview.h"
#define lpzTranLenFile CONFIG_PATH "TranslateLength.txt"
#define lpzSeparate " -------------------------------- "
using namespace std;
struct CharCountInfo
{
    QString strFamily;//字体
    int iMaxCharCount;//最大单词数

    CharCountInfo(QString strFontFamily, int iMaxCount)
    {
        strFamily = strFontFamily;
        iMaxCharCount = iMaxCount;
    }
};
typedef map<QString, CharCountInfo> MAP_TranCharCount;
static MAP_TranCharCount
map_TranCharCount;//用于存放字符串的翻译结果允许的最多字符个数（以'a'计算字符宽度）
static QTimer timer_tran_save;
#define time_note_string "weekday + mounth"
#define time_other_string "33 33 PM   33"
static int iTime_length = 0;
#define LANG_FLAG           "?`"
#define LANG_STRING_LEN_FLAG    2

//从strText当中提取出保存到map的key，如果strText中没有翻译项那么直接以strText为key，
//如果strText中要翻译的项不止一项，则以政局为key,只有一项是，提取出来作为key
void GetRealTranText(const QString strText, QString & strRealTran, QString & strOther)
{
    strRealTran = "";
    strOther = "";
    if (strText.count(LANG_FLAG) != 1)
    {
        strRealTran = strText;
        strOther = "";
        return;
    }
    else
    {
        yl::string strTran = strText.toUtf8().data();
        //保存翻译标记的位置
        int iPos = strTran.find(LANG_FLAG, 0);
        if (iPos == yl::string::npos)
        {
            return;
        }

        //取得第一个要翻译的长度
        int iLength = atoi(strTran.substr(iPos + strlen(LANG_FLAG), LANG_STRING_LEN_FLAG).c_str());

        //提取第一段
        yl::string strFirst = strTran.substr(0, iPos);

        //提取最后一段
        yl::string strThird = strTran.substr(iPos + iLength + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG);

        //翻译中间段，生成新的中间段
        yl::string strToTrans = strTran.substr(iPos + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG, iLength);

        strRealTran = toQString(strToTrans.c_str());

        strOther.append(strFirst.c_str());
        strOther.append(strThird.c_str());
    }
}

//添加一项到map
bool AddTranLenInfo(QString strKey, QString strFontFamily, int iCharMax)
{
    if (strKey.isEmpty())
    {
        return false;
    }

    MAP_TranCharCount::iterator iter = map_TranCharCount.find(strKey);
    if (iter != map_TranCharCount.end())
    {
        CharCountInfo info = iter->second;
        //同一字符串出现在不同的控件中，字符数取最小值
        if (iCharMax < info.iMaxCharCount)
        {
            iter->second.iMaxCharCount = iCharMax;
        }
    }
    else
    {
        map_TranCharCount.insert(pair<QString, CharCountInfo>(strKey, CharCountInfo(strFontFamily,
                                 iCharMax)));
    }
    return true;
}

//
void CountCharAndPushTranInfo(QString text, QString strFontFamily,
                              const QFontMetrics & objFontMetrics, int iTextAreaWidth)
{
    if (text.isEmpty())
    {
        return;
    }

    //获取Key
    QString strKey = "";
    QString strOther = "";
    //提取出保存到map的key
    GetRealTranText(text, strKey, strOther);

    int iTranWordWith = iTextAreaWidth - objFontMetrics.width(strOther);
    int iCharMax = CountMaxCharInCtrl(objFontMetrics, iTranWordWith);

    AddTranLenInfo(strKey, strFontFamily, iCharMax);
}

bool IsIdleTimeFormat(QString strTran)
{
    //01 51 PM ?`03Fri ?`03Sep 06
    int iHour;
    int iMin;
    int iDay;
    char szAMPM[4] = {0};
    char szWeek[4] = {0};
    char szMonth[4] = {0};
    int iResualt =  sscanf(((const char *)(strTran.toUtf8().data())), "%d %d %s ?`03%s ?`03%s %d",
                           &iHour, &iMin, szAMPM, szWeek, szMonth, &iDay);

    if (iResualt == 6)
    {
        return true;
    }

    iResualt =  sscanf(((const char *)(strTran.toUtf8().data())), "%d:%d %s ?`03%s ?`03%s %d",
                       &iHour, &iMin, szAMPM, szWeek, szMonth, &iDay);

    if (iResualt == 6)
    {
        return true;
    }

    iResualt =  sscanf(((const char *)(strTran.toUtf8().data())), "%d %d ?`03%s ?`03%s %d",
                       &iHour, &iMin, szWeek, szMonth, &iDay);

    if (iResualt == 5)
    {
        return true;
    }

    iResualt =  sscanf(((const char *)(strTran.toUtf8().data())), "%d:%d ?`03%s ?`03%s %d",
                       &iHour, &iMin, szWeek, szMonth, &iDay);

    if (iResualt == 5)
    {
        return true;
    }
    return false;
}

//开机时从文件中读取上次保存的内容，防止重写时原来的数据丢失
void LoadTranMapFromFile(yl::string strLangPath)
{
    map_TranCharCount.clear();
    yl::string strSeporate = lpzSeparate;

    ifstream tranFile(lpzTranLenFile);    //打开文件用于读
    if (!tranFile)
    {
        return;
    }
    yl::string strTemp;
    while (tranFile.good() && !tranFile.eof())
    {
        yl::string strKey;
        yl::string strLen;
        yl::string strFamily;
        yl::string strLineInfo;
        getline(tranFile, strLineInfo, '\n');

        int iPos = strLineInfo.find(strSeporate);
        if (iPos != yl::string::npos)
        {
            //提取翻译关键字
            strKey = strLineInfo.substr(0, iPos);
            //查找“ (”以便提取长度
            int iFamilyStart = strLineInfo.rfind(" (");
            if (iFamilyStart != yl::string::npos)
            {
                strLen = strLineInfo.substr(iPos + strSeporate.length(),
                                            iFamilyStart - (iPos + strSeporate.length()));
                int iFamlEnd = strLineInfo.rfind(")");
                if (iFamlEnd != yl::string::npos)
                {
                    strFamily = strLineInfo.substr(iFamilyStart + 2, iFamlEnd - iFamilyStart - 2);
                }
            }
            if (!strKey.empty() && ! strLen.empty())
            {
                AddTranLenInfo(strKey.c_str(), strFamily.c_str(), atoi(strLen.c_str()));
            }
        }
    }
}

#endif

// 内部函数声明
//////////////////////////////////////////////////////////////////////////
// class qMyStyle
#if TEST_TRAN_LEN
//保存翻译的计时器到时，保存翻译长度到文件
void qT4XStyle::OnTranSaveTimeout()
{
    ofstream tranFile(
        lpzTranLenFile);           //打开文件用于写，若文件不存在就创建它
    if (!tranFile)
    {
        return;    //打开文件失败则结束运行
    }
    MAP_TranCharCount::iterator iter = map_TranCharCount.begin();
    for (; iter != map_TranCharCount.end(); ++iter)
    {
        tranFile << iter->first.toUtf8().data() << lpzSeparate << iter->second.iMaxCharCount
                 << " (" << iter->second.strFamily.toUtf8().data() << ")" <<
                 endl; //使用插入运算符写文件内容
    }
    tranFile.close();
}
#endif

qT4XStyle::qT4XStyle()
    : BaseClass()
{
#if TEST_TRAN_LEN
    timer_tran_save.stop();
    timer_tran_save.start(40 * 1000);
    QObject::connect(&timer_tran_save, SIGNAL(timeout()), this, SLOT(OnTranSaveTimeout()));
    LoadTranMapFromFile(ROOT_PATH "config\\TranslateLength.txt");
#endif
}

qT4XStyle::~qT4XStyle()
{

}

void qT4XStyle::drawControl(ControlElement element, const QStyleOption * option,
                            QPainter * painter, const QWidget * widget) const
{
    // 入口指针判空,此处widget可以为空不需要判空
    if (NULL == option || NULL == painter)
    {
        return;
    }

    switch (element)
    {
    // Set QCheckBox or QRadioButton background transparency.
    // draw focus rect by ourself
    case CE_CheckBox:
    case CE_RadioButton:
        if (const QStyleOptionButton * btn = qstyleoption_cast<const QStyleOptionButton *>(option))
        {
            // 保存当前画笔
            painter->save();

            bool isRadio = (element == CE_RadioButton);// 判断是RadioButton还是CheckBox

            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator : SE_CheckBoxIndicator, btn, widget);
            drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox, &subopt, painter, widget);
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents : SE_CheckBoxContents, btn, widget);
            drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, painter, widget);

            if (btn->state & State_HasFocus)// 判断是否获得焦点
            {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*btn);
                fropt.rect = subElementRect(isRadio ? SE_RadioButtonFocusRect : SE_CheckBoxFocusRect, btn, widget);
                BaseClass::drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
            }

            // 恢复画笔为保存的画笔
            painter->restore();
        }
        break;

    // Here we draw comboBox text. if the text is not editable, we set the text color to Qt::darkGray
    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox * cb = qstyleoption_cast<const QStyleOptionComboBox *>(option))
        {
            painter->save();// 保存当前画笔
            // 获得comboBox的编辑框大小
            QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);

            const QString & strTran = cb->currentText;
            //进行字符的解析、翻译及重组
            QString strTemp = LANG_PARSE(cb->currentText);
            ReverseRTLMaskString(strTemp);

#if TEST_TRAN_LEN
            QComboBox * pBox = (QComboBox *)widget;
            QRect boxRect = pBox->rect();
            if (strTran.contains(LANG_FLAG))
            {
                CountCharAndPushTranInfo(strTran, painter->font().family(),
                                         painter->fontMetrics(), boxRect.width() - 4);
            }
#endif

            if (!cb->currentText.isEmpty()) // 判断当前的item的text是否为空
            {
                // CurrentItem未激活的话，设置其背景颜色
                if ((cb->state & State_Enabled) != State_Enabled)
                {
                    painter->setPen(Qt::darkGray);
                }

                // CurrentItem绘制的text
                painter->drawText(editRect.adjusted(1, 0, -1, 0),
                                  visualAlignment(cb->direction, Qt::AlignLeft | Qt::AlignVCenter), strTemp);
            }

            painter->restore();// 恢复保存的画笔
        }
        break;
    case CE_ItemViewItem:
        {
            const QStyleOptionViewItemV4 * vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option);
            if (vopt != NULL)
            {
                QStyleOptionViewItemV4 objItemOption(
                    *vopt);        //vopt不可改变，这里需要改变则对复制对象进行操作

                if (widget->inherits("CIconView"))
                {
                    const QString & strTran = objItemOption.text;
                    //进行文本的解析、翻译和重组，修改原始的文本属性
                    objItemOption.text = LANG_PARSE(objItemOption.text);
                    ReverseRTLMaskString(objItemOption.text);

                    const CIconView * pIconViewWidget = qobject_cast<const CIconView *>(widget);
                    if (pIconViewWidget)
                    {
                        // 截断文字
                        objItemOption.text =
                            CutOffText(objItemOption.text, painter->fontMetrics(), pIconViewWidget->GetItemSize().width() - 5);
                    }
#if TEST_TRAN_LEN
                    QSize textSize;//QRect textRect = subElementRect(SE_ItemViewItemText, vopt, widget);//vopt->rect;
                    CIconView * pIconVew = (CIconView *)widget;
                    textSize = pIconVew->gridSize();
                    if (strTran.contains(LANG_FLAG))
                    {
                        CountCharAndPushTranInfo(strTran, painter->font().family(),
                                                 painter->fontMetrics(), textSize.width() - 3);
                    }
#endif

                    // 先绘制ItemV4的样式
                    BaseClass::drawControl(element, &objItemOption, painter, widget);

                    //Idle界面的快捷菜单，不使用普通菜单图标的点击效果
                    if (!widget->inherits("CShortcutIconView") && (vopt->state & QStyle::State_Selected))
                    {
                        // 获得图标的位置
                        QRect rcDraw = subElementRect(SE_ItemViewItemDecoration, option, widget);

                        PixmapWeekPtr pm = myPxCache::GetPxCache(PxIconViewCover);

                        if (!pm.isNull())
                        {
                            // 设置选中效果的图片
                            painter->drawPixmap(rcDraw, *(pm.data()));
                        }
                    }
                    return;
                }

                if (widget->inherits("QComboBox"))
                {
                    const QString & strTran = vopt->text;
                    //对字符串进行解析、翻译和重组
                    objItemOption.text = LANG_PARSE(vopt->text);
                    ReverseRTLMaskString(objItemOption.text);

#if TEST_TRAN_LEN
                    QComboBox * pCombo = (QComboBox *)widget;
                    QRect textRect = pCombo->rect();//subElementRect(SE_ItemViewItemText, vopt, widget);//vopt->rect;
                    if (strTran.contains(LANG_FLAG))
                    {
                        CountCharAndPushTranInfo(strTran, painter->font().family(),
                                                 painter->fontMetrics(), textRect.width());
                    }
#endif

                    // 绘制Item
                    BaseClass::drawControl(element, &objItemOption, painter, widget);

                    return;
                }

                if (widget->inherits("CSwitchPageList"))
                {
                    objItemOption.text = "";// 设置绘制的text为空

                    // 绘制Item
                    BaseClass::drawControl(element, &objItemOption, painter, widget);

                    // 获得ItemText的位置
                    QRect textRect = subElementRect(SE_ItemViewItemText, vopt, widget);

                    // 设置文字的位置
                    textRect.setX(textRect.x() + I_SWITCH_PAGE_LIST_TEXT_OFFSET);
                    textRect.setWidth(textRect.width() + I_SWITCH_PAGE_LIST_TEXT_OFFSET);

                    const QString & strTran = vopt->text;
                    //对字符串进行解析、翻译和重组
                    QString strTemp = LANG_PARSE(vopt->text);
                    ReverseRTLMaskString(strTemp);

                    // 获得矩形范围的宽度，多减5是为了使截断效果更好
                    int nRectWidth = widget->width() - I_SWITCH_PAGE_LIST_TEXT_OFFSET - 5;
                    //如果滚动条开启，设置有效宽度减去滚动条宽度
                    QListWidget * pListWidget = (QListWidget *) widget;
                    if (pListWidget != NULL)
                    {
                        if (pListWidget->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn)
                        {
                            nRectWidth = nRectWidth - 35;
                        }
                    }

                    // 截断文字
                    strTemp = CutOffText(strTemp, painter->fontMetrics(), nRectWidth);
#if TEST_TRAN_LEN
                    CSwitchPageList * pList = (CSwitchPageList *)widget;
                    QRect pageListRect = vopt->rect;
                    int iLeftMargin = pList->contentsMargins().left();
                    if (strTran.contains(LANG_FLAG))
                    {
                        CountCharAndPushTranInfo(strTran, painter->font().family(),
                                                 painter->fontMetrics(), pageListRect.width() - I_SWITCH_PAGE_LIST_TEXT_OFFSET - iLeftMargin);
                    }
#endif
                    painter->save();
                    const QStyleOptionViewItemV4 * vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option);
#ifdef _T48
                    CSwitchPageList * pSwitchPageList = (CSwitchPageList *)widget;
                    if (NULL != pSwitchPageList)
                    {
                        //painter->setPen(QColor(vopt->index.row() == pListWidget->currentRow() ? Qt::white : Qt::black));
                        painter->setPen(QColor(vopt->index.row() == pSwitchPageList->GetCurrentItemIndex() ? Qt::white :
                                               Qt::black));
                    }
#endif
                    painter->drawText(textRect, Qt::AlignLeft & Qt::AlignVCenter, strTemp);
                    painter->restore();
                    return;
                }

                BaseClass::drawControl(element, option, painter, widget);
            }

        }
        break;
    case CE_ShapedFrame:
        {
#define COMBOX_BG_BOTTOM_HEIGHT 30
            if (NULL != widget->parent() && widget->parent()->inherits("QComboBox"))
            {
                painter->save();// 保存当前画笔
                painter->setPen(Qt::white);  // do this to draw the black rect line.

                // 绘制矩形区域
                QRect objRect = widget->rect();
                bool bPopBelow = true;
                // 判断下拉还是上拉
                if (NULL != widget && widget->parent()->inherits("QComboBox"))
                {
                    QComboBox * pCombobox = (QComboBox *)(widget->parent());
                    QAbstractItemView * pView = pCombobox->view();

                    if (NULL != pView && pView->isActiveWindow())
                    {
                        QPoint ptView = pView->mapToGlobal(QPoint(0, 0));
                        QPoint ptComboBox = pCombobox->mapToGlobal(QPoint(0, 0));

                        if (ptComboBox.y() > ptView.y())
                        {
                            bPopBelow = false;
                        }
                    }
                }
                // 下拉框或上拉框

                if (bPopBelow)
                {
                    QRect rcTop(objRect.left(), objRect.top(), objRect.width(),
                                objRect.height() - COMBOX_BG_BOTTOM_HEIGHT);
                    QPixmap pmTop = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_MID_BACKGROUND);

                    painter->drawPixmap(rcTop, pmTop);

                    QRect rcBottom(objRect.left(), objRect.top() + rcTop.height(), objRect.width(),
                                   COMBOX_BG_BOTTOM_HEIGHT);
                    QPixmap pmBottom = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_DOWN_BACKGROUND);

                    painter->drawPixmap(rcBottom, pmBottom);
                }
                else
                {
                    QRect rcTop(objRect.left(), objRect.top(), objRect.width(), COMBOX_BG_BOTTOM_HEIGHT);
                    QPixmap pmTop = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_UP_BACKGROUND);

                    painter->drawPixmap(rcTop, pmTop);

                    QRect rcBottom(objRect.left(), objRect.top() + COMBOX_BG_BOTTOM_HEIGHT,
                                   objRect.width(), objRect.height() - COMBOX_BG_BOTTOM_HEIGHT);

                    QPixmap pmBottom = THEME_GET_BMP(PIC_GENERAL_COMBO_LIST_MID_BACKGROUND);

                    painter->drawPixmap(rcBottom, pmBottom);
                }

                // 根据不同的界面绘制矩形边
                ////----add by qt:添加combobox显示的临时解决方案
//              if (isChildOf(widget->parent(), "CDlgSubMenu"))
//              {
//                  painter->drawRect(objRect.x() + 1, objRect.y() + 1, objRect.width() - 3, objRect.height() - 3);
//              }
//              else
//              {
//                  painter->drawRect(objRect.x(), objRect.y(), objRect.width() - 1, objRect.height() - 1);
//              }

//              // 如果有滚动条则绘制分割线
//              QComboBox* pComBobox = (QComboBox*)widget->parent();
//              if (pComBobox->maxVisibleItems() < pComBobox->count())
//              {
//                  painter->drawLine(objRect.topRight().x() - 40, objRect.topRight().y(),
//                      objRect.bottomRight().x() - 40, objRect.bottomRight().y());
//              }

                painter->restore();// 恢复保存的画笔
//              BaseClass::drawControl(element, option, painter, widget);
            }
            else
            {
                BaseClass::drawControl(element, option, painter, widget);
            }
        }
        break;
    case CE_MenuHMargin:
    case CE_MenuVMargin:
    case CE_MenuEmptyArea:
        break;

    case CE_MenuItem:
        myDrawMenuItem(element, option, painter, widget);
        break;

    default:
        BaseClass::drawControl(element, option, painter, widget);
        break;
    }
}

void qT4XStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
                              QPainter * painter, const QWidget * widget /* = 0 */) const
{
    // 入口指针判空,此处widget可以为空不需要判空
    if (NULL == option || NULL == painter)
    {
        return;
    }

    switch (element)
    {
    // draw QLineEdit background using custom color.
    case PE_PanelLineEdit:
        if (const QStyleOptionFrame * panel = qstyleoption_cast<const QStyleOptionFrame *>(option))
        {
            painter->save();// 保存当前画笔

            // 获得框和背景的颜色
            QColor clrBase = panel->palette.color(QPalette::Base);
            QPixmap pixBase = panel->palette.brush(QPalette::Base).texture();

            // 判断背景是否透明，如果为透明, 则不需要绘制椭圆白色背景框
            if (clrBase != WidgetTransparentColor
                    && pixBase.isNull())
            {
                // 非透明默认绘制白色框背景图
                QPixmap pmBG = THEME_GET_BMP((option->state & QStyle::State_HasFocus)
                                             ? PIC_DIRECTORY_EDIT_CONTACT_INFO_FOCUS : PIC_DIRECTORY_EDIT_CONTACT_INFO);


                if (!pmBG.isNull())
                {
                    QRect rect = panel->rect;
                    if (rect.width() != DFAULT_T48_EDIT_WITH || rect.height() != DFAULT_T48_EDIT_HEIGHT)
                    {
                        painter->drawPixmap(rect, pmBG.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio));
                    }
                    else
                    {
                        rect.setRect(rect.x(), rect.y() + 1, pmBG.width(), pmBG.height());
                        painter->drawPixmap(rect, pmBG);
                    }
                }
            }
            painter->restore();// 恢复保存的画笔
        }
        break;

    case PE_IndicatorCheckBox:
        if (const QStyleOptionButton * optBtn = qstyleoption_cast<const QStyleOptionButton *>(option))
        {
            QStyleOptionButton optNewBtn = *optBtn;
            optNewBtn.rect.setSize(QSize(28, 30));
            myDrawCheckBoxIndicator(element, &optNewBtn,  painter, widget);
        }
        break;

    case PE_IndicatorRadioButton:
        if (const QStyleOptionButton * optBtn = qstyleoption_cast<const QStyleOptionButton *>(option))
        {
            QStyleOptionButton optNewBtn = *optBtn;
            optNewBtn.rect.setSize(QSize(20, 20));
            int offsetY = (widget->height() - optNewBtn.rect.height()) >> 1;
            optNewBtn.rect.adjust(2, offsetY, 2, offsetY);
            myDrawRadioButtonIndicator(element, &optNewBtn, painter, widget);
        }
        break;
    case PE_PanelItemViewItem:
        // 不管是否失去焦点, 都使QTableWidget的行高亮, 不要变灰.
        if (const QStyleOptionViewItemV4 * vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option))
        {
            BaseClass::drawPrimitive(element, option, painter, widget);
        }
        break;

    // Remove all frame.
    case PE_Frame:
        break;

    // 避免QTableWidget出现一个灰色的选中框。
    case PE_FrameFocusRect:
        break;

    case PE_PanelMenu:
        myDrawPnaelMenu(element, option, painter, widget);
        break;

    default:
        BaseClass::drawPrimitive(element, option, painter, widget);
        break;
    }
}

void qT4XStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
                                   QPainter * painter, const QWidget * widget) const
{
    // 入口指针判空,此处widget可以为空不需要判空
    if (NULL == option || NULL == painter)
    {
        return;
    }

    switch (control)
    {
    // 自画QComboBox的边框, 背景, 以及下拉箭头.
    case CC_ComboBox:
        if (const QStyleOptionComboBox * comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option))
        {
            painter->save();// 保存当前画笔
            QStyleOptionComboBox optCpy = *comboBox;
            // 获得框和背景的颜色
            QColor clrBase = optCpy.palette.color(QPalette::Base);
            // 判断背景是否透明，如果为透明, 则不需要绘制椭圆白色背景框
            if (clrBase != WidgetTransparentColor)
            {
                QPixmap pmBG =  THEME_GET_BMP((option->state & QStyle::State_HasFocus)
                                              ? PIC_DIRECTORY_EDIT_CONTACT_INFO_FOCUS : PIC_DIRECTORY_EDIT_CONTACT_INFO);
                if (!pmBG.isNull())
                {
                    QRect rect = optCpy.rect;
                    // 调整高度与箭头对齐
                    painter->drawPixmap(rect, pmBG);
                }
            }
            myDrawComboBoxArrow(control, &optCpy, painter, widget);// 绘制ComBobox的箭头

            painter->restore();// 恢复保存的画笔
        }
        break;

    case CC_ScrollBar:
        if (const QStyleOptionSlider * optSlider = static_cast<const QStyleOptionSlider *>(option))
        {
            myDrawScrollBar(control, optSlider, painter, widget);
        }
        break;

    default:
        BaseClass::drawComplexControl(control, option, painter, widget);
    }
}

int qT4XStyle::pixelMetric(PixelMetric m, const QStyleOption * opt /* = 0 */,
                           const QWidget * widget /* = 0 */) const
{
    int ret = 0;
    switch (m)
    {
    case PM_IndicatorWidth:
        ret = 28;
        break;

    case PM_IndicatorHeight:
        ret = 30;
        break;

    case PM_ExclusiveIndicatorWidth:
        ret = 28;
        break;

    case PM_ExclusiveIndicatorHeight:
        ret = 28;
        break;

    case PM_ScrollBarExtent:
        ret = 48;
        break;

    case PM_TabBarScrollButtonWidth:
        ret = 1;
        break;

    case PM_MenuHMargin:
    case PM_MenuVMargin:
        ret = 0;
        break;

    case PM_MenuPanelWidth: //menu framewidth
        ret = 0;
        break;

    default:
        ret = BaseClass::pixelMetric(m, opt, widget);
        break;
    }

    return ret;
}

int qT4XStyle::styleHint(StyleHint sh, const QStyleOption * opt,
                         const QWidget * widget, QStyleHintReturn * hret) const
{
    int ret = 0;

    switch (sh)
    {
    // set this value to 0 to let QComboBox dropdown below or above QComboBox.
    case SH_ComboBox_Popup:
        ret = 0;
        break;

    default:
        ret = BaseClass::styleHint(sh, opt, widget, hret);
    }

    return ret;
}

QRect qT4XStyle::subElementRect(SubElement sr, const QStyleOption * opt,
                                const QWidget * widget) const
{
    QRect rc = BaseClass::subElementRect(sr, opt, widget);
    switch (sr)
    {
    default:
        break;
    }

    return rc;
}

QSize qT4XStyle::sizeFromContents(ContentsType type, const QStyleOption * option,
                                  const QSize & size, const QWidget * widget) const
{
    QSize sz;

    switch (type)
    {
    case CT_MenuItem:
        if (NULL != widget)
        {
            sz = QSize(widget->sizeHint().width(), 36);
        }
        break;
    default:
        {
            sz = BaseClass::sizeFromContents(type, option, size, widget);
        }
        break;
    }

    return sz;
}

QRect qT4XStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex * opt,
                                SubControl sc, const QWidget * widget) const
{
    QRect rc = BaseClass::subControlRect(cc, opt, sc, widget);
    switch (cc)
    {
    case CC_ComboBox:
        switch (sc)
        {
        // 组合框箭头向左偏移
        case SC_ComboBoxArrow:
            {
                if (NULL != widget)
                {
                    rc.setX(widget->rect().right() - knComboBoxImageWidth);
                }
                else
                {
                    rc.setX(rc.x() - knComboBoxImageWidth);
                }
                rc.setWidth(knComboBoxImageWidth);
                rc.setY(rc.y());
            }
            break;

        // 因为组合框箭头向左偏移了, 则组合框的编辑框的宽度也要相应减小
        case SC_ComboBoxEditField:
            {
                if (NULL != widget)
                {
                    rc.setWidth(widget->rect().width() - knComboBoxImageWidth - COMBOX_EDIT_ARROW_SPACE);
                }
                else
                {
                    rc.setWidth(rc.width() - knComboBoxImageWidth - COMBOX_EDIT_ARROW_SPACE);
                }
            }
            break;

        case SC_ComboBoxListBoxPopup:
            rc.setX(rc.x());
            rc.setWidth(rc.width());
            break;

        default:
            break;

        }
        break;
    case CC_ScrollBar:
        switch (sc)
        {
        case SC_ScrollBarSubLine:
            {
                rc.adjust(0, 0, 0, -23);
            }
            break;
        case SC_ScrollBarAddLine:
            {
                rc.adjust(0, 23, 0, 0);
            }
            break;
        case SC_ScrollBarSlider:
            {
                QRect rcAddLine = subControlRect(CC_ScrollBar, opt, SC_ScrollBarAddLine, widget);
                QRect rcSubLine = subControlRect(CC_ScrollBar, opt, SC_ScrollBarSubLine, widget);
                const QStyleOptionSlider * sliOpt = static_cast<const QStyleOptionSlider *>(opt);
                if (NULL != widget
                        && NULL != sliOpt)
                {
                    int iSliderH = rc.height();
                    int iSliderRang = widget->height()/* - rcAddLine.height() - rcSubLine.height()*/;
                    int iValueRang = sliOpt->maximum - sliOpt->minimum;

                    int x1 = rc.left() + 1;
                    int x2 = rc.right() - 1;
                    int y1 = 0;

                    //个数大于5
                    if (iValueRang > MaxScrollItemPerPage)
                    {
                        iSliderH = 500 / iValueRang;

                    }
                    //个数小于等于5
                    else
                    {
                        if (iValueRang != 0)
                        {
                            iSliderH = 100 / iValueRang;
                        }
                    }
                    iSliderRang -= iSliderH;
                    int iAdjY = static_cast<int>((iSliderRang - 8.0f) / iValueRang * sliOpt->sliderValue);
                    y1 +=  iAdjY;

                    rc.setCoords(x1, y1, x2 + 2, y1 + iSliderH);
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return rc;
}

void qT4XStyle::drawItemText(QPainter * painter, const QRect & rect,
                             int flags, const QPalette & pal, bool enabled,
                             const QString & text, QPalette::ColorRole textRole/* = QPalette::NoRole*/) const
{
    // 判断是否自动打断Text的显示
    bool isTextWordwrap = (flags & Qt::TextWordWrap) == Qt::TextWordWrap;
    const QString & strTran = text;
    QString strTemp = LANG_PARSE(text); //进行字符串的解析翻译及重组

#if TEST_TRAN_LEN
    int iWordMax = -1;
    QString strAfterTran = strTemp;
#endif

    RTL_GetRealString(strTemp);

    if (!isTextWordwrap)
    {
        strTemp = CutOffText(strTemp, painter->fontMetrics(), rect.width() - 3);
#if TEST_TRAN_LEN
        iWordMax = CountMaxCharInCtrl(painter->fontMetrics(), rect.width() - 3);
#endif
    }
#if TEST_TRAN_LEN

    if (strTran.contains(LANG_FLAG))
    {
        /*if (strTran.count(LANG_FLAG) == 1 && (strAfterTran.length() == strTran.length() - 4))
        {
            //整串翻译时map的key就是要翻译的字符串
            AddTranLenInfo(strAfterTran, painter->font().family(), iWordMax);
        }
        else*/
        {
            //是否为时间格式
            if (!IsIdleTimeFormat(strTran))
            {
                //获取Key
                QString strKey = "";
                QString strOther = "";
                //提取出保存到map的key
                GetRealTranText(strTran, strKey, strOther);
                strTran = strKey;
                if (iWordMax != -1)
                {
                    int iTranWordWith = rect.width() - 3 - painter->fontMetrics().width(strOther);
                    iWordMax = CountMaxCharInCtrl(painter->fontMetrics(), iTranWordWith);
                }

                //不是整串翻译时key可能带有翻译标记
                AddTranLenInfo(strTran, painter->font().family(), iWordMax);
            }
            else if (iTime_length == 0)
            {
                int iTranWordWith = rect.width() - 3 - painter->fontMetrics().width(time_other_string);
                iTime_length = CountMaxCharInCtrl(painter->fontMetrics(), iTranWordWith);
                AddTranLenInfo(time_note_string, painter->font().family(), iTime_length);
                //01 51 PM ?`03Fri ?`03Sep 06
            }
        }
    }

#endif

    // 掉用父类函数画Text
    BaseClass::drawItemText(painter, rect, flags, pal, enabled, strTemp, textRole);
}

bool qT4XStyle::myDrawPushButtonLabel(const QStyleOption * option,
                                      QPainter * painter, const QWidget * widget) const
{
    // 入口参数判断
    if (option == NULL)
    {
        return false;
    }

    qDrawShadePanel(painter, option->rect, option->palette, true);
    return true;
}

bool qT4XStyle::myDrawComboBoxArrow(ComplexControl control, const QStyleOptionComplex * option,
                                    QPainter * painter, const QWidget * widget) const
{
    QRect rc = subControlRect(CC_ComboBox, option, SC_ComboBoxArrow, widget);

    // 入口参数检查
    if (rc.isEmpty() || painter == NULL)
    {
        return false;
    }

    PixmapWeekPtr pm;

    if (State_On == (option->state & State_On))
    {
        bool bPopBelow = true;

        if (NULL != widget && widget->inherits("QComboBox"))
        {
            QComboBox * pCombobox = (QComboBox *)widget;
            QAbstractItemView * pView = pCombobox->view();

            if (NULL != pView && pView->isActiveWindow())
            {
                QPoint ptView = pView->mapToGlobal(QPoint(0, 0));
                QPoint ptComboBox = widget->mapToGlobal(QPoint(0, 0));

                if (ptComboBox.y() > ptView.y())
                {
                    bPopBelow = false;
                }
            }
        }

        if (bPopBelow)
        {
            pm = myPxCache::GetPxCache(PxComboBoxArrowDownFocus);
        }
        else
        {
            pm = myPxCache::GetPxCache(PxComboBoxArrowUpFocus);
        }
    }
    else
    {
        pm = myPxCache::GetPxCache(PxComboBoxArrowFocus);
    }

    if (!pm.isNull())// 判断获取图片是否成功
    {
        painter->drawPixmap(rc.x(), rc.y() - 1, *(pm.data()));
    }

    return true;
}

bool qT4XStyle::myDrawCheckBoxIndicator(PrimitiveElement element, const QStyleOption * option,
                                        QPainter * painter, const QWidget * widget /* = 0 */) const
{
    if (NULL == option || NULL == painter)
    {
        return false;
    }

    QRect rc = option->rect;
    if (rc.isEmpty())
    {
        return false;
    }

    // 重设控件的大小
    rc.setSize(QSize(28, 30));

    PixmapWeekPtr pm;
    if (option->state & (State_On))// 判断CheckBox是出于开启还是关闭状态
    {
        pm = myPxCache::GetPxCache(PxCheckBoxOn);
    }
    else
    {
        pm = myPxCache::GetPxCache(PxCheckBoxOff);
    }

    if (!pm.isNull())
    {
        // 绘制图片
        painter->drawPixmap(rc, *(pm.data()));
    }

    return true;
}

bool qT4XStyle::myDrawRadioButtonIndicator(PrimitiveElement element, const QStyleOption * option,
        QPainter * painter, const QWidget * widget /* = 0 */) const
{
    // 入口参数的正确性检查
    if (NULL == option || NULL == painter)
    {
        return false;
    }

    QRect rc = option->rect;
    if (rc.isEmpty())
    {
        return false;
    }

    PixmapWeekPtr pm;
    if (option->state & State_On)// 判断是开启还是关闭状态
    {
        pm = myPxCache::GetPxCache(PxRadioButtonOn);
    }
    else if (!(option->state & State_Enabled))
    {
        pm = myPxCache::GetPxCache(PxRadioButtonDisable);
    }
    else
    {
        pm = myPxCache::GetPxCache(PxRadioButtonOff);
    }

    if (!pm.isNull())
    {
        // 绘制图片
        painter->drawPixmap(rc, *(pm.data()));
    }

    return true;
}

// 绘制滚动条
bool qT4XStyle::myDrawScrollBar(ComplexControl control, const QStyleOptionComplex * option,
                                QPainter * painter, const QWidget * widget) const
{
    // 入口参数判断
    if (NULL == option
            || NULL == painter
            || NULL == widget)
    {
        return false;
    }


    const QStyleOptionSlider * optSlider = static_cast<const QStyleOptionSlider *>(option);

    if (optSlider == NULL)
    {
        return false;
    }

    painter->save();

    // 画滚动框的背景
    PixmapWeekPtr pmBackGround;
    if (optSlider->orientation & Qt::Horizontal)
    {
        pmBackGround = myPxCache::GetPxCache(PxScrollBarHorzBackGound);
    }
    else
    {
        pmBackGround = myPxCache::GetPxCache(PxScrollBarVertBackGound);
    }

    // 绘制背景框
    if (!pmBackGround.isNull())
    {
        painter->drawPixmap(option->rect.x() + 1, option->rect.y() + 4, 2, option->rect.height() - 8,
                            *(pmBackGround.data()));
    }

    bool ret = false;

    if (optSlider->subControls & SC_ScrollBarSlider)
    {
        ret &= myDrawScrollBarSlider(control, optSlider, painter, widget);;
    }


    painter->restore();

    return ret;
}

// 绘制滚动条向上或向左箭头
bool qT4XStyle::myDrawScrollBarSubLine(ComplexControl control, const QStyleOptionSlider * optSlider,
                                       QPainter * painter, const QWidget * widget) const
{
    // 入口参数判断
    if (NULL == optSlider || NULL == painter)
    {
        return false;
    }

    bool isHorizontal = (optSlider->orientation & Qt::Horizontal) ==
                        Qt::Horizontal;// 判断是横向的滚动条还是纵向的滚动条

    QRect rcSubLine = subControlRect(CC_ScrollBar, optSlider, SC_ScrollBarSubLine,
                                     widget);// 获得滑块的位置

    if (!rcSubLine.isNull())
    {
        // 获取相应的图片
        PixmapWeekPtr pmSubLine;
        pmSubLine = myPxCache::GetPxCache(isHorizontal ? PxScrollBarLeftArrow : PxScrollBarUpArrow);

        // 判断图片是否读取成功
        if (!pmSubLine.isNull())
        {
            // 绘制图片
            painter->drawPixmap(rcSubLine, *(pmSubLine.data()));
        }
    }

    return true;
}

// 绘制滚动条向下或向右箭头
bool qT4XStyle::myDrawScrollBarAddLine(ComplexControl control, const QStyleOptionSlider * optSlider,
                                       QPainter * painter, const QWidget * widget) const
{
    // 入口参数判断
    if (NULL == optSlider || NULL == painter)
    {
        return false;
    }

    bool isHorizontal = (optSlider->orientation & Qt::Horizontal) ==
                        Qt::Horizontal;// 判断是横向的滚动条还是纵向的滚动条

    // 画滚动条向上箭头
    QRect rcAddLine = subControlRect(CC_ScrollBar, optSlider, SC_ScrollBarAddLine,
                                     widget);// 获取向下箭头的区域
    if (!rcAddLine.isNull())
    {
        PixmapWeekPtr pmAddLine = myPxCache::GetPxCache(isHorizontal ? PxScrollBarRightArrow :
                                  PxScrollBarDownArrow);

        // 判断获取的图片是否有效
        if (!pmAddLine.isNull())
        {
            // 绘制图片
            painter->drawPixmap(rcAddLine, *(pmAddLine.data()));
        }
    }

    return true;
}

bool qT4XStyle::myDrawScrollBarSlider(ComplexControl control, const QStyleOptionSlider * optSlider,
                                      QPainter * painter, const QWidget * widget) const
{
    // 入口参数判断
    if (NULL == optSlider || NULL == painter)
    {
        return false;
    }

    bool isHorizontal = (optSlider->orientation & Qt::Horizontal) ==
                        Qt::Horizontal;// 判断是横向的滚动条还是纵向的滚动条

    // 画滚动条本身
    QRect rcSlider = subControlRect(CC_ScrollBar, optSlider, SC_ScrollBarSlider,
                                    widget);// 获得滚动条本身的位置
    if (!rcSlider.isEmpty())
    {
        PixmapWeekPtr pmSlider = myPxCache::GetPxCache(isHorizontal ? PxScrollBarHorzSlider :
                                 PxScrollBarVertSlider);
        PixmapWeekPtr pmSliderUpArrow = myPxCache::GetPxCache(PxScrollBarUpArrow);
        PixmapWeekPtr pmSliderDownArrow = myPxCache::GetPxCache(PxScrollBarDownArrow);

        // 判断图片是否正确
        if (!pmSlider.isNull())
        {
            // 绘制图片
            //painter->drawPixmap(rcSlider.x() - 1, rcSlider.y() , 4, 3, *(pmSliderUpArrow.data()));
            painter->drawPixmap(rcSlider.x() - 1, rcSlider.y() + 3, 4, rcSlider.height(),  *(pmSlider.data()));
            //painter->drawPixmap(rcSlider.x() - 1, rcSlider.y() + rcSlider.height() + 3, 4, 3, *(pmSliderDownArrow.data()));
        }
    }

    return true;
}


// 绘制菜单
bool qT4XStyle::myDrawPnaelMenu(PrimitiveElement element, const QStyleOption * option,
                                QPainter * painter, const QWidget * widget) const
{
    // 入口指针判空
    if (NULL == option || NULL == painter)
    {
        return false;
    }

    // 判断是否转换成功
    if (option == NULL)
    {
        return false;
    }

    painter->save();

    painter->setPen(Qt::NoPen);
    painter->setBackgroundMode(Qt::TransparentMode);

    PixmapWeekPtr pBkgPx = myPxCache::GetPxCache(PxPopMenuBackground);

    //绘制菜单背景
    if (!pBkgPx.isNull())
    {
        painter->drawPixmap(option->rect, *(pBkgPx.data()));
    }

    painter->restore();

    return true;
}

bool qT4XStyle::myDrawMenuItem(ControlElement element, const QStyleOption * option,
                               QPainter * painter, const QWidget * widget) const
{
    // 入口指针判空
    if (NULL == option
            || NULL == painter)
    {
        return false;
    }

    const QStyleOptionMenuItem * optMenu = static_cast<const QStyleOptionMenuItem *>(option);

    // 判断是否转换成功
    if (optMenu == NULL)
    {
        return false;
    }
    painter->save();

    QColor fontColor;

    QRect rc = optMenu->rect;
    rc.adjust(1, 1, -1, -1);

    PixmapWeekPtr pPix;
    //enabled
    if (optMenu->state & State_Enabled)
    {
        fontColor = Qt::white;

        if (optMenu->state & State_Selected)
        {
            //鼠标放上状态
            pPix = myPxCache::GetPxCache(PxPoPMenuSeletcedItem);
        }
    }

    painter->setBackgroundMode(Qt::TransparentMode);

    //画选中项指示背景
    if (!pPix.isNull())
    {
        painter->drawPixmap(rc, *(pPix.data()));
    }

    if (QStyleOptionMenuItem::Separator == optMenu->menuItemType)
    {
        //画分隔符
        int sepH = 2;
        QRect sepRec(rc.x(), rc.y() + ((rc.height() - sepH) >> 1), rc.width(), sepH);
        painter->fillRect(sepRec, Qt::black);
    }
    else
    {
        //写菜单文字
        QPalette plat = widget->palette();
        plat.setColor(QPalette::WindowText, Qt::black);
        rc.adjust(4, 0, 0, 0);
        drawItemText(painter, rc, Qt::AlignJustify | Qt::TextHideMnemonic,
                     plat, (optMenu->state & State_Enabled), optMenu->text, QPalette::WindowText);
    }

    painter->restore();

    return true;
}

QColor qT4XStyle::myGetStateColor(const QStyle::State st) const
{
    QColor ret_clr;

    // 判断是否获得焦点
    if (st & State_HasFocus)
    {
        // 设置获得焦点为白色
        ret_clr = Qt::white;
    }
    else
    {
        // 判断是否激活
        if (st & State_Enabled)
        {
            ret_clr = QColor(183, 205, 230);
        }
        else
        {
            ret_clr = QColor(183, 205, 230);  // no distance between enable and disable.
        }
    }

    return ret_clr;
}

