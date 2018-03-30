#include "textstylemanager.h"

// 根据QLabel的长度对text进行截断
// 将超出的部分替换为...
QString replaceLabelText(const QString & strText, QLabel * pLabel)
{
    if (NULL == pLabel || strText.isEmpty())
    {
        return strText;
    }

    // 判断是否自动打断Text的显示
    if (!pLabel->wordWrap())
    {
        return replaceTextByWidth(strText, pLabel->width(), pLabel->fontMetrics());
    }

    return strText;
}

// 根据长度对text进行截断
// 将超出的部分替换为...
QString replaceTextByWidth(const QString & strText, int iWidth, const QFontMetrics & objFontMetrics)
{
    int iTextWidth = objFontMetrics.width(strText);

    // 如下情况返回原字符
    // 字符串为空
    // 显示宽度大于文字长度
    if (strText.isEmpty()
            || iWidth >= iTextWidth)
    {
        return strText;
    }

    // 显示宽度小于末端符号返回原字符
    QString strEndSymbol("...");
    iWidth -= objFontMetrics.width(strEndSymbol);
    if (iWidth <= 0)
    {
        return strText;
    }

    // 二分法确定截断位置的范围
    int iMinIndex = 0;
    int iMaxIndex = strText.size() - 1;
    int iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    int iInterval = 2;

    // 循环确定截断的范围,范围大小为0~2*iInterval
    while (iMaxIndex > iMidIndex + iInterval)
    {
        if (iWidth >= objFontMetrics.width(strText.left(iMidIndex)))
        {
            iMinIndex = iMidIndex;
        }
        else
        {
            iMaxIndex = iMidIndex - 1;
        }

        iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    }

    // 逆向遍历找出第一个符合的位置
    while (iMinIndex <= iMaxIndex)
    {
        if (iWidth > objFontMetrics.width(strText.left(iMaxIndex)))
        {
            break;
        }

        --iMaxIndex;
    }

    return strText.left(iMaxIndex) + strEndSymbol;
}

// 截断Text,将超出iWidth的部分去掉
QString cutTextForWidth(const QString & strText, const QFontMetrics & objFontMetrics,
                        int iWidth, bool bCutWord/* = false*/)
{
    if (strText.isEmpty() || iWidth < 1)
    {
        return strText;
    }

    QString strTemp = strText;
    int nPixelsWide = objFontMetrics.width(strTemp);// 获得Text的宽度
    if (nPixelsWide > iWidth)
    {
        int nLen = strTemp.length();// 获得字符串长度
        strTemp = strTemp.remove(--nLen, 1);
        while (objFontMetrics.width(strTemp) > iWidth && nLen > 0)
        {
            strTemp = strTemp.remove(--nLen,
                                     1); //减一个字符，包括汉字（中文时减一个字符就减一个汉字）
        }

        // 判断是否截断单词
        // 若不截断则要还原或者删除部分单词
        if (!bCutWord)
        {
            // 查找字符串中是否有空格
            int iIndex = strTemp.lastIndexOf(" ");

            if (iIndex != -1)
            {
                // 有空格的话则将后面的内容去除
                strTemp = strTemp.remove(iIndex, nLen - iIndex);
            }
            else
            {
                // 没有空格,说明整行为一个单词的一部分,需要补齐单词
                int iSecWordIndex = strText.indexOf(" ");
                if (iSecWordIndex == -1)
                {
                    strTemp = strText;
                }
                else
                {
                    strTemp = strText.left(iSecWordIndex);
                }
            }
        }
    }

    return strTemp;
}

int GetSumPage(int nFixHeight, std::vector<int> vecItemHeight, std::vector<int> & vecPageItemNum)
{
    if (vecItemHeight.size() > 0)
    {
        int nTempCount = 0;
        int nPreIndex = 0;

        std::vector<int>::size_type qx = 0;
        for (; qx != vecItemHeight.size(); ++ qx)
        {
            nTempCount += vecItemHeight[qx];
            if (nTempCount < nFixHeight)
            {

            }
            else if (nTempCount == nFixHeight)
            {
                //分页，当前项分到下一页
                vecPageItemNum.push_back(qx - nPreIndex + 1);

                nPreIndex = qx + 1;

                //分页，置0
                nTempCount = 0;
//
//              nTempCount = vecItemHeight[qx];
            }
            else
            {
                //加到第qx个时超出范围，分页，上一项也分到下一页
                vecPageItemNum.push_back(qx - nPreIndex);

                //标记分页
                nPreIndex = qx;

                //下一页开始，重新计算，当前项计入下一页
                nTempCount = vecItemHeight[qx];
            }
        }
        //统计最后一页
        if (nPreIndex <= qx - 1)
        {
            //最后一页结束时没有排满
            vecPageItemNum.push_back(qx - nPreIndex);
        }

        //此时vecPageItemNum即为各页Item个数，其大小为总页数
        return vecPageItemNum.size();
    }
    return 0;
}

