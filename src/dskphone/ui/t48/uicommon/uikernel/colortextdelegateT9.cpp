#include "colortextdelegateT9.h"
#include "qtcommon/qmisc.h"
#include <ylstring.h>
#include "baselibs/include/commonapi/stringhelperapi.h"

CColorTextDelegateT9::CColorTextDelegateT9()
{
}

CColorTextDelegateT9::~CColorTextDelegateT9()
{
}

int CColorTextDelegateT9::SearchIndex(const QString & strTotalText) const
{
    int nIndexResult = -1;

    //正确的转换yl::string strText = fromQString(QString::fromLocal8Bit(strTotalText.toUtf8().data()));
    //错误转换，但可以搜索到汉字，可出现错误汉字高亮（逻辑层同搜索方法）
    yl::string strText = strTotalText.toUtf8().data();

    bool bIsDigital = commonAPI_IsDigital(fromQString(GetHighLightString()));

    if (bIsDigital)
    {
        QString strDigital = toQString(commonAPI_transT9String(strText));
        nIndexResult = strDigital.indexOf(GetHighLightString(), 0, Qt::CaseInsensitive);
    }
    else
    {
        nIndexResult = strTotalText.indexOf(GetHighLightString(), 0, Qt::CaseInsensitive);
    }

    if (GetIsFirstMatch() && 0 != nIndexResult)
    {
        return -1;
    }

    return nIndexResult;
}
