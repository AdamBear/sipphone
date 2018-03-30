/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2016-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY: V0.0.0.1
*
*    DATE:2016-09-07
*
*******************************************************************/
#ifndef __EXPUSB_API_H__
#define __EXPUSB_API_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* name size max
*/
#define EXPUSB_NAME_MAX             16
/*
* hw version size
*/
#define EXPUSB_HW_NUMBER            8
/*
* sw version size
*/
#define EXPUSB_SW_NUMBER            4
/*
* hw version size
*/
#define EXPUSB_SN_NUMBER            32
/*
* run mode
*/
#define EXPUSB_MODE_LOADER          0
#define EXPUSB_MODE_APP             1



#define DK_DEVTYPE_CTRL     10000   /* phone ctrl */
#define DK_DEVTYPE_CPOD     0       /* cp phone */
#define DK_DEVTYPE_DPOD     1       /* digital phone */
#define DK_DEVTYPE_VPOD     2       /* vcs phone */
#define DK_DEVTYPE_CAM      3       /* remote camera */
#define DK_DEVTYPE_HOST     4       /* host main device */
#define DK_DEVTYPE_BOX      5       /* remote box */
#define DK_DEVTYPE_V4L      6       /* host v4l device */
#define DK_DEVTYPE_NR       7       /* number */

#define EXPUSB_DEVTYPE_EXP  2


/************************************************************************/
/*                   driver types                                       */
/************************************************************************/
/*
* type of event
*/
struct expusb_event
{
    struct timeval time;
    unsigned int   port;

#define DK_EVTTYPE_INVALID  0xffff
#define DK_EVTTYPE_DEVADD   0x0001
#define DK_EVTTYPE_DEVDEL   0x0002
#define DK_EVTTYPE_DATADD   0x0003
#define DK_EVTTYPE_DATDEL   0x0004
#define DK_EVTTYPE_KEYEVT   0x0010
#define DK_EVTTYPE_MPADD    0x0100
#define DK_EVTTYPE_MPDEL    0x0200
#define DK_EVTTYPE_MBSADD   0x0300
#define DK_EVTTYPE_MBSDEL   0x0400
    unsigned short type;

#define DK_EVTTYPE_MICPOD0  0
#define DK_EVTTYPE_MICPOD1  1
#define DK_EVTTYPE_MBSLINE  4
#define DK_EVTTYPE_MBSDEV   5
    unsigned short code;
    int            value;
};

static inline const char * exp_evttype(unsigned short evttype)
{
    switch (evttype)
    {
    case DK_EVTTYPE_DEVADD  :
        return "EVTTYPE_DEVADD";
    case DK_EVTTYPE_DEVDEL  :
        return "EVTTYPE_DEVDEL";
    case DK_EVTTYPE_DATADD  :
        return "EVTTYPE_DATADD";
    case DK_EVTTYPE_DATDEL  :
        return "EVTTYPE_DATDEL";
    case DK_EVTTYPE_KEYEVT  :
        return "EVTTYPE_KEYEVT";
    case DK_EVTTYPE_MPADD   :
        return "EVTTYPE_MPADD";
    case DK_EVTTYPE_MPDEL   :
        return "EVTTYPE_MPDEL";
    }

    return "";
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of fb
*/
struct expusb_fillrect
{
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned short page;
    unsigned short display;
    unsigned int   port;
};

struct expusb_fscreen
{
    unsigned int height;            /* height of picture in mm    */
    unsigned int width;             /* width of picture in mm     */
    unsigned int bits_per_pixel;    /* guess what */

    unsigned long smem_start;       /* Start of frame buffer mem */
    unsigned int smem_len;          /* Length of frame buffer mem */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of version
*/
struct expusb_ver
{
#define USBEXP_DEVTYPE_EXP50     0 /* exp50 */
    unsigned int type;             /* version magic */
    unsigned int version;          /* let it reserved */
    unsigned int port;
};

struct expusb_hwver
{
    unsigned char version[EXPUSB_HW_NUMBER];
    unsigned int port;
};

struct expusb_swver
{
    unsigned char version[EXPUSB_SW_NUMBER];   /* exp sw version */
    unsigned int port;
};


/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of mtd
*/
struct expusb_ptab_item
{
    char         name[8];
    unsigned int addr;
    unsigned int size;
};

struct expusb_ptab
{
#define EXPUSB_PTAB_MAX   6
    unsigned int            ptab_num;
    struct expusb_ptab_item     ptab_set[EXPUSB_PTAB_MAX];
};

struct expusb_mtdinfo
{
    /* sw sets */
    unsigned int    load_addr;
    unsigned int    load_size;

    /* last,parts table */
    struct expusb_ptab  ptab;
    unsigned int     port;
};


/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of boot
*/

struct exp_apphdr
{
#define EXPUSB_APPHDR_MAGIC     0x89A8F3B6
    unsigned int    ih_magic;   /* Image Header Magic Number */
    unsigned int    ih_hcrc;    /* Image Header CRC Checksum */
    unsigned int    ih_size;    /* Image Data Size */
    unsigned int    ih_load;    /* Data  Load  Address */
    unsigned int    ih_ep;      /* Entry Point Address */
    unsigned int    ih_dcrc;    /* Image Data CRC Checksum */
    unsigned char   ih_name[8]; /* Image Name */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of buffer
*/
struct expusb_buff
{
    unsigned int length;     /* buffer length */
    char    *    buffer;     /* buffer start */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of val
*/
struct expusb_val
{
    unsigned int val;
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of verify
*/
struct expusb_verify
{
#define EXPUSB_VERIFY_CRC32     0
#define EXPUSB_VERIFY_CRC16     1
#define EXPUSB_VERIFY_CHKSUM    2
    unsigned int type;
    unsigned int start;     /* memory start address */
    unsigned int length;    /* memory length */
    unsigned int check;     /* memory check value */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of boot
*/
struct expusb_boot
{
    unsigned int start;     /* boot start address */
    int          argc;      /* boot argc count */
    unsigned int size;      /* boot argv size */
    const char * argv;      /* boot argv string */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of mem/reg
*/
struct expusb_mem
{
#define EXP_MEM_WIDTH_8      8
#define EXP_MEM_WIDTH_16     16
#define EXP_MEM_WIDTH_32     32
    unsigned int width;     /* memory/register width */
    unsigned int start;     /* memory/register start address */
    unsigned int length;    /* memory/register length */
    void    *    memory;    /* user buffer address */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of flash
*/
struct expusb_flasherase
{
    unsigned int start;     /* erase start address */
    unsigned int length;    /* erase length */
    unsigned int port;
};
struct expusb_flashdata
{
    unsigned int start;     /* read/write start address */
    unsigned int length;    /* read/write length */
    char    *    data;      /* read/write data */
    unsigned int port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of led
*/
typedef enum
{
    LED_CLOSE    =  0,
    LED_GREEN    =  1,
    LED_RED      =  2,
    LED_YELLOW   =  3,
    LED_GREEN_F  =  4,
    LED_RED_F    =  8,
    LED_YELLOW_F = 12,
} exp_led_t;

struct expusb_led
{
    unsigned short row;
    unsigned short col;
    unsigned int   mask;
    unsigned int   flash_timer;
    unsigned int   port;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define EXPUSB_DEV_PATH              "/dev/expusb"
/************************************************************************/
/*                        driver APIs                                   */
/************************************************************************/
#define EXPUSB_IOC_MAGIC            'E'

/************************************************************************/
/*                                                                      */
/************************************************************************/
/* driver version */
#define EXPUSB_IOC_VER              _IOR(EXPUSB_IOC_MAGIC,  0x00, struct expusb_ver *)

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define EXPUSB_IOC_START            _IOW(EXPUSB_IOC_MAGIC,  0x09, struct expusb_val)
/************************************************************************/
/*                                                                      */
/************************************************************************/
/* exp loader software version */
#define EXPUSB_IOC_LOADERSWVER      _IOR(EXPUSB_IOC_MAGIC,  0x10, struct expusb_swver *)
/* exp APP software version */
#define EXPUSB_IOC_APPSWVER         _IOR(EXPUSB_IOC_MAGIC,  0x11, struct expusb_swver *)
/* exp hardware version */
#define EXPUSB_IOC_HWVER            _IOR(EXPUSB_IOC_MAGIC,  0x12, struct expusb_hwver *)

#define EXPUSB_IOC_SETENC           _IOW(EXPUSB_IOC_MAGIC,  0x13, struct expusb_val *)
#define EXPUSB_IOC_GETENC           _IOR(EXPUSB_IOC_MAGIC,  0x14, struct expusb_val *)

/* led sets,set/get */
#define EXPUSB_IOC_SETID            _IOW(EXPUSB_IOC_MAGIC,  0x15, struct expusb_val *)
#define EXPUSB_IOC_GETID            _IOR(EXPUSB_IOC_MAGIC,  0x16, struct expusb_val *)

/* led sets,set/get */
#define EXPUSB_IOC_SETLED           _IOW(EXPUSB_IOC_MAGIC,  0x17, struct expusb_led *)
#define EXPUSB_IOC_GETLED           _IOR(EXPUSB_IOC_MAGIC,  0x18, struct expusb_led *)

/* backlight set/get*/
#define EXPUSB_IOC_SETBKLIGHT       _IOW(EXPUSB_IOC_MAGIC,  0x19, struct expusb_val *)
#define EXPUSB_IOC_GETBKLIGHT       _IOR(EXPUSB_IOC_MAGIC,  0x1A, struct expusb_val *)

/* lcd sets */
#define EXPUSB_IOC_FSCREEN          _IOR(EXPUSB_IOC_MAGIC,  0x1B, struct expusb_fscreen *)
#define EXPUSB_IOC_FBFLUSH          _IOW(EXPUSB_IOC_MAGIC,  0x1C, struct expusb_fillrect *)
#define EXPUSB_IOC_FBPAGESW         _IOR(EXPUSB_IOC_MAGIC,  0x1D, struct expusb_val *)

/* get run mode */
#define EXPUSB_IOC_RUNMODE          _IOR(EXPUSB_IOC_MAGIC,  0x1E, struct expusb_val *)
/* reset exp */
#define EXPUSB_IOC_RESET            _IOW(EXPUSB_IOC_MAGIC,  0x1F, struct expusb_val)
/* boot memory */
#define EXPUSB_IOC_BOOTM            _IOW(EXPUSB_IOC_MAGIC,  0x20, struct expusb_boot *)
#define EXPUSB_IOC_VERIFY           _IOW(EXPUSB_IOC_MAGIC,  0x21, struct expusb_boot *)

/* rd/wr memory */
#define EXPUSB_IOC_WRMEM            _IOW(EXPUSB_IOC_MAGIC,  0x22, struct expusb_mem *)
#define EXPUSB_IOC_RDMEM            _IOR(EXPUSB_IOC_MAGIC,  0x23, struct expusb_mem *)

/* flash sets,erase/read/write */
#define EXPUSB_IOC_ERFLASH          _IOW(EXPUSB_IOC_MAGIC,  0x24, struct expusb_flasherase *)
#define EXPUSB_IOC_RDFLASH          _IOR(EXPUSB_IOC_MAGIC,  0x25, struct expusb_flashdata *)
#define EXPUSB_IOC_WRFLASH          _IOW(EXPUSB_IOC_MAGIC,  0x26, struct expusb_flashdata *)
/* mtdinfo gets */
#define EXPUSB_IOC_MTDINFO          _IOR(EXPUSB_IOC_MAGIC,  0x27, struct expusb_mtdinfo *)

/*sn get/set */
#define EXPUSB_IOC_RDSN             _IOR(EXPUSB_IOC_MAGIC,  0x28, struct expusb_buff *)
#define EXPUSB_IOC_WRSN             _IOW(EXPUSB_IOC_MAGIC,  0x29, struct expusb_buff *)

/*LOG get/set */
#define EXPUSB_IOC_LOGLEV_SET       _IOW(EXPUSB_IOC_MAGIC,  0x2A, struct expusb_val *)
#define EXPUSB_IOC_LOGLEV_GET       _IOR(EXPUSB_IOC_MAGIC,  0x2B, struct expusb_val *)

/* get number of exp */
#define EXPUSB_IOC_GETEXPNUM        _IOR(EXPUSB_IOC_MAGIC,  0x2C, unsigned int *)

/* show welcome screen */
#define EXPUSB_IOC_DISPLOGO         _IOR(EXPUSB_IOC_MAGIC,  0x2D, struct expusb_val *)

/*max*/
#define EXPUSB_IOC_MAXNR            0x50

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __KERNEL__

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/fb.h>


/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_open(const char * path)
{
    int retry = 300;
    int fd;

    while (retry--)
    {
        fd = open(path, O_RDWR);
	
        if (fd > 0)
        {
            return fd;
        }
        usleep(1000 * 10);
    }

    return -EFAULT;
}

static inline void expusb_helper_close(int fd)
{
    if (fd > 0)
    {
        close(fd);
    }
}

static inline int  expusb_helper_start(int fd, unsigned int port, unsigned int val)
{
    struct expusb_val start;

    start.port = port;
    start.val = val;

    return ioctl(fd, EXPUSB_IOC_START, (unsigned long)&start);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline void * expusb_helper_mmap(int fd, size_t size)
{
    void * start;

    start = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (start == NULL || start == MAP_FAILED)
    {
        return NULL;
    }

    return start;
}

static inline void expusb_helper_munmap(void * start, size_t size)
{
    if (start)
    {
        munmap(start, size);
    }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_read(int fd, void * buf, size_t count)
{
    return read(fd, buf, count);
}

static inline int  expusb_helper_write(int fd, const void * buf, size_t count)
{
    return write(fd, buf, count);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_hwver(int fd, struct expusb_hwver * version)
{
    if (version)
    {
        return ioctl(fd, EXPUSB_IOC_HWVER, (unsigned long)version);
    }

    return -EFAULT;
}

static inline int  expusb_helper_appver(int fd, struct expusb_swver * version)
{
    if (version)
    {
        return ioctl(fd, EXPUSB_IOC_APPSWVER, (unsigned long)version);
    }

    return -EFAULT;
}

static inline int  expusb_helper_loaderver(int fd, struct expusb_swver * version)
{
    if (version)
    {
        return ioctl(fd, EXPUSB_IOC_LOADERSWVER, (unsigned long)version);
    }

    return -EFAULT;
}

static inline int  expusb_helper_drvver(int fd, struct expusb_ver * version)
{
    if (version)
    {
        return ioctl(fd, EXPUSB_IOC_VER, (unsigned long)version);
    }

    return -EFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_getrunmod(int fd, struct expusb_val * mod)
{
    if (!mod)
    {
        return -EFAULT;
    }

    return ioctl(fd, EXPUSB_IOC_RUNMODE, (unsigned long)mod);
}

static inline int  expusb_helper_bootm(int fd, unsigned int port, unsigned int addr, int argc,
                                       const char * argv)
{
    struct expusb_boot boot;

    boot.port = port;
    boot.start = addr;
    boot.argc  = argc;
    boot.argv  = argv;
    boot.size  = argv ? strlen(argv) + 1 : 0;

    return ioctl(fd, EXPUSB_IOC_BOOTM, (unsigned long)&boot);
}

static inline int  expusb_helper_reset(int fd, int port, int val)
{
    struct expusb_val reset;

    reset.port = port;
    reset.val  = val;

    return ioctl(fd, EXPUSB_IOC_RESET, (unsigned long)&reset);
}

static inline int expusb_helper_memory_common(int fd, unsigned int port, unsigned int cmd,
        unsigned int bitw, unsigned int addr, void * buf, unsigned int count)
{
    if (buf && count > 0)
    {
        struct expusb_mem mem;
        int               retry;
        int               retval;

        mem.port   = port;
        mem.width  = bitw;
        mem.start  = addr;
        mem.memory = buf;
        mem.length = count;

        retry = 3;
        do
        {
            retval = ioctl(fd, cmd, (unsigned long)&mem);
        }
        while (retval < 0 && retry--);

        if (retval < 0)
        {
            return -EFAULT;
        }
        return 0;
    }

    return -EFAULT;
}

static inline int  expusb_helper_memory_read(int fd, unsigned int port, unsigned int bitw,
        unsigned int addr, void * buf, unsigned int count)
{
    return expusb_helper_memory_common(fd, port, EXPUSB_IOC_RDMEM, bitw, addr, buf, count);
}

static inline int  expusb_helper_memory_write(int fd, unsigned int port, unsigned int bitw,
        unsigned int addr, const void * buf, unsigned int count)
{
    return expusb_helper_memory_common(fd, port, EXPUSB_IOC_WRMEM, bitw, addr, (void *)buf, count);
}

static inline int  expusb_helper_verify(int fd,  unsigned int port, unsigned int type,
                                        unsigned int addr, unsigned int size, unsigned int check)
{
    struct expusb_verify vfy;

    vfy.type   = type;
    vfy.start  = addr;
    vfy.length = size;
    vfy.check  = check;
    vfy.port   = port;

    return ioctl(fd, EXPUSB_IOC_VERIFY, (unsigned long)&vfy);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_flash_erase(int fd, unsigned int port, unsigned int addr,
        unsigned int size)
{
    struct expusb_flasherase erase;

    erase.port   = port;
    erase.start  = addr;
    erase.length = size;

    return ioctl(fd, EXPUSB_IOC_ERFLASH, (unsigned long)&erase);
}

static inline int expusb_helper_flash_common(int fd, unsigned int port, unsigned int cmd,
        unsigned int addr, void * buf, unsigned int count)
{
    if (buf && count > 0)
    {
        unsigned int offset;
        char * data = (char *)buf;
        struct expusb_flashdata fdata;
        int retry;
        int retval;

        fdata.port   = port;
        fdata.start  = addr;
        fdata.data   = data;
        fdata.length = count;

        retry = 3;
        do
        {
            retval = ioctl(fd, cmd, (unsigned long)&fdata);
        }
        while (retval < 0 && retry--);

        if (retval < 0)
        {
            return -EFAULT;
        }

        return 0;
    }

    return -EFAULT;
}

static inline int  expusb_helper_flash_read(int fd, unsigned int port, unsigned int addr,
        void * buf, unsigned int count)
{
    return expusb_helper_flash_common(fd, port, EXPUSB_IOC_RDFLASH, addr, buf, count);
}

static inline int  expusb_helper_flash_write(int fd, unsigned int port, unsigned int addr,
        const void * buf, unsigned int count)
{
    return expusb_helper_flash_common(fd, port, EXPUSB_IOC_WRFLASH, addr, (void *)buf, count);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_led_set(int fd, struct expusb_led led)
{
    return ioctl(fd, EXPUSB_IOC_SETLED, (unsigned long)&led);
}

static inline int  expusb_helper_led_get(int fd, struct expusb_led * led)
{
    if (!led)
    {
        return -EFAULT;
    }

    return ioctl(fd, EXPUSB_IOC_GETLED, (unsigned long)led);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

static inline int expusb_helper_fb_fscreeninfo(int fd, unsigned int port,
        struct fb_fix_screeninfo * finfo)
{
    struct expusb_fscreen fscreen;

    if (!finfo)
    {
        return -EFAULT;
    }

    fscreen.port = port;

    if (ioctl(fd, EXPUSB_IOC_FSCREEN, (unsigned long)&fscreen) < 0)
    {
        return -EFAULT;
    }

    finfo->smem_start = fscreen.smem_start;
    finfo->smem_len   = fscreen.smem_len;

    return 0;
}

static inline int expusb_helper_fb_vscreeninfo(int fd, unsigned int port,
        struct fb_var_screeninfo * vinfo)
{
    struct expusb_fscreen fscreen;

    if (!vinfo)
    {
        return -EFAULT;
    }

    fscreen.port = port;

    if (ioctl(fd, EXPUSB_IOC_FSCREEN, (unsigned long)&fscreen) < 0)
    {
        return -EFAULT;
    }

    vinfo->bits_per_pixel = fscreen.bits_per_pixel;
    vinfo->height         = fscreen.height;
    vinfo->width          = fscreen.width;
    vinfo->yres           = fscreen.height;
    vinfo->xres           = fscreen.width;

    return 0;
}

static inline int  expusb_helper_fb_flush(int fd, unsigned int port, unsigned short page,
        unsigned short display)
{
    struct expusb_fillrect rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = 0;
    rect.page    = page;
    rect.display = display;
    rect.port = port;

    return ioctl(fd, EXPUSB_IOC_FBFLUSH, (unsigned long)&rect);
}

static inline int  expusb_helper_fb_fillrect(int fd, struct expusb_fillrect rect)
{
    return ioctl(fd, EXPUSB_IOC_FBFLUSH, (unsigned long)&rect);
}

static inline int  expusb_helper_fb_pagedisp(int fd, unsigned int port, unsigned int page)
{
    struct expusb_val val;

    val.port = port;
    val.val  = page;

    return ioctl(fd, EXPUSB_IOC_FBPAGESW, (unsigned long)&val);
}

static inline int expusb_helper_fb_logodisp(int fd, unsigned int port)
{
    struct expusb_val val;

    val.port = port;
    val.val  = 0;

    return ioctl(fd, EXPUSB_IOC_DISPLOGO, (unsigned long)&val);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_setbklight(int fd, unsigned int port, unsigned int level)
{
    struct expusb_val bklight;

    bklight.port = port;
    bklight.val = level;
    return ioctl(fd, EXPUSB_IOC_SETBKLIGHT, (unsigned long)&bklight);
}

static inline int  expusb_helper_getbklight(int fd, struct expusb_val * bklight)
{
    if (!bklight)
    {
        return -EFAULT;
    }

    return ioctl(fd, EXPUSB_IOC_GETBKLIGHT, (unsigned long)bklight);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  expusb_helper_rdsn(int fd, unsigned int port, char * buff, unsigned int len)
{
    struct expusb_buff sn;

    if (!buff || len < EXPUSB_SN_NUMBER)
    {
        return -EINVAL;
    }

    sn.port = port;
    sn.buffer = buff;
    sn.length = len;

    return ioctl(fd, EXPUSB_IOC_RDSN, (unsigned long)&sn);
}

static inline int  expusb_helper_wrsn(int fd, unsigned int port, char * buff, unsigned int len)
{
    struct expusb_buff sn;

    if (!buff || len < EXPUSB_SN_NUMBER)
    {
        return -EINVAL;
    }

    sn.port = port;
    sn.buffer = buff;
    sn.length = len;

    return ioctl(fd, EXPUSB_IOC_WRSN, (unsigned long)&sn);
}

static inline int expusb_helper_expnum_get(int fd, unsigned int * expnum)
{
    if (!expnum)
    {
        return -EINVAL;
    }

    return ioctl(fd, EXPUSB_IOC_GETEXPNUM, (unsigned long)expnum);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
};
#endif

#endif /*__KERNEL__*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__EXPUSB_API_H__*/
