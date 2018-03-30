#ifndef __X_PREFERENCE_LAYOUT_H__
#define __X_PREFERENCE_LAYOUT_H__

#include "ETLLib/ETLLib.hpp"
#include "xWindow.h"
#include "xValueHandler.h"
#include "xGraphic.h"
#include <yllist.h>

#if IF_PREFERENCE_LAYOUT
class xPreferenceLayout;
class XWINDOW_EXPORT PreferenceParams : public chRegKey
{
    friend class xPreferenceLayout;
private:
    void bindView(xView* pView);
    bool loadValue();
    bool saveValue();
    bool valueChanged();
public:
    PreferenceParams();
public:
    yl::string      m_strDefaultValue;
    xView*          m_pValueView;
    xValueHandler*  m_pValueHandler;
};

typedef YLList<PreferenceParams>                typePreferenceParamsList;

class XWINDOW_EXPORT xPreferenceLayout : public xLinearLayout, xMessageHandler
{
public:
    void reloadValue();
    void saveValue();
    bool valueChanged();
protected:
    virtual void loadAttributesForContentView(xView* pChild, xml_node& node);

    bool loadRegistryKey(const yl::string& str, chRegKey& key, yl::string& strDefault);

private:
    void slotValueChanged(xSignal* /*sender*/, WPARAM /*wParam*/, LPARAM /*lParam */, int /*nDataBytes*/, LPVOID /*pData*/);
public:
    xPreferenceLayout();
public:
    xSignal                                     signalValueChanged;
    typePreferenceParamsList                        m_listPreference;
};

#endif

#endif // __X_PREFERENCE_LAYOUT_H__
