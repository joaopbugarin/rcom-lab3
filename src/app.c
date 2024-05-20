#include "app.h"

#define MAX_RETRANSMISSIONS 3
#define TIMEOUT 3

void llopen(const char* port, int* fd){
    struct termios oldtio,newtio;
    *fd = open(port, O_RDWR | O_NOCTTY );

	if (*fd <0) {perror(port); exit(-1); }

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

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;  
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

}


int llclose (int fd){
    struct termios oldtio;
	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);
    return 0;
}




void run_transmitter(int argc, char** argv) {
    // File descriptor for sending data. Replace with actual FD.
    int fd = 0;

    // Path to the file to be sent. Replace with actual path.
    const char* filename = argv[3];

    // Send the file.
    send_file(fd, filename);
}

void run_receiver(int argc, char** argv) {
    // File descriptor for receiving data. Replace with actual FD.
    int fd = 0;

    // Path to the output file. Replace with actual path.
    const char* output_filename = 'testing/'+ argv[3];

    // Receive the file.
    get_file(fd, output_filename);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage:\tnserial SerialPort Mode Filename\n\tex: nserial /dev/ttyS1 Tx file.txt\n");
        exit(1);
    }

    if (strcmp(argv[2], "Tx") != 0 && strcmp(argv[2], "Rx") != 0) {
        printf("Invalid mode. Use 'Tx' for transmitter mode or 'Rx' for receiver mode.\n");
        exit(1);
    }

    if (strcmp(argv[2], "Tx") == 0) {
        llopen(argv[1],0);
        run_transmitter(argc, argv);
    } else {
        llopen(argv[1],0);
        run_receiver(argc, argv);
    }

    return 0;
}