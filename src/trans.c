#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define ESCAPE 0x7D
#define A_TRANSMITTER_COMMAND 0x03
#define C_SET 0x03
#define MAX_RETRANSMISSIONS 3
#define TIMEOUT 3

volatile int STOP=FALSE;

void build_frame(int fd, char* buf, int size) {
    unsigned char frame[512]; // Assuming maximum frame size is 255 bytes before stuffing.
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

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int tries = 0;
    int i, sum = 0, speed = 0;

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    while (STOP==FALSE && tries < MAX_RETRANSMISSIONS) {       /* loop for input */

        // Test byte stuffing
        char test_data1[] = {'a', 'b', FLAG, 'd', 'e'};
        build_frame(fd, test_data1, sizeof(test_data1));

        sleep(1); // Wait for receiver to process the frame

        // Test error checking
        char test_data2[] = {'a', 'b', 'c', 'd', 'e'}; // Incorrect BCC
        build_frame(fd, test_data2, sizeof(test_data2));

        sleep(1); // Wait for receiver to process the frame

        // for (int i = 0; i < 255; i++) {
        //     buf[i] = 'a';
        // }

        // /*testing*/
        // buf[25] = '\n';

        build_frame(fd, buf, 255);

        alarm(TIMEOUT); // Set the timeout
        STOP = read(fd,buf,255); // Read the UA response

        if (STOP == FALSE) {
            tries++;
        }
    }

    if (tries == MAX_RETRANSMISSIONS) {
        printf("Failed to send frame after %d attempts\n", tries);
    }

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);
    return 0;
}
