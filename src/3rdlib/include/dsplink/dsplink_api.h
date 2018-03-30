/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2013 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2013-08-01
*
*******************************************************************/
#ifndef __DSPLINK_API_H__
#define __DSPLINK_API_H__

#ifndef WIN32
/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* ver-0:0x86E59100
*/
#define DK_DRV_VERSION          0x86E59100

/*
*   Common define:
*   B3[7:6]:    = 运行模式         (11==UBL | 10==BIOS | Reserved)
*   B3[5:0]:    = 保留
*   B2[7:0]:    = 保留

*   Function define:
*   B1[7:4]:    = flash type       (Reserved)
*   B1[3:0]:    = DDR type         (Reserved)
*   B0[7:6]：   = CPU型号          (Reserved)
*   B0[5:2]：   = LCD型号          (Reserved)
*   B0[1:0]：   = 保留
*/
/* Mask */
#define DK_DSP_RUNMODE_MASK     (0xC0000000)
#define DK_DSP_FLASH_MASK       (0x0000F000)
#define DK_DSP_DDR_MASK         (0x00000F00)
#define DK_DSP_CPU_MASK         (0x000000C0)
#define DK_DSP_LCDTYPE_MASK     (0x0000003C)

/* Value:run mode */
#define DK_DSP_RUNMODE_UBL      (0xC0000000)
#define DK_DSP_RUNMODE_APP      (0x80000000)
/* Value:flash */
#define DK_DSP_FLASH_MX25L3206E (0x00000000)
#define DK_DSP_FLASH_MX25L6406E (0x00001000)
/* Value:DDR */
#define DK_DSP_DDR_WINBONE      (0x00000000)
/* Value:CPU */
#define DK_DSP_CPU_C6746        (0x00000000)
#define DK_DSP_CPU_C6748        (0x00000040)
/* Value:LCD */
#define DK_DSP_LCD_UC1609       (0x00000000)

/* APIs */
#define DK_DSP_RUNMODE(v)       ((v) & DK_DSP_RUNMODE_MASK)

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* name size max
*/
#define DK_NAME_MAX             16

/*
* playload size max
*/
#define DK_PAYLOAD_MAX          1280

/************************************************************************/
/*                   driver types                                       */
/************************************************************************/
/*
* type of event
*/
struct dk_event
{
    struct timeval time;

#define DK_EVTTYPE_INVALID  0xffff
#define DK_EVTTYPE_DEVADD   0x0001
#define DK_EVTTYPE_DEVDEL   0x0002
#define DK_EVTTYPE_DATADD   0x0003
#define DK_EVTTYPE_DATDEL   0x0004
#define DK_EVTTYPE_KEYEVT   0x0010
#define DK_EVTTYPE_MPADD    0x0100
#define DK_EVTTYPE_MPDEL    0x0200
    unsigned short type;

#define DK_EVTTYPE_MICPOD0  0
#define DK_EVTTYPE_MICPOD1  1
    unsigned short code;
    int            value;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of fb
*/
struct dk_fillrect
{
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
};

struct dk_fscreen
{
    unsigned int height;            /* height of picture in mm    */
    unsigned int width;             /* width of picture in mm     */
    unsigned int bits_per_pixel;    /* guess what */

    unsigned long smem_start;       /* Start of frame buffer mem */
    unsigned int smem_len;          /* Length of frame buffer mem */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of version
*/
struct dk_drv_ver
{
    int  version;           /* version magic */
    int  reserved;          /* let it reserved */
};

struct dk_dsp_ver
{
#define DK_DEVTYPE_CTRL     10000   /* phone ctrl */
#define DK_DEVTYPE_CPOD     0       /* cp phone */
#define DK_DEVTYPE_DPOD     1       /* digital phone */
#define DK_DEVTYPE_VPOD     2       /* vcs phone */
#define DK_DEVTYPE_NR       3       /* number */
    unsigned int dev_type;  /* dsp type */
    unsigned int version;   /* dsp version */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of buffer
*/
struct dk_buff
{
    unsigned int length;     /* buffer length */
    char        *buffer;     /* buffer start */
};

struct dk_ext_info
{
#define DK_EXT_SOP 0x8000
#define DK_EXT_EOP 0x4000
    unsigned short  flags;

    /*
    * ext define
    */
#define DK_EXT_H2S_CMD_BASE     100
#define DK_EXT_S2H_CMD_BASE     20000
#define DK_EXT_DUP_CMD_BASE     40000

    /* host to slave */
#define DK_EXT_H2S_CMD_DEBUG    (DK_EXT_H2S_CMD_BASE+0)

    /* slave to host */

    /* host to slave or slave to host */
#define DK_EXT_DUP_CMD_DKDUMP   (DK_EXT_DUP_CMD_BASE+0)
#define DK_EXT_DUP_CMD_ECHO     (DK_EXT_DUP_CMD_BASE+1)

    unsigned short  cmd;

    /*
    * for payload
    */
    unsigned int    size;
    unsigned char   buff[0];
};

#define DK_EDATA_SIZE(i)    (sizeof(struct dk_ext_info) + (i)->size)
#define DK_EDATA_PLMAX      (DK_PAYLOAD_MAX - sizeof(struct dk_ext_info))

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of mem/reg
*/
struct dk_mem
{
#define DK_MEM_WIDTH_8      8
#define DK_MEM_WIDTH_16     16
#define DK_MEM_WIDTH_32     32
    unsigned int width;     /* memory/register width */
    unsigned int start;     /* memory/register start address */
    unsigned int length;    /* memory/register length */
    void        *memory;    /* user buffer address */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of verify
*/
struct dk_verify
{
#define DK_VERIFY_CRC32     0
#define DK_VERIFY_CRC16     1
#define DK_VERIFY_CHKSUM    2
    unsigned int type;
    unsigned int start;     /* memory start address */
    unsigned int length;    /* memory length */
    unsigned int check;     /* memory check value */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of bootm
*/
struct dk_bootm
{
    unsigned int start;     /* boot start address */
    int          argc;      /* boot argc count */
    unsigned int size;      /* boot argv size */
    const char  *argv;      /* boot argv string */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of flash
*/
struct dk_flasherase
{
    unsigned int start;     /* erase start address */
    unsigned int length;    /* erase length */
};
struct dk_flashdata
{
    unsigned int start;     /* read/write start address */
    unsigned int length;    /* read/write length */
    char        *data;      /* read/write data */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of led
*/
struct dk_led
{
#define DK_LEDMASK_LED0         (1<< 0)    /* led location 0 */
#define DK_LEDMASK_LED1         (1<< 1)    /* led location 1 */
#define DK_LEDMASK_MP_LED0      (1<<24)    /* led location micpod 0 红灯*/ 
#define DK_LEDMASK_MP_LED1      (1<<25)    /* led location micpod 1 绿灯*/

    unsigned int mask;      /* led mask */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* type of vpm
*/
struct dk_vpm
{
    unsigned int length;     /* buffer length */
    char        *buffer;     /* buffer start */
    unsigned int retlen;     /* buffer length */
    char        *retbuf;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline const char *dk_devname(unsigned int dev_type)
{
    switch (dev_type)
    {
    case DK_DEVTYPE_CTRL:
        return "ext_device";
    case DK_DEVTYPE_CPOD:
        return "ext_cp";
    case DK_DEVTYPE_DPOD:
        return "ext_micpod";
    case DK_DEVTYPE_VPOD:
        return "ext_phone";
    }

    return "";
}
static inline const char *dk_devname2(unsigned int dev_type)
{
    switch (dev_type)
    {
    case DK_DEVTYPE_CPOD:
        return "ext_cp_data";
    case DK_DEVTYPE_VPOD:
        return "ext_phone_data";
    }

    return "";
}

/************************************************************************/
/*                        driver APIs                                   */
/************************************************************************/
#define DK_IOC_MAGIC            'D'

/************************************************************************/
/*                                                                      */
/************************************************************************/
/* driver version */
#define DK_IOC_DRVVER           _IOR(DK_IOC_MAGIC,  0x00, struct dk_drv_ver *)

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DK_IOC_START            _IOR(DK_IOC_MAGIC,  0x10, int)

/************************************************************************/
/*                                                                      */
/************************************************************************/
/* dsp version */
#define DK_IOC_DSPVER           _IOR(DK_IOC_MAGIC,  0x20, struct dk_dsp_ver *)
/* send/recv buffer */
#define DK_IOC_SEND             _IOW(DK_IOC_MAGIC,  0x21, struct dk_buff *)
#define DK_IOC_RECV             _IOR(DK_IOC_MAGIC,  0x22, struct dk_buff *)
/* reset dsp/link */
#define DK_IOC_RSTDSP           _IOW(DK_IOC_MAGIC,  0x23, int)
#define DK_IOC_RSTLIK           _IOW(DK_IOC_MAGIC,  0x24, int)
/* rd/wr memory */
#define DK_IOC_RDMEM            _IOR(DK_IOC_MAGIC,  0x25, struct dk_mem *)
#define DK_IOC_WRMEM            _IOW(DK_IOC_MAGIC,  0x26, struct dk_mem *)
/* rd/wr register */
#define DK_IOC_RDREG            _IOR(DK_IOC_MAGIC,  0x27, struct dk_mem *)
#define DK_IOC_WRREG            _IOW(DK_IOC_MAGIC,  0x28, struct dk_mem *)
/* verify buffer */
#define DK_IOC_VERIFY           _IOR(DK_IOC_MAGIC,  0x29, struct dk_verify *)
/* boot memory */
#define DK_IOC_BOOTM            _IOW(DK_IOC_MAGIC,  0x2A, struct dk_bootm *)
/* flash sets,erase/read/write */
#define DK_IOC_ERFLASH          _IOW(DK_IOC_MAGIC,  0x2B, struct dk_flasherase *)
#define DK_IOC_RDFLASH          _IOR(DK_IOC_MAGIC,  0x2C, struct dk_flashdata *)
#define DK_IOC_WRFLASH          _IOW(DK_IOC_MAGIC,  0x2D, struct dk_flashdata *)
/* led sets,set/get */
#define DK_IOC_GETLED           _IOR(DK_IOC_MAGIC,  0x2E, struct dk_led *)
#define DK_IOC_SETLED           _IOW(DK_IOC_MAGIC,  0x2F, struct dk_led *)
/* lcd sets */
#define DK_IOC_FBFLUSH          _IOW(DK_IOC_MAGIC,  0x30, struct dk_fillrect *)
/* vpm sets */
#define DK_IOC_VPM              _IOWR(DK_IOC_MAGIC, 0x31, struct dk_vpm *)
/* lcd sets */
#define DK_IOC_FSCREEN          _IOR(DK_IOC_MAGIC,  0x32, struct dk_fscreen *)

/*max*/
#define DK_IOC_MAXNR            0x40

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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/fb.h>
/************************************************************************/
/*                                                                      */
/************************************************************************/
struct dk_record_hdr
{
#define DK_RECORD_TYPE_C55OC54_DSP  0x00
#define DK_RECORD_TYPE_C55AC54_DSP  0x01
#define DK_RECORD_TYPE_C6X_DSP      0x02
#define DK_RECORD_TYPE_END_RECORD   0xff
    unsigned char  type;    /* record type */
    unsigned int   addr;    /* record address */
    unsigned short length;  /* record length */
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  dk_helper_memory_common(int fd, unsigned int cmd, unsigned int bitw, unsigned int addr, void*buf, unsigned int count)
{
    if (buf && count>0)
    {
        unsigned int offset;
        char *data = (char*)buf;

        for (offset=0; offset<count; )
        {
            struct dk_mem mem;
            unsigned int  len;
            unsigned int  remain;
            int           retry;
            int           retval;

            remain = count - offset;
            len    = remain > DK_PAYLOAD_MAX ? DK_PAYLOAD_MAX : remain;

            mem.width  = bitw;
            mem.start  = addr + offset;
            mem.memory = data + offset;
            mem.length = len;

            retry = 3;
            do
            {
                retval = ioctl(fd, cmd, &mem);
            } while (retval<0 && retry--);
            
            if (retval < 0)
                return -EFAULT;

            offset += len;
        }

        return 0;
    }

    return -EFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int dk_helper_flash_common(int fd, unsigned int cmd, unsigned int addr, void*buf, unsigned int count)
{
    if (buf && count>0)
    {
        unsigned int offset;
        char *data = (char*)buf;

        for (offset=0; offset<count; )
        {
            struct dk_flashdata fdata;
            unsigned int  len;
            unsigned int  remain;
            int           retry;
            int           retval;

            remain = count - offset;
            len    = remain > DK_PAYLOAD_MAX ? DK_PAYLOAD_MAX : remain;

            fdata.start  = addr + offset;
            fdata.data   = data + offset;
            fdata.length = len;

            retry = 3;
            do
            {
                retval = ioctl(fd, cmd, &fdata);
            } while (retval<0 && retry--);

            if (retval < 0)
                return -EFAULT;

            offset += len;
        }

        return 0;
    }

    return -EFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  dk_helper_open(const char *path)
{
    int retry=300;
    int fd;

    while (retry--)
    {
        fd = open(path, O_RDWR);
        if (fd > 0)
            return fd;
        usleep(1000*10);
    }

    return -EFAULT;
}

static inline void dk_helper_close(int fd)
{
    if (fd > 0)
        close(fd);
}

static inline void*dk_helper_mmap(int fd, size_t size)
{
    void *start;

    start = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (start==NULL || start==MAP_FAILED)
        return NULL;

    return start;
}

static inline void dk_helper_munmap(void *start, size_t size)
{
    if (start)
        munmap(start, size);
}

static inline int  dk_helper_read(int fd, void *buf, size_t count)
{
    return read(fd, buf, count);
}

static inline int  dk_helper_write(int fd, const void *buf, size_t count)
{
    return write(fd, buf, count);
}

static inline int  dk_helper_start(int fd)
{
    return ioctl(fd, DK_IOC_START, 0);
}

static inline int  dk_helper_drvver(int fd, struct dk_drv_ver *version)
{
    if (version)
    {
        return ioctl(fd, DK_IOC_DRVVER, version);
    }

    return -EFAULT;
}
static inline int  dk_helper_dspver(int fd, struct dk_dsp_ver *version)
{
    if (version)
    {
        return ioctl(fd, DK_IOC_DSPVER, version);
    }

    return -EFAULT;
}
static inline int  dk_helper_send(int fd, const void*buf, unsigned int count)
{
    if (buf && count>0)
    {
        struct dk_buff buff;

        buff.buffer = (char*)buf;
        buff.length = count;

        return ioctl(fd, DK_IOC_SEND, &buff);
    }

    return -EFAULT;
}
static inline int  dk_helper_recv(int fd, void*buf, unsigned int count)
{
    if (buf && count>0)
    {
        struct dk_buff buff;

        buff.buffer = (char*)buf;
        buff.length = count;

        return ioctl(fd, DK_IOC_RECV, &buff);
    }

    return -EFAULT;
}
static inline int  dk_helper_reset_dsp(int fd, int ms)
{
    if (ms<=0)
        ms = 10;

    return ioctl(fd, DK_IOC_RSTDSP, ms);
}
static inline int  dk_helper_reset_link(int fd, int ms)
{
    if (ms<=0)
        ms = 10;

    return ioctl(fd, DK_IOC_RSTLIK, ms);
}
static inline int  dk_helper_memory_read(int fd, unsigned int bitw, unsigned int addr, void*buf, unsigned int count)
{
    return dk_helper_memory_common(fd, DK_IOC_RDMEM, bitw, addr, buf, count);
}
static inline int  dk_helper_memory_write(int fd, unsigned int bitw, unsigned int addr, const void*buf, unsigned int count)
{
    return dk_helper_memory_common(fd, DK_IOC_WRMEM, bitw, addr, (void*)buf, count);
}
static inline int  dk_helper_register_read(int fd, unsigned int bitw, unsigned int addr, void*buf, unsigned int count)
{
    return dk_helper_memory_common(fd, DK_IOC_RDREG, bitw, addr, buf, count);
}
static inline int  dk_helper_register_write(int fd, unsigned int bitw, unsigned int addr, const void*buf, unsigned int count)
{
    return dk_helper_memory_common(fd, DK_IOC_WRREG, bitw, addr, (void*)buf, count);
}
static inline int  dk_helper_verify(int fd, unsigned int type,unsigned int addr, unsigned int size, unsigned int check)
{
    struct dk_verify vfy;

    vfy.type   = type;
    vfy.start  = addr;
    vfy.length = size;
    vfy.check  = check;

    return ioctl(fd, DK_IOC_VERIFY, &vfy);
}
static inline int  dk_helper_bootm(int fd, unsigned int addr, int argc, const char*argv)
{
    struct dk_bootm boot;

    boot.start = addr;
    boot.argc  = argc;
    boot.argv  = argv;
    boot.size  = argv ? strlen(argv) : 0;

    return ioctl(fd, DK_IOC_BOOTM, &boot);
}
static inline int  dk_helper_flash_erase(int fd, unsigned int addr, unsigned int size)
{
    struct dk_flasherase erase;
    unsigned int erase_len;
    int retval;

    /* size has already align 4096 */
    while(size > 0)
    {
        if(size > 4096)
            erase_len = 4096;

        erase.start = addr;
        erase.length= erase_len;

        retval = ioctl(fd, DK_IOC_ERFLASH, &erase);

        if(retval<0)
            return retval;

        size -= erase_len;
        addr += erase_len;
    }

    return 0;
}
static inline int  dk_helper_flash_read(int fd, unsigned int addr, void*buf, unsigned int count)
{
    return dk_helper_flash_common(fd, DK_IOC_RDFLASH, addr, buf, count);
}

static inline int  dk_helper_flash_write(int fd, unsigned int addr, const void*buf, unsigned int count)
{
    return dk_helper_flash_common(fd, DK_IOC_WRFLASH, addr, (void*)buf, count);
}

static inline int  dk_helper_led_get(int fd, unsigned int*mask)
{
    struct dk_led led;

    if (mask)
    {
        if (ioctl(fd, DK_IOC_GETLED, &led) < 0)
            return -EFAULT;

        *mask = led.mask;
        return 0;
    }

    return -EFAULT;
}

static inline int  dk_helper_led_set(int fd, unsigned int mask)
{
    struct dk_led led;

    led.mask = mask;

    return ioctl(fd, DK_IOC_SETLED, &led);
}

static inline int  dk_helper_vpm(int fd, const void*buf, unsigned int buf_count, void*ret, unsigned int ret_count)
{
    struct dk_vpm vpm;

    vpm.buffer = (char*)buf;
    vpm.length = buf_count;
    vpm.retbuf = (char*)ret;
    vpm.retlen = ret_count;

    return ioctl(fd, DK_IOC_VPM, &vpm);
}

static inline int dk_helper_fb_fscreeninfo(int fd,struct fb_fix_screeninfo *finfo)
{
    struct dk_fscreen fscreen;

    if (!finfo)
        return -EFAULT;

    if (ioctl(fd, DK_IOC_FSCREEN, &fscreen) < 0)
        return -EFAULT;

    finfo->smem_start = fscreen.smem_start;
    finfo->smem_len   = fscreen.smem_len;

    return 0;
}

static inline int dk_helper_fb_vscreeninfo(int fd, struct fb_var_screeninfo *vinfo)
{
    struct dk_fscreen fscreen;

    if (!vinfo)
        return -EFAULT;

    if (ioctl(fd, DK_IOC_FSCREEN, &fscreen) < 0)
        return -EFAULT;

    vinfo->bits_per_pixel = fscreen.bits_per_pixel;
    vinfo->height         = fscreen.height;
    vinfo->width          = fscreen.width;

    return 0;
}

static inline int  dk_helper_fb_flush(int fd)
{
    return ioctl(fd, DK_IOC_FBFLUSH, NULL);
}

static inline int  dk_helper_fb_flush2(int fd,unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
    struct dk_fillrect rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return ioctl(fd, DK_IOC_FBFLUSH, &rect);
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
#endif //WIN32
#endif /*__DSPLINK_API_H__*/
