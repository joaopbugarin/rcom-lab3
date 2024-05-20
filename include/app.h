#ifndef APP_H
#define APP_H

#include "datalink.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void llopen(const char* port, int* fd);
int llclose(int fd);

#endif // APP_H