#include "dlgusbbrowsestorage.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "commonapi/stringhelperapi.h"
#include "uikernel/languagehelper.h"
#include "record/include/modrecord.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

CDlgUSBBrowseStorage::CDlgUSBBrowseStorage(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabTotal(NULL)
    , m_pLabUsed(NULL)
    , m_pLabFree(NULL)
    , m_pLabAvailableTime(NULL)
{
    InitPageData();
}

CDlgUSBBrowseStorage::~CDlgUSBBrowseStorage()
{

}

//界面初始化
void CDlgUSBBrowseStorage::InitPageData()
{
    if (!modRecord_IsStorageConnect())
    {
        return;
    }

    m_pLabTotal = new QLabel(this);
    m_pLabUsed = new QLabel(this);
    m_pLabFree = new QLabel(this);
    m_pLabAvailableTime = new QLabel(this);

    if (m_pLabTotal == NULL || m_pLabUsed == NULL || m_pLabFree == NULL
            || m_pLabAvailableTime == NULL)
    {
        return;
    }

    //设置文字颜色
    SetWgtFontColor(m_pLabTotal, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabUsed, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabFree, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabAvailableTime, FONT_TYPE_TEXT);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_USB_PROPERTY_TOTAL);
    qPair.first = strLab;
    qPair.second = m_pLabTotal;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_USB_PROPERTY_USED);
    qPair.first = strLab;
    qPair.second = m_pLabUsed;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_USB_PROPERTY_FREE);
    qPair.first = strLab;
    qPair.second = m_pLabFree;
    m_vecWidgetPair.push_back(qPair);

    if (0 != configParser_GetConfigInt(kszUSBRecordEnable))
    {
        strLab = LANG_TRANSLATE(TRID_USB_PROPERTY_AVAILABLE_AUDIO_RECORD);
        qPair.first = strLab;
        qPair.second = m_pLabAvailableTime;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pLabAvailableTime->hide();
    }

    AddWidget(m_vecWidgetPair);
}

bool CDlgUSBBrowseStorage::LoadPageData()
{
    if (m_vecWidgetPair.isEmpty())
    {
        QString strNote = LANG_TRANSLATE(TRID_USB_PLEASE_INSERT_USBDRIVE);
        //显示None
        m_pGeneralFrame->SetNote(strNote);

        return true;
    }

    StorageInfo strginfo;
    modRecord_GetStorageInfo(strginfo);

    yl::string strTotal = modRecord_TransSizeToString(strginfo.m_ullTotal);

    yl::string strUsed = modRecord_TransSizeToString(strginfo.m_ullUsed);
    strUsed = commonAPI_FormatString("%s(%.1f%% %s)", strUsed.c_str(),
                                     (float)strginfo.m_ullUsed * 100 / strginfo.m_ullTotal,
                                     LANG_TRANSLATE(TRID_USB_PROPERTY_USED_HINT).toUtf8().data());

    yl::string strFree = modRecord_TransSizeToString(strginfo.m_ullFree);
    strFree = commonAPI_FormatString("%s(%.1f%% %s)", strFree.c_str(),
                                     (float)strginfo.m_ullFree * 100 / strginfo.m_ullTotal,
                                     LANG_TRANSLATE(TRID_USB_PROPERTY_FREE_HINT).toUtf8().data());

    yl::string strTime = modRecord_GetAvailableRecordingTime();

    m_pLabTotal->setText(toQString(strTotal));
    m_pLabUsed->setText(toQString(strUsed));
    m_pLabFree->setText(toQString(strFree));
    m_pLabAvailableTime->setText(toQString(strTime));

    return true;
}

bool CDlgUSBBrowseStorage::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgUSBBrowseStorage::IsStatusChanged()
{
    return false;
}

bool CDlgUSBBrowseStorage::SavePageData()
{
    return true;
}
