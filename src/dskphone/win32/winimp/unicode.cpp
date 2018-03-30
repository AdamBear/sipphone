#include "common.h"

typedef unsigned char UCHAR;
typedef int           INT;
typedef unsigned short utf16; // 16 bits
typedef UCHAR utf8; // 8 bits
typedef UCHAR ubyte;

enum u78 {utf8NoBOM = 0, ascii7bits = 1, ascii8bits = 2};

static const unsigned char k_Boms[uniEnd][4] =
{
    {0x00, 0x00, 0x00, 0x00},  // Unknown
    {0xEF, 0xBB, 0xBF, 0x00},  // UTF8
    {0xFE, 0xFF, 0x00, 0x00},  // Big endian
    {0xFF, 0xFE, 0x00, 0x00},  // Little endian
};

INT uniTest = IS_TEXT_UNICODE_STATISTICS;

static u78 utf8_7bits_8bits(const unsigned char * buf, int bufLen)
{
    int rv = 1;
    int ASCII7only = 1;
    utf8 * sx    = (utf8 *)buf;
    utf8 * endx  = sx + bufLen;

    while (sx < endx)
    {
        if (!*sx)
        {
            // For detection, we'll say that NUL means not UTF8
            ASCII7only = 0;
            rv = 0;
            break;
        }
        else if (*sx < 0x80)
        {
            // 0nnnnnnn If the byte's first hex code begins with 0-7, it is an ASCII character.
            ++sx;
        }
        else if (*sx < (0x80 + 0x40))
        {
            // 10nnnnnn 8 through B cannot be first hex codes
            ASCII7only = 0;
            rv = 0;
            break;
        }
        else if (*sx < (0x80 + 0x40 + 0x20))
        {
            // 110xxxvv 10nnnnnn  If it begins with C or D, it is an 11 bit character
            ASCII7only = 0;
            if (sx >= endx - 1)
            {
                break;
            }
            if ((*sx & 0xC0) != 0xC0 || (sx[1] & (0x80 + 0x40)) != 0x80)
            {
                rv = 0;
                break;
            }
            sx += 2;
        }
        else if (*sx < (0x80 + 0x40 + 0x20 + 0x10))
        {
            // 1110qqqq 10xxxxvv 10nnnnnn If it begins with E, it is 16 bit
            ASCII7only = 0;
            if (sx >= endx - 2)
            {
                break;
            }
            if ((*sx & 0xE0) != 0xE0 || (sx[1] & (0x80 + 0x40)) != 0x80 || (sx[2] & (0x80 + 0x40)) != 0x80)
            {
                rv = 0;
                break;
            }
            sx += 3;
        }
        else
        {
            // more than 16 bits are not allowed here
            ASCII7only = 0;
            rv = 0;
            break;
        }
    }
    if (ASCII7only)
    {
        return ascii7bits;
    }
    if (rv)
    {
        return utf8NoBOM;
    }
    return ascii8bits;
}

UniMode determineEncoding(const unsigned char * buf, int bufLen)
{
    UniMode eEncoding = uni8Bit;

    // detect UTF-16 big-endian with BOM
    if (bufLen > 1 && buf[0] == k_Boms[uni16BE][0] && buf[1] == k_Boms[uni16BE][1])
    {
        eEncoding = uni16BE;
    }
    // detect UTF-16 little-endian with BOM
    else if (bufLen > 1 && buf[0] == k_Boms[uni16LE][0] && buf[1] == k_Boms[uni16LE][1])
    {
        eEncoding = uni16LE;
    }
    // detect UTF-8 with BOM
    else if (bufLen > 2 && buf[0] == k_Boms[uniUTF8][0] &&
             buf[1] == k_Boms[uniUTF8][1] && buf[2] == k_Boms[uniUTF8][2])
    {
        eEncoding = uniUTF8;
    }
    /* should not use uni16LE_NoBOM and uni16BE_NoBOM in yealink phone system */
    // try to detect UTF-16 little-endian without BOM
    else if (bufLen > 1 && buf[0] != NULL && buf[1] == NULL && IsTextUnicode(buf, bufLen, &uniTest))
    {
        eEncoding = uni16LE_NoBOM;
    }
    /* UTF-16 big-endian without BOM detection is taken away scince this detection is very week */
    // try to detect UTF-16 big-endian without BOM
    else if (bufLen > 1 && buf[0] == NULL && buf[1] != NULL)
    {
        eEncoding = uni16BE_NoBOM;
    }
    else
    {
        u78 detectedEncoding = utf8_7bits_8bits(buf, bufLen);
        if (detectedEncoding == utf8NoBOM)
        {
            eEncoding = uniCookie;
        }
        else if (detectedEncoding == ascii7bits)
        {
            eEncoding = uni7Bit;
        }
        else //(detectedEncoding == ascii8bits)
        {
            eEncoding = uni8Bit;
        }
    }

    return eEncoding;
}

unsigned int UCS2Length(const char * s, unsigned int len)
{
    unsigned int ulen = 0;
    for (unsigned int i = 0; i < len; ++i)
    {
        UCHAR ch = static_cast<UCHAR>(s[i]);
        if ((ch < 0x80) || (ch > (0x80 + 0x40)))
        {
            ++ulen;
        }
    }
    return ulen;
}

unsigned int UTF8Length(const wchar_t * uptr, unsigned int tlen)
{
    unsigned int len = 0;
    for (unsigned int i = 0; i < tlen && uptr[i]; ++i)
    {
        unsigned int uch = uptr[i];
        if (uch < 0x80)
        {
            ++len;
        }
        else if (uch < 0x800)
        {
            len += 2;
        }
        else
        {
            len += 3;
        }
    }
    return len;
}

// read one block and decision
UniMode determineFileEncode(const char * path)
{
    if (path == NULL)
    {
        return uniUnkown;
    }

    std::string data;
    dump_file_imp(path, data, 0, 4 * 1024);
    return determineEncoding((const unsigned char *)data.c_str(), data.size());
}

unsigned int bomLength(const unsigned char * buf, int bufLen)
{
    if (buf == NULL || bufLen == 0)
    {
        return 0;
    }

    UniMode mode = determineEncoding(buf, bufLen);
    if (mode >= uniUTF8 && mode <= uni16LE)
    {
        return strlen((char *)k_Boms[mode]);
    }
    return 0;
}

