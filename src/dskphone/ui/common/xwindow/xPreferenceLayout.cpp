#include "xPreferenceLayout.h"
#include "modviewdump.h"


#if IF_PREFERENCE_LAYOUT

#include "keymap.h"
#include "commonapi/stringhelperapi.h"

#include "xWindowManager.h"
#include "xControls.h"

EXPORT_VIEW_CLASS(xPreferenceLayout);

//////////////////////////////////////////////////////////////////////////
PreferenceParams::PreferenceParams()
{
    m_pValueView = NULL;
    m_pValueHandler = NULL;
}

void PreferenceParams::bindView(xView* pView)
{
    m_pValueView = pView;
    m_pValueHandler = m_pValueView->findHandler<xValueHandler>();
}

bool PreferenceParams::loadValue()
{
    if (m_pValueHandler != NULL)
    {
        return m_pValueHandler->setValue(chRegKey::GetTextValue(m_strDefaultValue.c_str()).c_str());
    }
    else
    {
        m_pValueView->showValue(chRegKey::GetTextValue(m_strDefaultValue.c_str()).c_str());
    }
    return true;
}

bool PreferenceParams::saveValue()
{
    if (m_pValueHandler != NULL && chRegKey::SetTextValue(m_pValueHandler->getValue().c_str()))
    {
        chRegKey::FlushKey();
        return true;
    }
    return false;
}

bool PreferenceParams::valueChanged()
{
    return m_pValueHandler != NULL && chRegKey::GetTextValue(m_strDefaultValue.c_str()) != m_pValueHandler->getValue().c_str();
}

//////////////////////////////////////////////////////////////////////////
xPreferenceLayout::xPreferenceLayout()
{
    REGISTER_VIEW_DUMP_CLASS(xPreferenceLayout)

    orientation = orientation_vertical;
}

void xPreferenceLayout::reloadValue()
{
    typePreferenceParamsList::iterator it = m_listPreference.begin();

    for (; it != m_listPreference.end(); ++it)
    {
        PreferenceParams& objParams = *it;

        objParams.loadValue();
    }
}

void xPreferenceLayout::saveValue()
{
    typePreferenceParamsList::iterator it = m_listPreference.begin();

    for (; it != m_listPreference.end(); ++it)
    {
        PreferenceParams& objParams = *it;

        objParams.saveValue();
    }
}

bool xPreferenceLayout::valueChanged()
{
    typePreferenceParamsList::iterator it = m_listPreference.begin();

    for (; it != m_listPreference.end(); ++it)
    {
        PreferenceParams& objParams = *it;

        if (objParams.valueChanged())
        {
            return true;
        }
    }
    return false;
}

void xPreferenceLayout::loadAttributesForContentView(xView* pChild, xml_node& node)
{
    chASSERT(pChild != NULL && xWorkStation::containChildView(this, pChild));
    yl::string preference;

    PopupAndroidString(node, XWINDOW_NAME_PREFERENCE, preference);

    if (!preference.empty())
    {
        PreferenceParams objParams;

        if (loadRegistryKey(preference, objParams, objParams.m_strDefaultValue))
        {
            objParams.bindView(pChild);
            objParams.loadValue();
            chASSERT(objParams.m_pValueHandler != NULL);
            SLOT_CONNECT(objParams.m_pValueHandler, signalValueChanged, this, xPreferenceLayout::slotValueChanged);

            m_listPreference.push_back(objParams);
        }
    }
}

// system@camera:bExpose
// user@camera:bExpose
// /config/user/voip/sipAccount.cfg@camera:bExpose
bool xPreferenceLayout::loadRegistryKey(const yl::string& str, chRegKey& objKey, yl::string& strDefault)
{
    if (str.empty())
    {
        return false;
    }

    LPCSTR lpszPreference = str.c_str();

    //file
    chConstString file;
    etlPeekPhraseByCharToken(lpszPreference, file, '@');
    chWARNING(*lpszPreference != '\0');

    //section
    chConstString section;
    etlPeekPhraseByCharToken(lpszPreference, section, ':');
    chWARNING(*lpszPreference != '\0');

    chConstString key;
    etlPeekPhraseByCharToken(lpszPreference, key, '=');
    chWARNING(*lpszPreference != '\0');

    static chConstString strUser = "user";
    static chConstString strSystem = "system";

    if (file == strUser)
    {
        file = CONFIG_USER_INI_FILE_PATH;
    }
    else if (file == strSystem)
    {
        file = CONFIG_SYSTEM_INI_FILE_PATH;
    }
    strDefault = lpszPreference;
    return objKey.OpenKey(file, section, key) != FALSE;
}

void xPreferenceLayout::slotValueChanged(xSignal* /*sender*/, WPARAM /*wParam*/, LPARAM /*lParam */, int /*nDataBytes*/, LPVOID /*pData*/)
{
    signalValueChanged.emitSignal(valueChanged(), 0, 0, 0);
}

#endif
