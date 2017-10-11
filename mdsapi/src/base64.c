#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include <mdsapi/mds_api.h>


static const char b64_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'};

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
int mds_api_b64_encode(const unsigned char *src, int src_size, char *out_buff, int out_buff_len)
{
  int i = 0;
  int j = 0;
  char *enc = NULL;
  int size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  // alloc
  enc = out_buff;
  if (NULL == enc)
  {
    return -1;
  }

  // parse until end of source
  while (src_size--)
  {
    // read up to 3 bytes at a time into `tmp'
    tmp[i++] = *(src++);

    // if 3 bytes read then encode into `buf'
    if (3 == i)
    {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      // allocate 4 new byts for `enc` and
      // then translate each encoded buffer
      // part by index from the base 64 index table
      // into `enc' unsigned char array
      if (out_buff_len < (size + 4) )
      {
        printf("[err] %s (%d) : size [%d] bufflen [%d] \r\n", __func__, __LINE__, size, out_buff_len);
        return -1;
      }

      for (i = 0; i < 4; ++i)
      {
        enc[size++] = b64_table[buf[i]];
      }

      // reset index
      i = 0;
    }
  }

  // remainder
  if (i > 0)
  {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j)
    {
      tmp[j] = '\0';
    }

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j)
    {
      if (out_buff_len < (size + 1) )
      {
        printf("[err] %s (%d) : size [%d] bufflen [%d] \r\n", __func__, __LINE__, size, out_buff_len);
        return -1;
      }
      enc[size++] = b64_table[buf[j]];
    }

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3))
    {
      if (out_buff_len < (size + 1) )
      {
        printf("[err] %s (%d) : size [%d] bufflen [%d] \r\n", __func__, __LINE__, size, out_buff_len);
        return -1;
      }
      enc[size++] = '=';
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  if (out_buff_len < (size + 1) )
  {
    printf("[err] %s (%d) : size [%d] bufflen [%d] \r\n", __func__, __LINE__, size, out_buff_len);
    return -1;
  }
  enc[size] = '\0';

  return size;
}

int mds_api_b64_decode(const char *src, int src_size, unsigned char *out_buff, int out_buff_len)
{
  int i = 0;
  int j = 0;
  int l = 0;
  int size = 0;
  unsigned char *dec = NULL;
  unsigned char buf[3];
  unsigned char tmp[4];

  // alloc
  dec = out_buff;
  if (NULL == dec)
  {
    return -1;
  }

  // parse until end of source
  while (src_size--)
  {
    // break if char is `=' or not base64 char
    if ('=' == src[j])
    {
      break;
    }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j]))
    {
      break;
    }

    // read up to 4 bytes at a time into `tmp'
    tmp[i++] = src[j++];

    // if 4 bytes read then decode into `buf'
    if (4 == i)
    {
      // translate values in `tmp' from table
      for (i = 0; i < 4; ++i)
      {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l)
        {
          if (tmp[i] == b64_table[l])
          {
            tmp[i] = l;
            break;
          }
        }
      }

      // decode
      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      // write decoded buffer to `dec'
      if ( out_buff_len < (size + 3) )
        return -1;

      if (dec != NULL)
      {
        for (i = 0; i < 3; ++i)
        {
          dec[size++] = buf[i];
        }
      }
      else
      {
        return -1;
      }

      // reset
      i = 0;
    }
  }

  // remainder
  if (i > 0)
  {
    // fill `tmp' with `\0' at most 4 times
    for (j = i; j < 4; ++j)
    {
      tmp[j] = '\0';
    }

    // translate remainder
    for (j = 0; j < 4; ++j)
    {
      // find translation char in `b64_table'
      for (l = 0; l < 64; ++l)
      {
        if (tmp[j] == b64_table[l])
        {
          tmp[j] = l;
          break;
        }
      }
    }

    // decode remainder
    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];


    // write decoded buffer to `dec'
    if ( out_buff_len < ( size + (i - 1) ) )
          return -1;
        
          
    if (dec != NULL)
    {
      for (j = 0; (j < i - 1); ++j)
      {
        dec[size++] = buf[j];
      }
    }
    else
    {
      return -1;
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  if ( out_buff_len < ( size + 1 ) )
    return -1;
  if (dec != NULL)
  {
    dec[size] = '\0';
  }
  else
  {
    return -1;
  }

  return size;
}
