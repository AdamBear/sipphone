#include "pingandtraceoutresultdelegate.h"
#include "diagnostics/include/moddiagnostics.h"
#include "uicallbackfun.h"
#include "keymap.h"

namespace
{
const yl::wstring HIDE_CHARS = L" \t";
const yl::wstring CUT_CHARS = HIDE_CHARS + L"-,.;)}]!?>";
#define ITEM_PING_RESULT_WIDTH 244
}

CPingAndTraceOutResultDelegate::CPingAndTraceOutResultDelegate()
{

}

CPingAndTraceOutResultDelegate::~CPingAndTraceOutResultDelegate()
{

}

CSettingUIDelegateBase * CPingAndTraceOutResultDelegate::CreatResultDelegate()
{
    CPingAndTraceOutResultDelegate * pDelegate = new CPingAndTraceOutResultDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CPingAndTraceOutResultDelegate::InitExtraData(void * pData)
{
    yl::string strResult = Diagnostics_GetDiagnositcsResult();
    m_vecText.clear();
    GetShowText(strResult, m_vecText);
}

void CPingAndTraceOutResultDelegate::GetShowText(const yl::string & strText, VEC_TEXT & vecText)
{
    if (strText.empty())
    {
        return;
    }

    vecText.clear();
    xFont font;

    yl::wstring wstr = yl::to_utf16(strText);
    typedef YLVector<yl::wstring> LINES;
    LINES vecLines = yl::string_split<yl::wstring::value_type>(wstr, L"\n", true);

    int iRow = 0;
    int iFirstCharPos = 0;
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring & str = *begin;
        while (!str.empty())
        {
            if (ITEM_PING_RESULT_WIDTH < font.textBound(yl::to_utf8(str)).cx)
            {
                yl::wstring::size_type upper_pos = str.size();
                yl::wstring::size_type lower_pos = 0;
                yl::wstring::size_type mid_pos = (upper_pos + lower_pos) >> 1;
                yl::string paintText;
                int nLoop = 0;
                //二分查找合适位置
                while (lower_pos != mid_pos)
                {
                    DEAD_LOOP_BREAK(nLoop);

                    paintText = yl::to_utf8(str.substr(0, mid_pos));
                    int nTextSize = font.textBound(paintText).cx;

                    if (nTextSize <= ITEM_PING_RESULT_WIDTH)
                    {
                        lower_pos = mid_pos;
                    }
                    else
                    {
                        upper_pos = mid_pos;
                    }

                    mid_pos = (upper_pos + lower_pos) >> 1;
                }

                //分词换行
                yl::wstring strLine(str, 0, lower_pos);

                //一个字符都显示不下
                if (strLine.empty())
                {
                    break;
                }

                //如果结尾的字符不能换行
                if (CUT_CHARS.find_first_of(strLine.back()) == yl::wstring::npos)
                {
                    yl::wstring::size_type cutPos = strLine.find_last_of(CUT_CHARS);
                    if (cutPos != yl::wstring::npos)
                    {
                        strLine.resize(cutPos + 1);
                    }
                }

                vecText.push_back(yl::to_utf8(strLine));
                str.erase(0, strLine.size());
            }
            else
            {
                vecText.push_back(yl::to_utf8(str));
                break;
            }
        }
    }
}
