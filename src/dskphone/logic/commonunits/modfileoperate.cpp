
#include "modfileoperate.h"

#include<stdio.h>
#include<stdlib.h>

#include <ETLLib.hpp>

#include<sys/types.h>
#include<sys/fcntl.h>
#include<unistd.h>

/** Base64 encoding routines. Implemented according to RFC 3548 */

/** map of all byte values to the base64 values, or to
    '65' which indicates an invalid character. '=' is '64' */
static const char _base64_invcharmap[256] =
{
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 62, 65, 65, 65, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 65, 65, 65, 64, 65, 65,
    65, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 65, 65, 65, 65, 65,
    65, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65
};

/** map of all 6-bit values to their corresponding byte
    in the base64 alphabet. Padding char is the value '64' */
static const char _base64_charmap[65] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
    '='
};

int     base64_encoded_len(const unsigned len)
{
    /* encoded steam is 4 bytes for every three, rounded up */
    return ((len + 2) / 3) << 2;
}

unsigned remove_newline(char * buffer, unsigned buffer_len)
{
    char  *  p1, *p2;

    p1 = p2 = buffer;

    while (p2 < buffer + buffer_len)
    {
        if (*p2 == '\n')
        {
            p2++;
            continue;
        }
        *p1++ = *p2++;
    }

    return (p1 - buffer);   /* return the new length */
}

typedef unsigned int    uint32_t;

int base64_encode(const unsigned char * const buffer, const unsigned len, char * out)
{
    int clen;
    char * cbuf, *c;
    uint32_t word, hextet;
    int i;

    clen = base64_encoded_len(len);
    cbuf = out;
    //cbuf = malloc(clen + 1);

    if (cbuf != NULL)
    {
        c = cbuf;
        /* loop over data, turning every 3 bytes into 4 characters */
        for (i = 0; i < len - 2; i += 3)
        {
            word = buffer[i] << 16 | buffer[i + 1] << 8 | buffer[i + 2];
            hextet = (word & 0x00FC0000) >> 18;
            *c++ = _base64_charmap[hextet];
            hextet = (word & 0x0003F000) >> 12;
            *c++ = _base64_charmap[hextet];
            hextet = (word & 0x00000FC0) >> 6;
            *c++ = _base64_charmap[hextet];
            hextet = (word & 0x000003F);
            *c++ = _base64_charmap[hextet];
        }
        /* zero, one or two bytes left */
        switch (len - i)
        {
        case 0:
            break;
        case 1:
            hextet = (buffer[len - 1] & 0xFC) >> 2;
            *c++ = _base64_charmap[hextet];
            hextet = (buffer[len - 1] & 0x03) << 4;
            *c++ = _base64_charmap[hextet];
            *c++ = _base64_charmap[64]; /* pad */
            *c++ = _base64_charmap[64]; /* pad */
            break;
        case 2:
            hextet = (buffer[len - 2] & 0xFC) >> 2;
            *c++ = _base64_charmap[hextet];
            hextet = ((buffer[len - 2] & 0x03) << 4) |
                     ((buffer[len - 1] & 0xF0) >> 4);
            *c++ = _base64_charmap[hextet];
            hextet = (buffer[len - 1] & 0x0F) << 2;
            *c++ = _base64_charmap[hextet];
            *c++ = _base64_charmap[64]; /* pad */
            break;
        }
        /* add a terminal null */
        *c = '\0';
    }

    return clen;
}

int     base64_decoded_len(const char * const buffer, const unsigned len)
{
    int nudge;
    int c;

    /* count the padding characters for the remainder */
    nudge = -1;
    c = _base64_invcharmap[(int)buffer[len - 1]];
    if (c < 64)
    {
        nudge = 0;
    }
    else if (c == 64)
    {
        c = _base64_invcharmap[(int)buffer[len - 2]];
        if (c < 64)
        {
            nudge = 1;
        }
        else if (c == 64)
        {
            c = _base64_invcharmap[(int)buffer[len - 3]];
            if (c < 64)
            {
                nudge = 2;
            }
        }
    }
    if (nudge < 0)
    {
        return 0;    /* reject bad coding */
    }

    /* decoded steam is 3 bytes for every four */
    return 3 * (len >> 2) - nudge;
}

int base64_decode(const char * const buffer, const unsigned len, unsigned char * out)
{
    int dlen;
    unsigned char * dbuf = NULL, *d = NULL;
    uint32_t word, hextet;
    int i;

    /* len must be a multiple of 4 */
    if (len & 0x03)
    {
        return -1;
    }

    dlen = base64_decoded_len(buffer, len);
    dbuf = out;
    //dbuf = malloc(dlen + 1);

    if (dbuf != NULL)
    {
        d = dbuf;
        /* loop over each set of 4 characters, decoding 3 bytes */
        for (i = 0; i < len - 3; i += 4)
        {
            hextet = _base64_invcharmap[(int)buffer[i]];
            if (hextet & 0xC0)
            {
                break;
            }
            word = hextet << 18;
            hextet = _base64_invcharmap[(int)buffer[i + 1]];
            if (hextet & 0xC0)
            {
                break;
            }
            word |= hextet << 12;
            hextet = _base64_invcharmap[(int)buffer[i + 2]];
            if (hextet & 0xC0)
            {
                break;
            }
            word |= hextet << 6;
            hextet = _base64_invcharmap[(int)buffer[i + 3]];
            if (hextet & 0xC0)
            {
                break;
            }
            word |= hextet;
            *d++ = (word & 0x00FF0000) >> 16;
            *d++ = (word & 0x0000FF00) >> 8;
            *d++ = (word & 0x000000FF);
        }
        if (hextet > 64)
        {
            goto _base64_decode_error;
        }
        /* handle the remainder */
        switch (dlen % 3)
        {
        case 0:
            /* nothing to do */
            break;
        case 1:
            /* redo the last quartet, checking for correctness */
            hextet = _base64_invcharmap[(int)buffer[len - 4]];
            if (hextet & 0xC0)
            {
                goto _base64_decode_error;
            }
            word = hextet << 2;
            hextet = _base64_invcharmap[(int)buffer[len - 3]];
            if (hextet & 0xC0)
            {
                goto _base64_decode_error;
            }
            word |= hextet >> 4;
            *d++ = word & 0xFF;
            hextet = _base64_invcharmap[(int)buffer[len - 2]];
            if (hextet != 64)
            {
                goto _base64_decode_error;
            }
            hextet = _base64_invcharmap[(int)buffer[len - 1]];
            if (hextet != 64)
            {
                goto _base64_decode_error;
            }
            break;
        case 2:
            /* redo the last quartet, checking for correctness */
            hextet = _base64_invcharmap[(int)buffer[len - 4]];
            if (hextet & 0xC0)
            {
                goto _base64_decode_error;
            }
            word = hextet << 10;
            hextet = _base64_invcharmap[(int)buffer[len - 3]];
            if (hextet & 0xC0)
            {
                goto _base64_decode_error;
            }
            word |= hextet << 4;
            hextet = _base64_invcharmap[(int)buffer[len - 2]];
            if (hextet & 0xC0)
            {
                goto _base64_decode_error;
            }
            word |= hextet >> 2;
            *d++ = (word & 0xFF00) >> 8;
            *d++ = (word & 0x00FF);
            hextet = _base64_invcharmap[(int)buffer[len - 1]];
            if (hextet != 64)
            {
                goto _base64_decode_error;
            }
            break;
        }
    }
    *d = '\0';
    return dlen;

_base64_decode_error:
    /* invalid character; abort decoding! */
    return -1;
}

int file_read(const char * filename, char ** buffer,
              size_t * buffer_len)
{
    int fd = -1;
    int nread = 0, n = 0;
    char  *  buf = NULL;
    size_t  buf_len = 0;

    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        goto read_error;
    }

    buf_len = 4096;
    buf = static_cast<char *>(malloc(buf_len)); /* default size */

    nread = 0;
    while (1)
    {
        /* expand if necessary */
        if (nread >= buf_len)
        {
            buf_len = 2 * buf_len;
            buf = static_cast<char *>(realloc(buf, buf_len)); //TODO. 内存泄露
        }

        n = read(fd, buf + nread, buf_len - nread);
        if (n < 0)
        {
            goto read_error;
        }
        if (n == 0)
        {
            break;
        }
        nread += n;
    }
    buf[nread] = 0;

    *buffer = buf;
    *buffer_len = nread;
    close(fd);
    return nread;

read_error:
    if (fd > 0)
    {
        close(fd);
    }
    if (buf)
    {
        free(buf);
    }
    *buffer = NULL;
    *buffer_len = 0;
    return -1;
}

int file_write(const char * filename, const char * buffer,
               size_t buffer_len)
{
    int fd = -1;
    int nremain = 0, nwritten = 0, n = 0;

    /* remove first, ignore errors */
    remove(filename);

    if ((fd = open(filename, O_CREAT | O_RDWR, 0755)) < 0)
    {
        perror(filename);
        goto write_error;
    }

    nremain = buffer_len;
    nwritten = 0;
    while (1)
    {
        if ((n = write(fd, buffer + nwritten, nremain)) < 0)
        {
            perror("write");
            goto write_error;
        }

        if (n == 0)
        {
            break;
        }

        nremain -= n;
        nwritten += n;
    }

    close(fd);
    return nwritten;

write_error:
    if (fd > 0)
    {
        close(fd);
    }
    return nwritten;
}
