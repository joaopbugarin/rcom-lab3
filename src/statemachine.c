#include "utils/statemachine.h"

State state;
volatile int STOP = FALSE;

char compute_bcc(char a, char c) {
    return a ^ c;
}

void parse_frame(int fd, char c) {
    static char buf[255];
    static int i = 0;
    switch (state) {
        case START:
            if (c == FLAG) {
                state = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (c == A_RECEIVER_SENDER) {
                state = A_RCV;
            } else if (c != FLAG) {
                state = START;
            }
            break;
        case A_RCV:
            if (c == C_SET) {
                state = C_RCV;
            } else {
                state = START;
            }
            break;
        case C_RCV:
            if (compute_bcc(A_RECEIVER_SENDER, C_SET) == c) {
                state = BCC_OK;
            } else {
                state = START;
            }
            break;
        case BCC_OK:
            if (c == FLAG) {
                state = STOP_STATE;
            } else if (c == ESCAPE) {
                state = ESCAPE_RCV;
            } else {
                buf[i++] = c;
                state = DATA_RCV;
            }
            break;
        case DATA_RCV:
            if (c == FLAG) {
                // End of frame, process data
                write(fd, buf, i);
                i = 0;
                state = START;
            } else if (c == ESCAPE) {
                state = ESCAPE_RCV;
            } else {
                buf[i++] = c;
            }
            break;
        case ESCAPE_RCV:
            buf[i++] = c ^ 0x20;
            state = DATA_RCV;
            break;
        case STOP_STATE:
            STOP = TRUE;
            state = START;
            break;
    }
}
