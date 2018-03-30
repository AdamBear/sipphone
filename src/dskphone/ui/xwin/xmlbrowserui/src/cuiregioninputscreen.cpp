#include "cuiregioninputscreen.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "imeedit/ximeedit.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/twolineeditlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"
#include "devicelib/networkserver.h"
#include "xmlbrowsermessagebox.h"

#if XML_BROWSER_SUPPORT_UI

static yl::string sStrEmpty;

namespace
{
#define XML_BROWSER_INPUTSCREEN_NORMAL_ITEM_HEIGHT          36
#define XML_BROWSER_INPUTSCREEN_CONDENSE_ITEM_HEIGHT        18
#define XML_BROWSER_INPUTSCREEN_NORMAL_ITEM_COUNT_EACH_PAGE         2
#define XML_BROWSER_INPUTSCREEN_CONDENSE_ITEM_COUNT_EACH_PAGE       4
#define XML_BROWSER_INPUTSCREEN_INCORRECT_FORMAT_SHOW_TIME  2000
}

////////////////////////////////////////////////////////////////////////////////
CUIRegionInputScreen::CUIRegionInputScreen()
    : CUIRegionBaseListScreen()
    , m_iPreviousFocusIndex(0)
    , m_bLineFold(false)
{
    m_vecItemData.clear();
}

CUIRegionInputScreen::~CUIRegionInputScreen()
{

}

void CUIRegionInputScreen::Update(void * pInputData)
{
    if (NULL == pInputData)
    {
        return;
    }

    XBInputData * pNewData = static_cast<XBInputData *>(pInputData);

    if (NULL == m_pTxtTitle
            || NULL == m_pListView || NULL == pNewData)
    {
        return;
    }

    m_stBaseData.m_strTitle = pNewData->stAttr.strTitle;
    m_stBaseData.m_bIsWrapTitle = pNewData->stAttr.bWrapTitle;
    m_stBaseData.m_nDefaultIndex = pNewData->stAttr.iDefaultIndex;
    LoadInputItemData(pNewData);
    m_iQuitTime = pNewData->stAttr.iTimeOut;
    m_bLock = pNewData->stAttr.bLockIn;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
#endif
    UpdateSoftkeyData();
}

const VecSoftkeyParam & CUIRegionInputScreen::GetFocusSoftkey()
{
    int nIndex = GetCurrentItemIndex();
    return GetSoftkeyByIndex(nIndex);
}

const VecSoftkeyParam & CUIRegionInputScreen::GetSoftkeyByIndex(int nIndex)
{
    if (CheckDataExist(nIndex))
    {
        return m_vecItemData[nIndex].m_vecSoftkey;
    }
    return sVecSoftParam;
}

void CUIRegionInputScreen::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    xEditTextView * pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }

    softkeyParam.clear();
    const InputItemData & stInpuData = GetFocusInputData();

    switch (stInpuData.eInputType)
    {
    case INPUT_IP:
        {
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_DOT));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        }
        break;
    case INPUT_NUMBER:
        {
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        }
        break;
    case INPUT_STRING:
        {
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
            SoftkeyParam stSoftPara;
            stSoftPara.eType = XB_ST_CHANGEMODE;
            stSoftPara.strName = pEditView->GetImeName();
            stSoftPara.strURI = "";
            softkeyParam.push_back(stSoftPara);
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_DOT));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_NEXTSPACE));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        }
        break;
    case INPUT_TIME_US:
    case INPUT_TIME_INT:
    case INPUT_DATE_US:
    case INPUT_DATE_INT:
        {
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
            SoftkeyParam stSoftPara;
            stSoftPara.eType = XB_ST_CHANGEMODE;
            stSoftPara.strName = pEditView->GetImeName();
            stSoftPara.strURI = "";
            softkeyParam.push_back(stSoftPara);
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        }
        break;
    default:
        {
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
            softkeyParam.push_back(TypeToSoftInfo(XB_ST_SELECT));
        }
        break;
    }
}

yl::string CUIRegionInputScreen::GetDefaultImeByType(INPUT_TYPE eInputType)
{
    yl::string strDefaultIME = "";
    if (INPUT_STRING == eInputType)
    {
        strDefaultIME = configParser_GetConfigString(kszXmlInputStringDefaultIME);
    }
    return strDefaultIME;
}

yl::string CUIRegionInputScreen::GetImeByType(INPUT_TYPE eInputType)
{
    switch (eInputType)
    {
    case INPUT_NUMBER:
    case INPUT_IP:
        {
            return IME_DIGIT;
        }
        break;
    case INPUT_STRING:
        {
            ;
        }
        break;
    case INPUT_TIME_US:
    case INPUT_TIME_INT:
    case INPUT_DATE_US:
    case INPUT_DATE_INT:
        {
            return IME_ALL;
        }
        break;
    default:
        break;
    }
    return IME_ALL;
}

bool CUIRegionInputScreen::LoadInputItemData(XBInputData * pInputData)
{
    if (NULL == pInputData)
    {
        return false;
    }

    List_Input_Field& refListItem = pInputData->listItem;
    // 是否分行
    m_bLineFold = (pInputData->stAttr.eDisplayMode != MODE_CONDENSED);

    m_vecItemData.clear();
    //解析数据
    List_Input_Field::iterator itItem = refListItem.begin();
    InputItemData itemData;
    for (; itItem != refListItem.end(); ++itItem)
    {
        InputField* pItem = (*itItem);
        if (NULL == pItem)
        {
            continue;
        }

        itemData.bPassword = pItem->bPassword;
        itemData.bEditable = pItem->bEditable;
        itemData.eInputType = pItem->eType;
        itemData.strPrompt = pItem->strPrompt;
        itemData.strContent = pItem->strDefault;
        itemData.strIme = GetImeByType(pItem->eType);
        itemData.strDefaultIme =  GetDefaultImeByType(pItem->eType);
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        itemData.m_vecSoftkey = pItem->vecSoftkey;
#endif
        m_vecItemData.push_back(itemData);
    }

    return true;
}

void CUIRegionInputScreen::InitListViewLayout(xListView * pListView)
{
    CUIRegionBaseListScreen::InitListViewLayout(pListView);
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vecItemData.size());
}

xListViewItem * CUIRegionInputScreen::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CEditListItemPtr pEditItem = NULL;

    if (m_bLineFold)
    {
        CTwoLineEditListItemPtr pTwoLineEditItem = CTwoLineEditListItem::GetTwoLineEditListItem(pItem);

        if (NULL == pTwoLineEditItem)
        {
            pTwoLineEditItem = static_cast<CTwoLineEditListItemPtr>(g_ItemFactory.ApplyItem(CTwoLineEditListItem::GetTwoLineEditListItemType()));
        }
        pEditItem = pTwoLineEditItem;
    }
    else
    {
        pEditItem = CEditListItem::GetEditListItem(pItem);

        if (NULL == pEditItem)
        {
            pEditItem = static_cast<CEditListItemPtr>(g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType()));
        }
    }

    if (NULL == pEditItem)
    {
        return NULL;
    }

    if (CheckDataExist(nDataIndex))
    {
        const InputItemData& stMenuData = m_vecItemData[nDataIndex];
        pEditItem->SetEditEnable(stMenuData.bEditable);
        pEditItem->SetSuddenPassword(true);
        pEditItem->SetPassword(stMenuData.bPassword);
        pEditItem->SetShowColon(false);
        pEditItem->InstallIME(stMenuData.strIme.c_str(), stMenuData.strDefaultIme.c_str());
        xEditTextView* pEditView = pEditItem->GetEditText();
        if (NULL != pEditView)
        {
            pEditView->SetDeleteSoftkeyIndex(-1);
            pEditView->SetImeSoftkeyIndex(-1);
        }
    }

    return pEditItem;
}

bool CUIRegionInputScreen::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CEditListItemPtr pEditItem = NULL;

    if (m_bLineFold)
    {
        CTwoLineEditListItemPtr pTwoLineEditItem = CTwoLineEditListItem::GetTwoLineEditListItem(pItem);
        pEditItem = pTwoLineEditItem;
    }
    else
    {
        pEditItem = CEditListItem::GetEditListItem(pItem);
    }

    if (NULL == pEditItem)
    {
        return false;
    }

    pEditItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();

    if (CheckDataExist(nDataIndex))
    {
        const InputItemData & stMenuData = m_vecItemData[nDataIndex];
        pEditItem->SetTitle(stMenuData.strPrompt);
        pEditItem->SetValue(stMenuData.strContent);
    }

    return true;
}

void CUIRegionInputScreen::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        CUIRegionBaseListScreen::OnItemAction(pListView, pItem, nAction);
        UpdateSoftkeyData();
        m_signalAction.emitSignal(REGION_ACTION_CURRENT_ITEM_CHANGE, 0, 0, NULL);
        int nDataIndex = m_pListView->GetFocusItemDataIndex();
        m_iPreviousFocusIndex = nDataIndex;
    }
}

void CUIRegionInputScreen::UpdateImeSoftkeyInfo(VecSoftkeyParam& softkeyParam)
{
    xEditTextView* pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }
    for (int i = 0; i < softkeyParam.size(); ++i)
    {
        SoftkeyParam& softParam = softkeyParam[i];
        if (XB_ST_CHANGEMODE == softParam.eType)
        {
            // 设置IME显示名
            softParam.strName  = pEditView->GetImeName();
        }
    }
}

void CUIRegionInputScreen::UpdateSoftkeyData()
{
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    const VecSoftkeyParam & vecPrivateSoftkey = GetSoftkeyByIndex(GetCurrentItemIndex());
    if (vecPrivateSoftkey.size() > 0)
    {
        SetSoftkey(vecPrivateSoftkey);
    }
    else
#endif
    {
        UpdateImeSoftkeyInfo(m_vecGlobalSoftkey);
        SetSoftkey(m_vecGlobalSoftkey);
    }
}

bool CUIRegionInputScreen::GetPageData(MapPageData & PageData)
{
    int nDataIndex = GetCurrentItemIndex();
    SetItemDataValueByDataIndex(nDataIndex, GetItemValueByDataIndex(nDataIndex));
    PageData.clear();
    for (int i = 0; i < m_vecItemData.size(); ++i)
    {
        const InputItemData& stInputData = m_vecItemData[i];
        if (!IsValidInput(stInputData.eInputType, stInputData.strContent))
        {
            return false;
        }
        PageData.insert(i, stInputData.strContent);
    }
    return true;
}

CUIRegionInputScreen * CUIRegionInputScreen::GetInputScreenUIRegion(CUIRegionBase * pItem)
{
    if (NULL == pItem || XMT_INPUT != pItem->GetElementType())
    {
        return NULL;
    }

    return static_cast<CUIRegionInputScreen *>(pItem);
}

void CUIRegionInputScreen::OnClickSoftKeyBackSpace()
{
    const InputItemData & stInpuData = GetFocusInputData();
    if (!stInpuData.bEditable)
    {
        return;
    }

    xEditTextView * pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }
    pEditView->DeleteChar();
}

void CUIRegionInputScreen::OnClickSoftKeyDot()
{
    xEditTextView * pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }
    pEditView->InsertChar('.');
}

void CUIRegionInputScreen::OnClickSoftKeyNextSpace()
{
    xEditTextView * pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }
    pEditView->InsertChar(' ');
}

void CUIRegionInputScreen::OnClickSoftKeyChangeIME()
{
    const InputItemData & stInpuData = GetFocusInputData();
    if (!stInpuData.bEditable)
    {
        return;
    }

    xEditTextView * pEditView = GetFocusEditTextView();
    if (NULL == pEditView)
    {
        return;
    }
    pEditView->SwitchIme();
    UpdateSoftkeyData();
    m_signalAction.emitSignal(REGION_ACTION_CURRENT_ITEM_CHANGE, 0, 0, NULL);
}

xEditTextView * CUIRegionInputScreen::GetFocusEditTextView()
{
    return GetEditTextViewByDataIndex(GetCurrentItemIndex());
}

const InputItemData & CUIRegionInputScreen::GetFocusInputData()
{
    int nSelect = GetCurrentItemIndex();
    if (CheckDataExist(nSelect))
    {
        return m_vecItemData[nSelect];
    }
    static InputItemData sInputData;
    return sInputData;
}

xEditTextView * CUIRegionInputScreen::GetEditTextViewByDataIndex(int nIndex)
{
    if (NULL == m_pListView)
    {
        return NULL;
    }

    xListViewItem* pListItem = m_pListView->GetItemByDataIndex(nIndex);
    CEditListItemPtr pEditItem = NULL;
    if (m_bLineFold)
    {
        CTwoLineEditListItemPtr pTwoLineEditItem = CTwoLineEditListItem::GetTwoLineEditListItem(pListItem);
        pEditItem = pTwoLineEditItem;
    }
    else
    {
        pEditItem = CEditListItem::GetEditListItem(pListItem);
    }

    if (NULL == pEditItem)
    {
        return NULL;
    }

    xEditTextView * pEditView = pEditItem->GetEditText();
    return pEditView;
}

const yl::string & CUIRegionInputScreen::GetItemValueByDataIndex(int nDataIndex)
{
    xEditTextView * pEditText = GetEditTextViewByDataIndex(nDataIndex);
    if (NULL == pEditText)
    {
        return sStrEmpty;
    }
    return pEditText->GetText();
}

void CUIRegionInputScreen::SetItemValueByDataIndex(int nDataIndex, const yl::string & strValue)
{
    xEditTextView * pEditText = GetEditTextViewByDataIndex(nDataIndex);
    if (NULL == pEditText)
    {
        return;
    }
    return pEditText->SetText(strValue);
}

void CUIRegionInputScreen::SetItemDataValueByDataIndex(int nDataIndex, const yl::string & strValue)
{
    if (!CheckDataExist(nDataIndex))
    {
        return;
    }
    m_vecItemData[nDataIndex].strContent = strValue;
}

const yl::string & CUIRegionInputScreen::GetItemDataValueByDataIndex(int nDataIndex)
{
    if (!CheckDataExist(nDataIndex))
    {
        return sStrEmpty;
    }
    return m_vecItemData[nDataIndex].strContent;
}

bool CUIRegionInputScreen::CheckDataExist(int nDataIndex)
{
    if (nDataIndex >= 0 && nDataIndex < m_vecItemData.size())
    {
        return true;
    }
    return false;
}

bool CUIRegionInputScreen::OnKeyPress(int nKey)
{
    int nFocusDataIndex = (NULL != m_pListView) ? m_pListView->GetFocusItemDataIndex() : -1;

    // 上下切换焦点、点击OK时，先保存当前项数据到内存中
    if (PHONE_KEY_OK == nKey || PHONE_KEY_UP == nKey || PHONE_KEY_DOWN == nKey)
    {
        SetItemDataValueByDataIndex(nFocusDataIndex, GetItemValueByDataIndex(nFocusDataIndex));
    }
    // 锁定键盘,则屏蔽其他按键
    if (PHONE_KEY_OK == nKey)
    {
        if (m_bLock && !IsIgnoredLockKey(PHONE_KEY_OK))
        {
            return true;
        }
        else
        {
            MapPageData objMap;
            bool bLegalData = GetPageData(objMap);
            if (bLegalData)
            {
                //submit和按ok键confirm处理相同
                xmlbrowser_OnAction(GetElementID(), XB_ACTION_CONFIRM, (void*)&objMap);
            }
            return true;
        }
    }
    return CUIRegionBase::OnKeyPress(nKey);

}

bool CUIRegionInputScreen::IsValidInput(INPUT_TYPE eType, yl::string strValue)
{
    if (INPUT_IP == eType)
    {
        if (!strValue.empty()
                && !netIsValidIP(strValue.c_str()))
        {
            _XBMessageManager.ShowMessage(XB_TIP_INVALID_IP, XML_BROWSER_INPUTSCREEN_INCORRECT_FORMAT_SHOW_TIME);
            return false;
        }
    }
    else if (INPUT_DATE_INT == eType)
    {
        // dateInt  DD/MM/YYYY 其中 MM:1-12,DD:1-31,YYYY:0000-9999    31/01/2010
        int iMon = 0, iDate = 0, iYear = 0;
        if (!strValue.empty()
                && 3 == sscanf(strValue.c_str(), "%d/%d/%d", &iDate, &iMon, &iYear)
                && IsDateValid(iYear, iMon, iDate))
        {
            return true;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_DATE_INT, XML_BROWSER_INPUTSCREEN_INCORRECT_FORMAT_SHOW_TIME);
        return false;
    }
    else if (INPUT_DATE_US == eType)
    {
        // dateUS   MM/DD/YYYY 其中MM:1-12,DD:1-31,YYYY:0000-9999 12/31/2009
        int iMon = 0, iDate = 0, iYear = 0;
        if (!strValue.empty()
                && 3 == sscanf(strValue.c_str(), "%d/%d/%d", &iMon, &iDate, &iYear)
                && IsDateValid(iYear, iMon, iDate))
        {
            return true;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_DATE_US, XML_BROWSER_INPUTSCREEN_INCORRECT_FORMAT_SHOW_TIME);
        return false;
    }
    else if (INPUT_TIME_INT == eType)
    {
        // timeInt  HH:MM:SS   其中HH:0-23, MM:0-59, SS:0-59  23:25:00
        int iHour = 0, iMinute = 0, iSecond = 0;
        if (!strValue.empty()
                && 3 == sscanf(strValue.c_str(), "%d:%d:%d", &iHour, &iMinute, &iSecond)
                && iHour >= 0 && iHour <= 23
                && iMinute >= 0 && iMinute <= 59
                && iSecond >= 0 && iSecond <= 59)
        {
            return true;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_TIME_INT, XML_BROWSER_INPUTSCREEN_INCORRECT_FORMAT_SHOW_TIME);
        return false;
    }
    else if (INPUT_TIME_US == eType)
    {
        // timeUS  HH:MM:SS AM/PM 其中HH:1-12, MM:0-59, SS:0-59
        int iHour = 0, iMinute = 0, iSecond = 0;
        char szBuf[128] = "";
        if (!strValue.empty()
                && 4 == sscanf(strValue.c_str(), "%d:%d:%d %s", &iHour, &iMinute, &iSecond, szBuf)
                && iHour >= 1 && iHour <= 12
                && iMinute >= 0 && iMinute <= 59
                && iSecond >= 0 && iSecond <= 59
                && (0 == strcmp(szBuf, "AM") || 0 == strcmp(szBuf, "PM")))
        {
            return true;
        }
        _XBMessageManager.ShowMessage(XB_TIP_INVALID_TIME_US, 2000);
        return false;
    }
    return true;
}

bool CUIRegionInputScreen::IsDateValid(int iYear, int iMon, int iDay)
{
    int iLeapYear = 0;
    int arrMonData[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (iYear < 0
            || iYear > 10000)
    {
        return false;
    }

    if ((0 == iYear % 4 && 0 != iYear % 100)
            || 0 == iYear % 400)
    {
        iLeapYear = 1;
    }

    if (iMon < 1
            || iMon > 12)
    {
        return false;
    }
    // 闰年2月多1天
    arrMonData[1] += iLeapYear;

    if (iDay < 1
            || iDay > arrMonData[iMon - 1])
    {
        return false;
    }

    return true;
}

int CUIRegionInputScreen::GetItemHeight()
{
    int nItemHeight = 0;
    if (m_bLineFold)
    {
        nItemHeight = XML_BROWSER_INPUTSCREEN_NORMAL_ITEM_HEIGHT;
    }
    else
    {
        nItemHeight = XML_BROWSER_INPUTSCREEN_CONDENSE_ITEM_HEIGHT;
    }
    return nItemHeight;
}

#endif
