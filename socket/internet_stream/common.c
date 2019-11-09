/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

#include "common.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

/* Read characters from 'fd' until a newline is encountered. If a newline
  character is not encountered in the first (n - 1) bytes, then the excess
  characters are discarded. The returned string placed in 'p' is
  null-terminated and includes the newline character if it was read in the
  first (n - 1) bytes. The function return value is the number of bytes
  placed in buffer (which includes the newline character if encountered,
  but excludes the terminating null byte). */
ssize_t readLine(int fd, void* buffer, size_t n)
{
    if (n <= 0 || buffer == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    char ch;
    ssize_t readSize = 0;
    size_t totalReadSize = 0;
    char* p = (char*)buffer;
    for (;;)
    {
        readSize = read(fd, &ch, 1);
        if (readSize == -1)
        {
            if (errno == EINTR)
                /* Interrupted --> restart read() */
                continue;
            else
                /* Some other error */
                return -1;
        }
        else if (readSize == 0)
        {
            /* EOF */
            if (totalReadSize == 0)
                /* No bytes read; return 0 */
                return 0;
            else
                /* Some bytes read; add '\0' */
                break;
        }
        else
        {
            /* 'readSize' must be 1 if we get here */
            if (totalReadSize < n - 1)
            {
                /* Discard > (n - 1) bytes */
                totalReadSize++;
                *p++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *p = 0;
    return totalReadSize;
}

void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
