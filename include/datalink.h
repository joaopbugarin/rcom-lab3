#ifndef DATALINK_H
#define DATALINK_H

#include "utils/statemachine.h"
#define A_TRANSMITTER_COMMAND 0x03
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

volatile int STOP=FALSE;

#endif // DATALINK_H