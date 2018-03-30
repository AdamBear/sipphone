#include <QtGui>
#include "directorycommon.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include <resource/modresource.h>
#include "inputmethod/inputmethod.h"


#define CONTACT_RES_TYPE_IMAGE "contact"
typedef YLSmartPtr<BaseResItem> CResDataPtr;
typedef YLList<CResDataPtr> LIST_RESOURCE;


bool Res_GetResType(int nResType, yl::string & strResType)
{
    if (DIR_RES_TYPE_RING == nResType)
    {
        strResType = RING_TYPE;

        return true;
    }
    else if (DIR_RES_TYPE_PHOTO == nResType)
    {
        strResType = CONTACT_RES_TYPE_IMAGE;

        return true;
    }

    return false;
}

bool Res_GetNameByPath(int nResType, const yl::string & strPath, yl::string & strName)
{
    yl::string strResType;

    if (!Res_GetResType(nResType, strResType))
    {
        return false;
    }

    LIST_RESOURCE listRes;

    if (0 != res_getResItemsByResType(listRes, strResType.c_str()))
    {
        return false;
    }

    LIST_RESOURCE::iterator iter = listRes.begin();

    for (; iter != listRes.end(); iter++)
    {
        CResDataPtr & pItem = *iter;

        if (pItem.IsEmpty())
        {
            continue;
        }

        if (strPath == pItem->resPath)
        {
            strName = pItem->resItemName;
            return true;
        }
    }

    return false;
}

bool ParseFileName(const yl::string & strFilePath, yl::string & strFile)
{
    yl::string::size_type pos = strFilePath.find_last_of("/");

    if (yl::string::npos == pos)
    {
        return false;
    }

    strFile = strFilePath.substr(pos + 1);

    return true;
}

bool ParseFilePath(const yl::string & strFilePath, yl::string & strPath, yl::string & strFile)
{
    yl::string::size_type pos = strFilePath.find_last_of("/");

    if (yl::string::npos == pos)
    {
        return false;
    }

    strPath = strFilePath.substr(0, pos + 1);
    strFile = strFilePath.substr(pos + 1);

    return true;
}

bool IsBsftAllowSipAuth()
{
    return (1 == configParser_GetConfigInt(kszAllowSipAuth));
}

void DirUI_SetNameIME(QLineEdit * pNameEdit)
{
    if (NULL == pNameEdit)
    {
        return;
    }

    yl::string strDefaultIME(configParser_GetConfigString(kszContactDefaultIME));

    bool bHaveCustomIme = Ime_GetCustomImeCount() > 0;

    if (strDefaultIME.empty())
    {
        strDefaultIME = bHaveCustomIme ? IME_DIY : IME_Abc;
    }

    if (bHaveCustomIme)
    {
        qInstallIME(pNameEdit, IME_CUSTOM);
    }
    else
    {
        qInstallIME(pNameEdit, IME_Name);
    }

    qSetCurrentIME(pNameEdit, strDefaultIME.c_str());
}

bool DirUI_AdjustFrameList(CFrameList* m_pFrameList, bool bReset, QRect rcDefault, int nOffset, int nTop)
{
    if (m_pFrameList == NULL)
    {
        return false;
    }

    if (bReset)
    {
        m_pFrameList->SetRect(rcDefault);
        return true;
    }

    const QWidget* pWgt = GetCurrInputWgt();
    if (pWgt != NULL)
    {
        if (pWgt->inherits("QLineEdit")
                || pWgt->inherits("QTextEdit")
           )
        {
            QPoint ptInput = pWgt->mapToGlobal(pWgt->pos());
            QRect rcFlist = m_pFrameList->geometry();

            // 焦点向下移动判断是否遮挡
            if (ptInput.y() + pWgt->height() + nOffset > g_iFullKeyBoardY)
            {
                rcFlist.setY(rcFlist.y() - ptInput.y() - pWgt->height() - nOffset + g_iFullKeyBoardY);
                rcFlist.setHeight(m_pFrameList->height());
                m_pFrameList->SetRect(rcFlist);
            }
            // 焦点向上移动判断是否被界面边缘覆盖
            else if (ptInput.y() - nOffset < nTop)
            {
                rcFlist.setY(rcFlist.y() + nTop - ptInput.y() + nOffset);
                rcFlist.setHeight(m_pFrameList->height());
                m_pFrameList->SetRect(rcFlist);
            }
        }
        return true;
    }
    return false;
}
