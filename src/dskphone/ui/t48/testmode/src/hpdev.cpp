/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-10-08
*
*******************************************************************/
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <linux/netlink.h>
#include <linux/kdev_t.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "hpdev.h"

#ifdef WIN32
#define close(s) closesocket(s)
#endif

/************************************************************************/
/*macro                                                                 */
/************************************************************************/
#define HPDEV_MAX_BUF_UNION     1024
#define HPDEV_UEVENT_MSG_LEN    (2*HPDEV_MAX_BUF_UNION)

#define HPDEV_USB_DEV_KEY   "usb"
#define HPDEV_STORAGE_KEY   "/block/sd"
#define HPDEV_BT_KEY        "/bluetooth/"
#define HPDEV_DIR_STORAGE_KEY "sd"
#define HPDEV_DIR_BT_KEY      "hci"

#define HPDEV_UEVENT_INIT(uevent) do {uevent->action = "";\
                                uevent->path = "";   \
                                uevent->subsystem = ""; \
                                uevent->firmware = "";  \
                                uevent->major = -1;     \
                                uevent->minor = -1;     \
                                uevent->partition_name = NULL; \
                                uevent->partition_num = -1; \
                                } while (0)

/************************************************************************/
/* type                                                                 */
/************************************************************************/
typedef struct
{
    int fd;
    int halt;
    void * lpPrivate;
    pthread_t hpdev_tid;
    struct pollfd fds;
    HPDEV_CB hpdev_evcb;
} HPDEV_INST;

struct uevent
{
    const char * action;
    const char * path;
    const char * subsystem;
    const char * firmware;
    const char * partition_name;
    int partition_num;
    int major;
    int minor;
};

#ifndef __cplusplus
struct ucred
{
    pid_t pid;          /* PID of sending process.  */
    uid_t uid;          /* UID of sending process.  */
    gid_t gid;          /* GID of sending process.  */
};
#endif

/************************************************************************/
/* static fun                                                           */
/************************************************************************/
static void * hpdev_do_run(void * arg)
{
    HPDEV_INST * hpdev_inst;
    HPDEV_HANDLE handle;
    int ret;
    int time;

    handle = (HPDEV_HANDLE)arg;
    hpdev_inst = (HPDEV_INST *)arg;
    time = HPDEV_INFTIM;

    while (!hpdev_inst->halt)
    {
        hpdev_set_events(handle, POLLIN);

        ret = hpdev_poll(handle, time);
        if (ret < 0)
        {
            ERR("hpdev poll err!\n");
            continue;
        }

        ret = hpdev_process(handle);
        if (ret < 0)
        {
            ERR("hpdev msg process err!\n");
        }
    }

    pthread_exit(0);
    return NULL;
}

/*判断设备是否为存储设备*/
static char * hpdev_storage_chk(const char * str, const char * keyword)
{
    char * charp;

    charp = (char *)strstr(str, keyword);
    if (charp)
    {
        charp += (strlen(keyword) + 1);
        if (*charp++ != '/')
        {
            charp = NULL;
        }
    }

    return charp;
}

/*判断设备是否为蓝牙设备*/
static char * hpdev_bt_chk(const char * str, const char * keyword)
{
    char * charp;

    charp = (char *)strstr(str, keyword);
    if (charp)
    {
        charp += strlen(keyword);
    }

    return charp;
}

/*判断是否有设备插入*/
static int hpdev_usb_dev_chk(struct uevent * uevent, const char * keyword)
{
    int ret;

    ret = strcmp(uevent->subsystem, keyword);
    if (0 == ret && uevent->major >= 0 && uevent->minor >= 0)
    {
        return ret;
    }

    return -1;
}

static int hpdev_uevtsub2evtsub(const char * subsystem)
{
    if (!strncmp(subsystem, "block", strlen("block")))
    {
        return HPDEV_SUB_BLOCK;
    }
    else if (!strncmp(subsystem, "bluetooth", strlen("bluetooth")))
    {
        return HPDEV_SUB_BLUETOOTH;
    }

    return HPDEV_SUB_INVALID;
}

static int hpdev_parse_event(char * msg, struct uevent * uevent)
{
    HPDEV_UEVENT_INIT(uevent);

    if (!*msg)
    {
        return -1;
    }

    /* currently ignoring SEQNUM */
    while (*msg)
    {
        if (!strncmp(msg, "ACTION=", 7))
        {
            msg += 7;
            uevent->action = msg;
        }
        else if (!strncmp(msg, "DEVPATH=", 8))
        {
            msg += 8;
            uevent->path = msg;
        }
        else if (!strncmp(msg, "SUBSYSTEM=", 10))
        {
            msg += 10;
            uevent->subsystem = msg;
        }
        else if (!strncmp(msg, "FIRMWARE=", 9))
        {
            msg += 9;
            uevent->firmware = msg;
        }
        else if (!strncmp(msg, "MAJOR=", 6))
        {
            msg += 6;
            uevent->major = atoi(msg);
        }
        else if (!strncmp(msg, "MINOR=", 6))
        {
            msg += 6;
            uevent->minor = atoi(msg);
        }
        else if (!strncmp(msg, "PARTN=", 6))
        {
            msg += 6;
            uevent->partition_num = atoi(msg);
        }
        else if (!strncmp(msg, "PARTNAME=", 9))
        {
            msg += 9;
            uevent->partition_name = msg;
        }

        /* advance to after the next \0 */
        while (*msg++)
            ;
    }
    INFO("Parse uevent:\nACTION=%s\nDEVPATH=%s\nSUBSYSTEM=%s\nFIRMWARE=%s\nMAJOR=%d\nMINOR=%d\nPARTN=%d\nPARTNAME=%s\n",
         uevent->action, uevent->path, uevent->subsystem, uevent->firmware, uevent->major, uevent->minor,
         uevent->partition_num,
         uevent->partition_name);

    return 0;
}

/*
* 如需要处理新设备可在该函数中添加设备类型和设备动作判断支持。
*/
static int hpdev_decode(char * buf, HPDEV_EVT * hpdev_evt)
{
    struct uevent uevent;
    char * start = buf;
    char * charp;

    INFO("netlink msg:\n%s\n", start);

    if (hpdev_parse_event(start, &uevent) < 0)
    {
        return -1;
    }

    if (!strcmp(uevent.action, "add"))
    {
        hpdev_evt->dev_event = HPDEV_EVENT_ADD;
        hpdev_evt->dev_major = uevent.major;
        hpdev_evt->dev_minor = uevent.minor;

        charp = hpdev_storage_chk(uevent.path, HPDEV_STORAGE_KEY);
        if (charp)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_STORAGE;
            hpdev_evt->dev_subsystem = hpdev_uevtsub2evtsub(uevent.subsystem);
            strcpy(hpdev_evt->dev_node, charp);

            return 0;
        }

        charp = hpdev_bt_chk(uevent.path, HPDEV_BT_KEY);
        if (charp)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_BT_HOST;
            hpdev_evt->dev_subsystem = hpdev_uevtsub2evtsub(uevent.subsystem);;
            strcpy(hpdev_evt->dev_node, charp);

            return 0;
        }

        if (hpdev_usb_dev_chk(&uevent, HPDEV_USB_DEV_KEY) == 0)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_UNKNOWN;
            hpdev_evt->dev_subsystem = HPDEV_SUB_INVALID;
            strcpy(hpdev_evt->dev_node, "");

            return 0;
        }

        return -1;
    }
    else if (!strcmp(uevent.action, "remove"))
    {
        hpdev_evt->dev_event = HPDEV_EVENT_REMOVE;
        hpdev_evt->dev_major = uevent.major;
        hpdev_evt->dev_minor = uevent.minor;

        charp = hpdev_storage_chk(uevent.path, HPDEV_STORAGE_KEY);
        if (charp)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_STORAGE;
            hpdev_evt->dev_subsystem = hpdev_uevtsub2evtsub(uevent.subsystem);;
            strcpy(hpdev_evt->dev_node, charp);

            return 0;
        }

        charp = hpdev_bt_chk(uevent.path, HPDEV_BT_KEY);
        if (charp)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_BT_HOST;
            hpdev_evt->dev_subsystem = hpdev_uevtsub2evtsub(uevent.subsystem);;
            strcpy(hpdev_evt->dev_node, charp);

            return 0;
        }

        if (hpdev_usb_dev_chk(&uevent, HPDEV_USB_DEV_KEY) == 0)
        {
            hpdev_evt->dev_type = HPDEV_TYPE_UNKNOWN;
            hpdev_evt->dev_subsystem = HPDEV_SUB_INVALID;
            strcpy(hpdev_evt->dev_node, "");

            return 0;
        }

        return -1;
    }
    else if (!strcmp(uevent.action, "change"))
    {
        hpdev_evt->dev_event = HPDEV_EVENT_CHANGE;

        /*ignoring change msg*/
        return -1;
    }
    else
    {
        WARNING("unknown msg!\n");
    }

    return -1;
}

/************************************************************************/
/*create/destroy                                                        */
/************************************************************************/
int hpdev_create(HPDEV_HANDLE * pHandle, HPDEV_CB hpdev_evcb, void * private_data)
{
    struct sockaddr_nl nladdr;
    HPDEV_INST * hpdev_inst;
    int sz = 64 * HPDEV_MAX_BUF_UNION;
    int on = 1;
    int fd;

    hpdev_inst = (HPDEV_INST *)malloc(sizeof(HPDEV_INST));
    if (hpdev_inst == NULL)
    {
        ERR("No memory for hpdev!\n");
        return -1;
    }
    memset(hpdev_inst, 0, sizeof(HPDEV_INST));
    hpdev_inst->fd = -1;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0xffffffff;

    if ((fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)) < 0)
    {
        ERR("Unable to create uevent socket!\n");
        goto create_err;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, (const char *)&sz, sizeof(sz)) < 0)
    {
        ERR("Unable to set uevent socket SO_RECBUFFORCE\n!");
        goto set_err;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_PASSCRED, (const char *)&on, sizeof(on)) < 0)
    {
        ERR("Unable to set uevent socket SO_PASSCRED\n");
        goto set_err;
    }

    if (bind(fd, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0)
    {
        ERR("Unable to bind uevent socket\n");
        goto set_err;
    }

    hpdev_inst->fd = fd;
    hpdev_inst->halt = 0;
    hpdev_inst->lpPrivate = private_data;
    hpdev_inst->fds.fd = fd;
#ifdef WIN32
    hpdev_inst->hpdev_tid.p = 0;
#else
    hpdev_inst->hpdev_tid = -1;
#endif
    hpdev_inst->hpdev_evcb = hpdev_evcb;

    *pHandle = (HPDEV_HANDLE)hpdev_inst;

    INFO("hpdev create succ!\n");

    return 0;

set_err:
    close(fd);
create_err:
    *pHandle = 0;
    free(hpdev_inst);

    return -1;
}

int hpdev_destroy(HPDEV_HANDLE hHandle)
{
    HPDEV_INST * hpdev_inst;

    if (!hHandle)
    {
        return 0;
    }

    hpdev_inst = (HPDEV_INST *)hHandle;

    if (hpdev_inst->fd >= 0)
    {
        close(hpdev_inst->fd);
    }
    if (hpdev_inst)
    {
        free(hpdev_inst);
    }

    INFO("hpdev destroy succ!\n");

    return 0;
}

/************************************************************************/
/*run/halt                                                              */
/************************************************************************/
int hpdev_run(HPDEV_HANDLE hHandle)
{
    HPDEV_INST * hpdev_inst;
    int tid;

    /* May be a USB storage have been pluged in before we invoke this function. */
    hpdev_dir_scan(hHandle);

    hpdev_inst = (HPDEV_INST *)hHandle;

    tid = pthread_create(&hpdev_inst->hpdev_tid, NULL, hpdev_do_run, hpdev_inst);
    if (tid < 0)
    {
        ERR("hpdev_run thread create err!\n");
        return -1;
    }

    return 0;
}

int hpdev_halt(HPDEV_HANDLE hHandle)
{
    HPDEV_INST * hpdev_inst;

    hpdev_inst = (HPDEV_INST *)hHandle;
    hpdev_inst->halt = 1;

    pthread_cancel(hpdev_inst->hpdev_tid);
    pthread_join(hpdev_inst->hpdev_tid, 0);

    INFO("hpdev_halt succ!\n");

    return 0;
}

/************************************************************************/
/*poll/process*                                                         */
/************************************************************************/
int hpdev_set_events(HPDEV_HANDLE hHandle, short events)
{
    HPDEV_INST * hpdev_inst;

    hpdev_inst = (HPDEV_INST *)hHandle;
    hpdev_inst->fds.events = events;

    return 0;
}

int hpdev_poll(HPDEV_HANDLE hHandle, int time)
{
    HPDEV_INST * hpdev_inst;

    hpdev_inst = (HPDEV_INST *)hHandle;

    return poll(&hpdev_inst->fds, 1, time);
}

int hpdev_process(HPDEV_HANDLE hHandle)
{
    HPDEV_EVT hpdev_evt;
    HPDEV_INST * hpdev_inst;
    char buf[HPDEV_UEVENT_MSG_LEN + 2];
    struct sockaddr_nl snl;
    struct ucred * cred;
    struct cmsghdr * cmsg;
    char cred_msg[CMSG_SPACE(sizeof(struct ucred))];
    struct iovec iov;
    struct msghdr hdr;
    int size;

    hpdev_inst = (HPDEV_INST *)hHandle;
    iov.iov_base = &buf;
    iov.iov_len = sizeof(buf);
    memset(&hdr, 0, sizeof(struct msghdr));
    hdr.msg_name = &snl;
    hdr.msg_namelen = sizeof(snl);
    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = cred_msg;
    hdr.msg_controllen = sizeof(cred_msg);

    if (hpdev_inst->fds.revents & POLLIN)
    {
        size = recvmsg(hpdev_inst->fd, &hdr, 0);
        if (size < 0)
        {
            ERR("unable to receive message!\n");
            return -1;
        }

        if ((snl.nl_groups != 1) || (snl.nl_pid != 0))
        {
            INFO("ignoring non-kernel netlink multicast message\n");
            return 0;
        }

        cmsg = CMSG_FIRSTHDR(&hdr);
        if (cmsg == NULL || cmsg->cmsg_type != SCM_CREDENTIALS)
        {
            INFO("ignoring message with no sender credentials\n");
            return 0;
        }

        cred = (struct ucred *)CMSG_DATA(cmsg);
        if (cred->uid != 0)
        {
            INFO("ignoring message from non-root UID %d\n", cred->uid);
            return 0;
        }

        if (size < 32 || size >= HPDEV_UEVENT_MSG_LEN)
        {
            INFO("invalid message length!\n");
            return -1;
        }
        buf[size] = '\0';
        buf[size + 1] = '\0';

        if (strstr(buf, "@/") == NULL)
        {
            INFO("ignor message !\n");
            return 0;
        }

        if (!hpdev_decode(buf, &hpdev_evt))
        {
            if (hpdev_inst->hpdev_evcb)
            {
                hpdev_inst->hpdev_evcb(hpdev_inst->lpPrivate, &hpdev_evt);
            }
        }
    }

    return 0;
}

/*
*  We also can use this function to detect hotplug devices.
*/
int hpdev_dir_scan(HPDEV_HANDLE hHandle)
{
    DIR * dir;
    struct dirent * entry;
    struct stat stat_buf;
    HPDEV_INST * hpdev_inst;
    HPDEV_EVT hpdev_evt;
    char  filename[128];

    hpdev_inst = (HPDEV_INST *)hHandle;
    /* detect usb storages */
    dir = opendir("/dev");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strncmp(entry->d_name, HPDEV_DIR_STORAGE_KEY, strlen(HPDEV_DIR_STORAGE_KEY)))
        {
            continue;
        }

        if (strlen(entry->d_name) >= 4)
        {
            hpdev_evt.dev_event = HPDEV_EVENT_ADD;

            sprintf(filename, "/dev/%s", entry->d_name);
            if (stat(filename, &stat_buf) < 0)
            {
                continue;
            }

            hpdev_evt.dev_major = MAJOR(stat_buf.st_rdev);
            hpdev_evt.dev_minor = MINOR(stat_buf.st_rdev);
            hpdev_evt.dev_type = HPDEV_TYPE_STORAGE;
            hpdev_evt.dev_subsystem = HPDEV_SUB_BLOCK;
            strcpy(hpdev_evt.dev_node, entry->d_name);

            if (hpdev_inst->hpdev_evcb)
            {
                hpdev_inst->hpdev_evcb(hpdev_inst->lpPrivate, &hpdev_evt);
            }
        }
    }

    closedir(dir);

    /* detect bluetooth devices */
    dir = opendir("/sys/class/bluetooth");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strncmp(entry->d_name, HPDEV_DIR_BT_KEY, strlen(HPDEV_DIR_BT_KEY)))
        {
            continue;
        }

        if (strlen(entry->d_name) >= 4)
        {
            hpdev_evt.dev_event = HPDEV_EVENT_ADD;

            sprintf(filename, "/sys/class/bluetooth/%s", entry->d_name);
            if (stat(filename, &stat_buf) < 0)
            {
                continue;
            }

            hpdev_evt.dev_major = MAJOR(stat_buf.st_rdev);
            hpdev_evt.dev_minor = MINOR(stat_buf.st_rdev);

            hpdev_evt.dev_type = HPDEV_TYPE_BT_HOST;
            hpdev_evt.dev_subsystem = HPDEV_SUB_BLUETOOTH;
            strcpy(hpdev_evt.dev_node, entry->d_name);

            if (hpdev_inst->hpdev_evcb)
            {
                hpdev_inst->hpdev_evcb(hpdev_inst->lpPrivate, &hpdev_evt);
            }
        }
    }

    closedir(dir);

    return 0;
}
/************************************************************************/
/*helper                                                                */
/************************************************************************/
int hpdev_helper_ispoint_mounted(const char * mount_point)
{
    char device[PATH_MAX];
    char mount_path[PATH_MAX];
    char rest[PATH_MAX];
    FILE * fp;
    char line[1024];

    if (!mount_point)
    {
        return -1;
    }

    if (!(fp = fopen("/proc/mounts", "r")))
    {
        ERR("Error opening /proc/mounts!\n");
        return -1;
    }

    while (fgets(line, sizeof(line), fp))
    {
        line[strlen(line) - 1] = '\0';
        sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
        if (!strcmp(mount_path, mount_point))
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int hpdev_helper_mount(const char * dev, const char * mount_point, const char * fs_type,
                       unsigned long mountflags, void * data)
{
    int ret;

    ret = mount(dev, mount_point, fs_type, mountflags, data);
    if (ret < 0)
    {
        perror("Mount err");
        return ret;
    }

    INFO("mount %s succ\n", dev);

    return ret;
}

int hpdev_helper_umount(const char * mount_point)
{
    int ret;

    ret = umount(mount_point);
    if (ret < 0)
    {
        perror("Umount err");
        return ret;
    }

    INFO("umont %s succ\n", mount_point);

    return ret;
}

int hpdev_helper_create_devnode(const char * path, int major, int minor)
{
    mode_t mode = 0660 | S_IFBLK;
    dev_t dev = (major << 8) | minor;

    if (mknod(path, mode, dev) < 0)
    {
        if (errno != EEXIST)
        {
            return -1;
        }
    }

    return 0;
}

int hpdev_helper_format(const char * dev_node)
{
    return 0;
}
