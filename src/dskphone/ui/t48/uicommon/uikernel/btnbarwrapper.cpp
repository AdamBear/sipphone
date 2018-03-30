#include "btnbarwrapper.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

#include <QtGui>

CBtnBarWrapper::CBtnBarWrapper(QWidget * pWidget, bool bAutoDelBtn)
    : m_pWidget(pWidget)
    , m_pArrBtn(NULL)
    , m_nBtnCount(0)
    , m_bAutoDelBtn(bAutoDelBtn)
{
}

CBtnBarWrapper::~CBtnBarWrapper(void)
{
    ResetBtnArray();
}

void CBtnBarWrapper::ResetBtnArray()
{
    if (NULL != m_pArrBtn)
    {
        if (m_bAutoDelBtn)
        {
            for (int i = 0; i < m_nBtnCount; ++i)
            {
                CButtonWrapper * pBtnWrapper = *(m_pArrBtn + i);

                if (NULL != pBtnWrapper)
                {
                    delete pBtnWrapper;
                }

                *(m_pArrBtn + i) = NULL;
            }
        }

        delete []m_pArrBtn;
        m_pArrBtn = NULL;
    }

    m_nBtnCount = 0;
}

void CBtnBarWrapper::SetBtnCount(int nCount)
{
    ResetBtnArray();

    if (nCount <= 0)
    {
        return;
    }

    m_nBtnCount = nCount;
    m_pArrBtn = new CButtonWrapper*[nCount];

    for (int i = 0; i < nCount; ++i)
    {
        *(m_pArrBtn + i) = NULL;
    }
}

bool CBtnBarWrapper::SetBtn(int nIndex, CButtonWrapper * pBtn)
{
    if (nIndex < 0 || nIndex >= m_nBtnCount || NULL == pBtn)
    {
        return false;
    }

    *(m_pArrBtn + nIndex) = pBtn;

    return true;
}

int CBtnBarWrapper::GetBtnCount() const
{
    return m_nBtnCount;
}

CButtonWrapper * CBtnBarWrapper::GetBtn(int nIndex) const
{
    if (nIndex < 0 || nIndex >= m_nBtnCount)
    {
        return NULL;
    }

    return *(m_pArrBtn + nIndex);
}

CButtonWrapper * CBtnBarWrapper::GetDownBtn() const
{
    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL != pBtn && pBtn->IsDown())
        {
            return pBtn;
        }
    }

    return NULL;
}

int CBtnBarWrapper::GetDownBtnIndex() const
{
    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL != pBtn && pBtn->IsDown())
        {
            return i;
        }
    }

    return -1;
}

void CBtnBarWrapper::ResetAllBtnDown(CButtonWrapper * pExcludeBtn)
{
    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL != pBtn && pBtn != pExcludeBtn)
        {
            pBtn->ResetDown();
        }
    }
}

CButtonWrapper * CBtnBarWrapper::ProcessMousePress(QMouseEvent * pEvent)
{
    if (m_nBtnCount <= 0 || NULL == pEvent)
    {
        return NULL;
    }

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL == pBtn)
        {
            break;
        }

        bool bDown = pBtn->IsDown();

        if (pBtn->ProcessMousePress(pEvent))
        {
            ResetAllBtnDown(pBtn);

            return pBtn;
        }
        else if (bDown)
        {
            pBtn->ResetDown();
        }
    }

    return NULL;
}

CButtonWrapper * CBtnBarWrapper::ProcessMouseRelease(QMouseEvent * pEvent)
{
    if (m_nBtnCount <= 0 || NULL == pEvent)
    {
        return NULL;
    }

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL == pBtn)
        {
            break;
        }

        if (pBtn->ProcessMouseRelease(pEvent))
        {
            ResetAllBtnDown(NULL);
            return pBtn;
        }
    }

    return NULL;
}

CButtonWrapper * CBtnBarWrapper::ProcessKeyPress(QKeyEvent * pEvent)
{
    if (m_nBtnCount <= 0 || NULL == pEvent)
    {
        return NULL;
    }

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL == pBtn)
        {
            break;
        }

        bool bDown = pBtn->IsDown();

        if (pBtn->ProcessKeyPress(pEvent))
        {
            ResetAllBtnDown(pBtn);

            return pBtn;
        }
        else if (bDown)
        {
            pBtn->ResetDown();
        }
    }

    return NULL;
}

CButtonWrapper * CBtnBarWrapper::ProcessKeyRelease(QKeyEvent * pEvent)
{
    if (m_nBtnCount <= 0 || NULL == pEvent)
    {
        return NULL;
    }

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL == pBtn)
        {
            break;
        }

        if (pBtn->ProcessKeyRelease(pEvent))
        {
            ResetAllBtnDown(NULL);
            return pBtn;
        }
    }

    return NULL;
}

#ifdef IF_ENABLE_TESTABILITY
QString CBtnBarWrapper::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CButtonWrapper * pBtn = *(m_pArrBtn + i);

        if (NULL == pBtn)
        {
            continue;;
        }
        QString strBtnId = QString("btn%1").arg(i);
        AddBtnNode(nodeRoot, *pBtn, fromQString(strBtnId));
    }

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif