#include "app.h"

#define MAX_RETRANSMISSIONS 3
#define TIMEOUT 3

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
        serial_open(argv[1],0);
        run_transmitter(argc, argv);
    } else {
        run_receiver(argc, argv);
    }

    return 0;
}