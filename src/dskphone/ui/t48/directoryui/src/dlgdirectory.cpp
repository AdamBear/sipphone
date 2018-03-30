#include "baseui/framelist.h"
#include "baseui/treelist.h"
#include "baseui/treelistdelegate.h"
#include "baseui/t4xpicpath.h"

#include "dlgdirectory.h"

#include "directorycommon.h"
#include "browserdelegate.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "groupnormaltitle.h"
#include "keymap.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/rectdefine.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "imagemanager/modimagemanager.h"
#include "callmanager/modcallmanager.h"

namespace
{
#define DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE 16
}

CDlgDirectory::CDlgDirectory(QWidget * parent/* = NULL*/)
    : CBaseDialog(parent)
    , m_pFrameList(NULL)
    , m_pTreeList(NULL)
{
    SetWindowType(eWT_Directory);
    m_dlgRect = RECT_DIRECTORY;
    SetWindowTitle(LANG_TRANSLATE(TRID_DIRECTORY));

    m_pTreeList = new CTreeList(this);

    if (NULL != m_pTreeList)
    {
        CGroupNormalTitlePtr pTitle = new CGroupNormalTitle;

        m_pTreeList->SetFrameListTitle(pTitle);

        m_pTreeList->SetRect(RECT_DIR_GROUP);
        m_pTreeList->SetGridLine(true);
        m_pTreeList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetItemSpace(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetLineImage(PIC_GENERAL_LIST_ITEM_LEFT_LINE);
        m_pTreeList->SetItemSelectBG(NULL, PIC_GENERAL_TREE_ITEM_SELECT,
                                     PIC_GENERAL_TREE_ITEM_BOTTOM_SELECT);
        m_pTreeList->SetItemClickBG(NULL, PIC_GENERAL_TREE_ITEM_CLICK, PIC_GENERAL_TREE_ITEM_BOTTOM_CLICK);
        m_pTreeList->show();
        m_pTreeList->raise();
    }

    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetRect(RECT_DIR_CONTACT);
        m_pFrameList->SetContentPos(CONTACT_LIST_ITEM_CONTENT_LEFT_MARGIN,
                                    CONTACT_LIST_ITEM_CONTENT_RIGHT_MARGIN);
        m_pFrameList->SetGridLine(true);
        m_pFrameList->SetLinePosY(DEFAULT_RIGHT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineHeight(DEFAULT_RIGHT_LIST_ITEM_SPACE);
        m_pFrameList->SetItemSpace(DEFAULT_RIGHT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineImage(PIC_GENERAL_LIST_ITEM_RIGHT_LINE);
        m_pFrameList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                     NULL);
        m_pFrameList->SetNote(LANG_TRANSLATE(TRID_NONE));
        m_pFrameList->SetNoteFontSize(DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE);
        m_pFrameList->show();
        m_pFrameList->raise();
    }
}

CDlgDirectory::~CDlgDirectory()
{
}

void CDlgDirectory::Init()
{
}

void CDlgDirectory::Uninit()
{
    if (NULL != m_pTreeList)
    {
        m_pTreeList->ClearContent();
        m_pTreeList->SetDelegate(NULL);
    }

    if (NULL != m_pFrameList)
    {
        m_pFrameList->ClearContent();
        m_pFrameList->SetDelegate(NULL);
    }

#if DIR_SUPPORT_SELECT_CONTACT
    g_DirectoryMgr.CancelSelectDirectory();
#endif
}

void CDlgDirectory::UpdateWnd()
{
    g_DirectoryMgr.OnUIShow(DIR_UI_STATE_BROWSER, m_pTreeList, m_pFrameList);
}

void CDlgDirectory::OnFocusChanged(bool bFocused)
{
    if (!bFocused)
    {
        g_DirectoryMgr.OnUIHide(m_pTreeList, m_pFrameList);
    }
}

CTreeList * CDlgDirectory::GetTreeList() const
{
    return m_pTreeList;
}

CFrameList * CDlgDirectory::GetFrameList() const
{
    return m_pFrameList;
}

QPixmap CDlgDirectory::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_CALLMANAGER_DIR_BACKGROUND);
}

bool CDlgDirectory::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    if (MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    if (NULL != m_pFrameList && m_pFrameList->eventFilter(pObj, pEvent))
    {
        return true;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress://键盘输入事件
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_CANCEL:
                {
                    callmanager_PutPage(CALLMGR_PAGE_NAME_DIRECTORY);
                    return true;
                }
                break;
            default:
                {
                    if (PHONE_KEY_STAR == nKey || (nKey >= PHONE_KEY_0 && nKey <= PHONE_KEY_9))
                    {
                        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

                        if (!pControler.IsEmpty())
                        {
                            int nContactType = pControler->GetType();

                            if (DIRECTORY_TYPE_NONE != nContactType)
                            {
                                g_DirectoryMgr.EnterSearch(nContactType, pControler->GetGroupId(), nKey);
                            }
                        }
                    }
                }
                break;
            }
        }
        break;
    default:
        break;
    }

    return CBaseDialog::eventFilter(pObj, pEvent);
}

bool CDlgDirectory::OnAction(const QString & strAction)
{
    bool bProcess = true;

    if (strAction == ACTION_BACK)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
        CBrowserDelegate * pBrowserDelegate = CBrowserDelegate::GetDirDelegate(pDelegate);

        if (NULL != pBrowserDelegate && pBrowserDelegate->OnBack())
        {
            return true;
        }

        callmanager_PutPage(CALLMGR_PAGE_NAME_DIRECTORY);

        return true;
    }
    else
    {
        bProcess = false;
    }

    return bProcess;
}
