#ifndef DATALINK_H
#define DATALINK_H
#include <assert.h>
#include "utils/statemachine.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


void build_frame(int fd, char* buf, int size);

#endif // DATALINK_H