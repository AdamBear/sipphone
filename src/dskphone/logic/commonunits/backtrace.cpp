#include "backtrace.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "dsklog/log.h"
#include "modcommonunits.h"

#define MAX_BACKTRACE_NUM 20

#ifdef __cplusplus
extern "C"
{
#endif

static void MyBackTrace(unsigned long * fb, int i)
{
    if (i == 0)
    {
        return;
    }
    unsigned long * pc = (unsigned long *)(*fb);


    APP_ERR("Stack [%d],the func name 0x%3x", i, pc);

    unsigned long * prefb = (unsigned long *)(*(fb - 1));

    //printf("the prefbt 0x%3x\n",prefb);

    if (prefb == NULL)
    {
        return;
    }

    i--;

    MyBackTrace(prefb, i);
}


void PrintMaps()
{
    char szMemFile[100] = "";
    char buf[256] = "";
    sprintf(szMemFile, "/proc/%d/maps", getpid());

    FILE * pf = fopen(szMemFile, "rb");
    if (pf != NULL)
    {
        APP_ERR("mapsfile_begin===============%s===============", szMemFile);
        while (fgets(buf, 256, pf) != NULL)
        {
            APP_ERR("Lib-In:%s", buf);
            memset(buf, 0, 256);
        }
        APP_ERR("mapsfile_end===============%s===============", szMemFile);
        fclose(pf);
    }
}

#ifndef _WIN32
static void sig_handler(int signo, siginfo_t * info, void * ptr)
{
    if (info == NULL || ptr == NULL)
    {
        return;
    }

    PrintMaps();

    APP_ERR("~~~~~~~~~~~~~~~~~~~~Fault Trace begin~~~~~~~~~~~~~~~~~~~~");

    APP_ERR("info.si_signo = %d", signo);
    APP_ERR("info.si_errno = %d", info->si_errno);
    APP_ERR("info.si_code  = %d", info->si_code);
    APP_ERR("info.si_addr  = %p", info->si_addr);
    APP_ERR("sender pid = %d, sender uid = %d", info->si_pid, info->si_uid);

    ucontext_t * ucontext = (ucontext_t *)ptr;
    // for arm
    APP_ERR("the arm_fp 0x%3x", ucontext->uc_mcontext.arm_fp);
    APP_ERR("the arm_ip 0x%3x", ucontext->uc_mcontext.arm_ip);
    APP_ERR("the arm_sp 0x%3x", ucontext->uc_mcontext.arm_sp);
    APP_ERR("the arm_lr 0x%3x", ucontext->uc_mcontext.arm_lr);
    APP_ERR("the arm_pc 0x%3x", ucontext->uc_mcontext.arm_pc);
    APP_ERR("the arm_cpsr 0x%3x", ucontext->uc_mcontext.arm_cpsr);
    APP_ERR("the falut_address 0x%3x", ucontext->uc_mcontext.fault_address);

    APP_ERR("the function dump stack:");

    unsigned long * bp = (unsigned long *)ucontext->uc_mcontext.arm_fp;

    int k = MAX_BACKTRACE_NUM;

    MyBackTrace(bp, k);

    // 通知各模块dump info
    //msgSendMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), INNER_MSG_CATCH_SIG_TO_EXIT, signo, 0);

    APP_ERR("~~~~~~~~~~~~~~~~~~~~Fault Trace end~~~~~~~~~~~~~~~~~~~~");

    //防止退不出
    _exit(0);
}
#endif
/*
struct sigcontext {
    unsigned long trap_no;
    unsigned long error_code;
    unsigned long oldmask;
    unsigned long arm_r0;
    unsigned long arm_r1;
    unsigned long arm_r2;
    unsigned long arm_r3;
    unsigned long arm_r4;
    unsigned long arm_r5;
    unsigned long arm_r6;
    unsigned long arm_r7;
    unsigned long arm_r8;
    unsigned long arm_r9;
    unsigned long arm_r10;
    unsigned long arm_fp;
    unsigned long arm_ip;
    unsigned long arm_sp;
    unsigned long arm_lr;
    unsigned long arm_pc;
    unsigned long arm_cpsr;
    unsigned long fault_address;
};*/

void OnSingle(int iSingle)
{
#ifndef _WIN32
    switch (iSingle)
    {
    case SIGHUP:
    {
        APP_ERR("Catch Single: SIGQUIT\n");
        commonUnits_System("echo Catch Single > " TEMP_PATH "log.txt");
    }
    break;
    default:
    {
        return;
    }
    break;
    }
#endif
}

#ifndef _WIN32
void catch_sig()
{
#define SET_SIGACTION(sig, action) \
    do \
    {\
    if(sigaction(sig, &action, NULL) < 0)\
    perror("sigaction:");\
} while(0);

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = sig_handler;
    action.sa_flags = SA_SIGINFO;

    SET_SIGACTION(SIGSEGV, action);
    SET_SIGACTION(SIGABRT, action);
    SET_SIGACTION(SIGFPE, action);
    SET_SIGACTION(SIGILL, action);
    SET_SIGACTION(SIGTERM, action);
    SET_SIGACTION(SIGQUIT, action);
    SET_SIGACTION(SIGBUS, action);
    SET_SIGACTION(SIGSYS, action);
    SET_SIGACTION(SIGIOT, action);
    SET_SIGACTION(SIGTRAP, action);
    SET_SIGACTION(SIGXCPU, action);
    SET_SIGACTION(SIGXFSZ, action);
    signal(SIGHUP, OnSingle);
}
#else
void catch_sig() {}
#endif

#ifdef __cplusplus
}//extern "C"
#endif

