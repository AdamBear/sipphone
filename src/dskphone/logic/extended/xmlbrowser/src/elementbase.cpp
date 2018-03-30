#include "logic_inc.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementBase::CElementBase()
{
    m_pParseData = NULL;
    m_nElementID = (int)this;
}

CElementBase::~CElementBase()
{
    ReleaseData();
}

// 判断并处理合法的URL
void CElementBase::ProcessLegalURL(yl::string & strURL)
{
    if (strncmp(strURL.c_str(), "http", 4) == 0
            || strncmp(strURL.c_str(), "https", 5) == 0)
    {
        _URIProcesser.ProcessURI(strURL);
    }
    else
    {
        XMLBROWSER_WARN("error! not http(s) uri.");
        _XMLBrowserManager.ShowMessageBox(XB_TIP_INVAILID_URI, 3000);
    }
}
#endif // #ifdef IF_FEATURE_XMLBROWSER

