#include "app.h"

#define MAX_RETRANSMISSIONS 3
#define TIMEOUT 3

void run_transmitter(int argc, char** argv) {
    int fd, res;
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


    buf[0] = FLAG;
    buf[1] = A_RECEIVER_SENDER;
    buf[2] = C_SET;
    buf[3] = buf[1] ^ buf[2];
    buf[4] = FLAG;

    res = write(fd,buf,255);
    printf("%d bytes written\n", res);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }


    close(fd);
}

void run_receiver(int argc, char** argv) {
    int fd, res;
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
        res = read(fd,buf,1);
        buf[res]=0;               /* so we can printf... */
        printf(":%s:%d\n", buf, res);

        for (int i = 0; i < res; i++) {
            parse_frame(fd, buf[i]);
        }
    }


    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
}

int main(int argc, char** argv)
{
    // Check if the correct number of arguments is provided
    if (argc < 3) {
        printf("Usage:\tnserial SerialPort Mode\n\tex: nserial /dev/ttyS1 Tx\n");
        exit(1);
    }

    // Check if the mode argument is valid
    if (strcmp(argv[2], "Tx") != 0 && strcmp(argv[2], "Rx") != 0) {
        printf("Invalid mode. Use 'Tx' for transmitter mode or 'Rx' for receiver mode.\n");
        exit(1);
    }

    // Run in transmitter mode if the mode argument is "Tx", otherwise run in receiver mode
    if (strcmp(argv[2], "Tx") == 0) {
        run_transmitter(argc, argv);
    } else {
        run_receiver(argc, argv);
    }

    return 0;
}
