#ifndef _MODFILEOPERATE_H
#define _MODFILEOPERATE_H

typedef unsigned int size_t;

int     base64_encoded_len(const unsigned len);
int     base64_decoded_len(const char * const buffer,
                           const unsigned len);

/*
 * @buffer: input binary data
 * @len:    length of buffer
 * @out:    output base64 data, sizeof(out) >= (base64_encoded_len(len)+1)
 *
 * return:  number of bytes encoded in @out
 * error:   no error, always success
 */
int base64_encode(const unsigned char * const buffer,
                  const unsigned len, char * out);



/*
 * @buffer: input base64 data
 * @len:    length of buffer
 * @out:    output binary data, sizeof(out) >= (base64_decoded_len(len)+1)
 *
 * return:  on success: number of bytes decoded in out
 *      on error:   -1
 */
int base64_decode(const char * const buffer,
                  const unsigned len, unsigned char * out);

unsigned remove_newline(char * buffer, unsigned buffer_len);

int file_read(const char * filename, char ** buf, size_t * buf_size);

int file_write(const char * filename, const char * buf, size_t buf_size);

#endif // _MODCOMMON_H
