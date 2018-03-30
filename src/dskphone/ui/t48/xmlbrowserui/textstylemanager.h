#ifndef _TEXT_STYLE_MANAGER_
#define _TEXT_STYLE_MANAGER_

#include <QtGui>

#define MAX_NUM_ITEM    8       //每页最大Item条数

// 根据QLabel的长度对text进行截断
// 将超出的部分替换为...
QString replaceLabelText(const QString & strText, QLabel * pLabel);

// 根据长度对text进行截断
// 将超出的部分替换为...
QString replaceTextByWidth(const QString & strText, int iWidth,
                           const QFontMetrics & objFontMetrics);

// 截断Text,将超出iWidth的部分去掉
QString cutTextForWidth(const QString & strText, const QFontMetrics & objFontMetrics,
                        int iWidth, bool bCutWord = false);

//给定一页的固定高度，给定多个项的高度
//返回需要多少页，每页存放几项-vecPageItemNum
int GetSumPage(int nFixHeight, std::vector<int> vecItemHeight, std::vector<int> & vecPageItemNum);

#endif
