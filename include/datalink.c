#include "datalink.h"

void build_frame(int fd, char* buf, int size) {
    unsigned char frame[300]; // Assuming maximum frame size is 255 bytes before stuffing. Additional bytes to accomodate extra stuffing.
    memset(frame, 0, sizeof(frame));

    frame[0] = FLAG;
    frame[1] = A_TRANSMITTER_COMMAND;
    frame[2] = C_SET;
    frame[3] = frame[1] ^ frame[2];

    int j = 4;
    for (int i = 0; i < size; i++) {
        if (buf[i] == FLAG || buf[i] == ESCAPE) {
            frame[j++] = ESCAPE;
            frame[j++] = buf[i] ^ 0x20;
        } else {
            frame[j++] = buf[i];
        }
    }

    frame[j++] = FLAG;

    write(fd, frame, j);
}