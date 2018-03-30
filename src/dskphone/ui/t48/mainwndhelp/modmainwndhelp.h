#ifndef _MOD_MAIN_WND_HELP_H_
#define _MOD_MAIN_WND_HELP_H_

#define EDK_MESSAGEBOX_ID_BASE 140
typedef bool(*ActionCallBack)(bool, int, yl::string &);

struct structEDKCallBack
{
    int nID;
    QString strInput;
    ActionCallBack pCallBackFun;
};
Q_DECLARE_METATYPE(structEDKCallBack)

void MainWndHelp_Init();

void MainWndHelp_Uinit();

void MainWndHelp_EDK_Show_MessageBox(int iIndex, int nID, ActionCallBack pFun);

#endif // _MOD_MAIN_WND_DELEGATE_H_
