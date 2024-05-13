#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define ESCAPE 0x7D
#define A_RECEIVER_COMMAND 0x03
#define C_SET 0x03

 int STOP=FALSE;

enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP_STATE,
    DATA_RCV,
    ESCAPE_RCV
} state;

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
            if (c == A_RECEIVER_COMMAND) {
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
            if (compute_bcc(A_RECEIVER_COMMAND, C_SET) == c) {
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
            state = START;
            break;
    }
}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */


    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");


    while (STOP==FALSE) {       /* loop for input */
        res = read(fd,buf,255);   /* returns after 5 chars have been input */
        buf[res]=0;               /* so we can printf... */
        printf(":%s:%d\n", buf, res);
        if (buf[0]=='z') STOP=TRUE;

        for (int i = 0; i < res; i++) {
            parse_frame(fd, buf[i]);
        }
    }


    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
