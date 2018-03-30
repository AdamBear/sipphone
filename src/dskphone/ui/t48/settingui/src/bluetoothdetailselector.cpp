#include "bluetoothdetailselector.h"
#include "uikernel/qcheckbox.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "baseui/framelist.h"
#include "messagebox/modmessagebox.h"
#include "popupboxitem.h"

#include "dsklog/log.h"

namespace
{
#ifdef _T49
#define SELECT_POP_BOX_POS_ITEM_HEIGHT          74      // Item Height
// #define RADIO_FONT_SIZE                         23      // RadioButton Font Size
// #define RADIO_WIDTH                             120
// #define CHECKBOX_WIDTH                          250
#else
#define SELECT_POP_BOX_POS_ITEM_HEIGHT          48      // Item Height
// #define RADIO_FONT_SIZE                         23      // RadioButton Font Size
// #define RADIO_WIDTH                             120
// #define CHECKBOX_WIDTH                          250
#endif
}

BluetoothDetailSelector::BluetoothDetailSelector()
    : m_nSelIndex(0)
    , m_pMobileChkBox(NULL)
    , m_nTotCnt(-1)
    , m_eType(BLUETOOTH_DETAIL_POPUP_DLG_COMMON)
{
    m_pMobileChkBox = new qCheckBox(NULL);
    if (NULL == m_pMobileChkBox)
    {
        return;
    }

    //m_pMobileChkBox->SetRadioWidth(RADIO_WIDTH);
    //m_pMobileChkBox->SetFontSize(RADIO_FONT_SIZE);
    //m_pMobileChkBox->setFixedWidth(CHECKBOX_WIDTH);
}

BluetoothDetailSelector::~BluetoothDetailSelector()
{
    if (NULL != m_pMobileChkBox)
    {
        m_pMobileChkBox->deleteLater();
        m_pMobileChkBox = NULL;
    }
}

void BluetoothDetailSelector::SetTotalCount(int iTotCnt)
{
    m_nTotCnt = iTotCnt;
}

void BluetoothDetailSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex,
        int nDataCount)
{
    if (NULL == pFrameList || NULL == m_pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    m_pFrameList->SetItemClickBG("");
    m_pFrameList->SetItemSelectBG("");

    int nItemIndex = 0;
    int iMaxVec = m_vecWgtPair.size();
    int iItemNum = m_pFrameList->GetTotalItemCount();

    SETTINGUI_INFO("[BluetoothDetailSelector::OnLoadData] tot[%d]", iMaxVec);

    m_pFrameList->SetTotalDataCount(iMaxVec);

    for (int i = 0; i < iMaxVec; ++i)
    {
        bool bAdd = false;
        PopupBoxItem * pItem = NULL;
        if (i < iItemNum)
        {
            pItem = (PopupBoxItem *)m_pFrameList->GetItemByIndex(nItemIndex);
        }

        if (NULL == pItem)
        {
            bAdd = true;
            pItem = new PopupBoxItem;

            if (NULL == pItem)
            {
                SETTINGUI_INFO("itemIdx[%d] is NULL item", nItemIndex);
                continue;
            }

            pItem->SetAction(this);
            pItem->SetDataIndex(nItemIndex);
            pItem->setGeometry(0, 0, 0, SELECT_POP_BOX_POS_ITEM_HEIGHT);
        }

        //init
        pItem->SetLeftDisplay("");
        pItem->SetWidget(NULL);

        pItem->SetLeftDisplay(m_vecWgtPair[i].first);
        if (NULL != m_vecWgtPair[i].second)
        {
            pItem->SetWidget(m_vecWgtPair[i].second);
            m_vecWgtPair[i].second->setParent(pItem);
        }

        if (bAdd && !m_pFrameList->AddItem(pItem))
        {
            pItem->Release();
        }

        ++nItemIndex;
    }

    m_pFrameList->update();
}

void BluetoothDetailSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    if (NULL == pFrameList || NULL == m_pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
    // deal with bluetooth headphones
    if (BLUETOOTH_DETAIL_POPUP_DLG_COMMON == m_eType)
    {
        if (0 == m_nSelIndex)
        {
            if (LIST_ITEM_ACTION_SELECT != nAction)
            {
                return;
            }
            MessageBox_RemoveTopMessageBox(MessageBox_Delete);
        }
        return;
    }

    // deal with mobile bluetooth
    if (0 == m_nSelIndex && BLUETOOTH_DETAIL_POPUP_DLG_MOBILE == m_eType)
    {
        if (LIST_ITEM_ACTION_BLUETOOTH_MOBILE_SYNC == nAction)
        {
            MessageBox_RemoveTopMessageBox(m_pMobileChkBox->GetData() ? MessageBox_MobileSyncOn :
                                           MessageBox_MobileSyncOff);
        }
        return;
    }

    if (LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    if (1 == m_nSelIndex)
    {
        MessageBox_RemoveTopMessageBox(MessageBox_Delete);
    }

}

void BluetoothDetailSelector::ResetCheckBox()
{
    m_pMobileChkBox->setParent(NULL);
}

void BluetoothDetailSelector::LoadDataByType(BLUETOOTH_DETAIL_POPUP_DLG_TYPE eType)
{
    m_eType = eType;
    if (//NULL == m_pChkBox ||
        NULL == m_pMobileChkBox)
    {
        return;
    }

    m_vecWgtPair.clear();
    QWidgetPair qPair;
    QString strLab;


    if (BLUETOOTH_DETAIL_POPUP_DLG_MOBILE == m_eType)
    {
        //m_pMobileChkBox->setObjectName(MOBILE_SYNC_CHECKBOX_OBJECT_NAME);
        strLab = LANG_TRANSLATE(TRID_MOBILE_CONTACTS_SYNC);
        qPair.first = strLab;
        qPair.second = m_pMobileChkBox;
        m_vecWgtPair.push_back(qPair);
    }



    strLab = LANG_TRANSLATE(TRID_DELETE);
    qPair.first = strLab;
    qPair.second = NULL;
    m_vecWgtPair.push_back(qPair);
}

void BluetoothDetailSelector::SetMobileSync(bool bMobileSync)
{
    if (NULL != m_pMobileChkBox)
    {
        m_pMobileChkBox->SetData(bMobileSync);
    }
}

