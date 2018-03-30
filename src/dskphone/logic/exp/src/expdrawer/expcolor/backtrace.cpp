#ifndef _XWIN
#include "backtrace.h"

#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include <signal.h>
#endif
#include <iostream>
#include <string.h>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

static void MyBackTrace(unsigned long * fb, int i)
{
#ifndef _WIN32
    if (i == 0)
    {
        return;
    }
    unsigned long * pc = (unsigned long *)(*fb);


    printf("Stack [%d],the func name 0x%3x\n", i, pc);

    unsigned long * prefb = (unsigned long *)(*(fb - 1));

    //printf("the prefbt 0x%3x\n",prefb);

    if (prefb == NULL)
    {
        return;
    }

    i--;

    MyBackTrace(prefb, i);
#endif
}


void PrintMaps()
{
#ifndef _WIN32
    char szMemFile[100] = "";
    char buf[256] = "";
    sprintf(szMemFile, "/proc/%d/maps", getpid());

    FILE * pf = fopen(szMemFile, "rb");
    if (pf != NULL)
    {
        printf("mapsfile_begin===============%s===============\n", szMemFile);
        while (fgets(buf, 256, pf) != NULL)
        {
            printf("Lib-In:%s\n", buf);
            memset(buf, 0, 256);
        }
        printf("mapsfile_end===============%s===============\n", szMemFile);
        fclose(pf);
    }
#endif
}

#ifndef _WIN32
static void sig_handler(int signo, siginfo_t * info, void * ptr)
{
    if (info == NULL || ptr == NULL)
    {
        return;
    }

    PrintMaps();

    printf("~~~~~~~~~~~~~~~~~~~~Fault Trace begin~~~~~~~~~~~~~~~~~~~~\n");

    printf("info.si_signo = %d\n", signo);
    printf("info.si_errno = %d\n", info->si_errno);
    printf("info.si_code  = %d\n", info->si_code);
    printf("info.si_addr  = %p\n", info->si_addr);
    printf("sender pid = %d, sender uid = %d\n", info->si_pid, info->si_uid);

    ucontext_t * ucontext = (ucontext_t *)ptr;
    // for arm
    printf("the arm_fp 0x%3x\n", ucontext->uc_mcontext.arm_fp);
    printf("the arm_ip 0x%3x\n", ucontext->uc_mcontext.arm_ip);
    printf("the arm_sp 0x%3x\n", ucontext->uc_mcontext.arm_sp);
    printf("the arm_lr 0x%3x\n", ucontext->uc_mcontext.arm_lr);
    printf("the arm_pc 0x%3x\n", ucontext->uc_mcontext.arm_pc);
    printf("the arm_cpsr 0x%3x\n", ucontext->uc_mcontext.arm_cpsr);
    printf("the falut_address 0x%3x\n", ucontext->uc_mcontext.fault_address);

    printf("the function dump stack:\n");

    unsigned long * bp = (unsigned long *)ucontext->uc_mcontext.arm_fp;

    int k = 10;

    MyBackTrace(bp, k);

    // 通知各模块dump info
    //msgSendMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), INNER_MSG_CATCH_SIG_TO_EXIT, signo, 0);

    printf("~~~~~~~~~~~~~~~~~~~~Fault Trace end~~~~~~~~~~~~~~~~~~~~\n");

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
            printf("Catch Single: SIGQUIT\n");
//          commonUnits_System("echo Catch Single > /tmp/log.txt");
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
#endif

#ifdef __cplusplus
}//extern "C"
#endif
#endif // _XWIN

