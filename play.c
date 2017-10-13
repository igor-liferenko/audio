/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: serialrecplay.c
  AUTHOR......: David Rowe                                           
  DATE CREATED: 14 Feb 2010
                                                                             
  Full duplex record and play of audio from a serial device, runs on an
  x86 Linux PC.  To make this work properly #define BUF_SZ 1800 was reqd
  in Arduino uart2analog.c, as PC would go away and do other things for a
  while.  This means proper echo cancellation is probably not possible,
  limiting the use of this user-mode program to testing.

  Serial code based on ser.c sample from http://www.captain.at

  Compile with:

    gcc serialrecplay.c -o serialrecplay -Wall -g -O2 -lm

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2011 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <math.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>  
#include <errno.h>  
#include <termios.h>

#define N        160
#define FS       8000
#define PI       3.141593

void write_serial(int fd, char *data, int len) {
    int n;
    //printf("  writing %d bytes\n", len);
    n = write(fd, data, len);
    if (n < 0) {
	perror("write failed");
	exit(1);
    }
}

/* note this only works reliably with len == 1 */

void read_serial(int fd, char *data, int len) {
    int n;
    //printf("  reading %d bytes  \n", len);

    n = read(fd, data, len);
    if (n < 0) {
	perror("read failed");
	exit(1);
    }
    //printf("  read %d bytes\n", len);
}

int main(int argc, char **argv) {
    int    fd;
    FILE  *fplay;
    int    i, len, tone, ret;
    unsigned char rx,tx;

    fplay = fopen("x.raw", "rb");
	if (fplay == NULL) {
	    printf("error opening raw file\n");
	    exit(1);
	}

    /* open and configure serial port */

    fd = open("/dev/avr", O_WRONLY | O_NOCTTY);
    if (fd == -1) {
	perror("open_port: Unable to open /dev/ttyUSB0 - ");
	exit(1);
    }
	
    struct termios terminal;
    tcgetattr(fd, &terminal); /* this is needed even if cfmakeraw is used */
    cfmakeraw(&terminal);
    cfsetispeed(&terminal, B115200);
    cfsetospeed(&terminal, B115200);
    tcsetattr(fd, TCSANOW, &terminal);

    tx = 0;

    while (fread(&tx, 1, 1, fplay))
      write_serial(fd, &tx, 1);

fclose(fplay);
    return 0;
}
