#include "datalink.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define BAUDRATE B38400
#define CHUNK_SIZE 1024

void build_frame(int fd, unsigned char* buf, int size) {
    unsigned char frame[5 + size];
    frame[0] = FLAG;
    frame[1] = A_RECEIVER_SENDER;
    frame[2] = C_SET;
    frame[3] = frame[1] ^ frame[2];
    memcpy(&frame[4], buf, size);
    frame[4 + size] = FLAG;
    write(fd, frame, 5 + size);
}

unsigned char* perform_byte_stuffing(unsigned char* data_array, int* data_size) {
    unsigned char* stuffed_msg;
    int index = 0;
    int output_index = 0;
    unsigned int buffer_size = *data_size;
    stuffed_msg = (unsigned char *) malloc(buffer_size);

    for(; index < *data_size; index++, output_index++) {
        if(output_index >= buffer_size) {
            buffer_size += buffer_size / 2;
            stuffed_msg = (unsigned char*) realloc(stuffed_msg, buffer_size);
        }

        switch(data_array[index]) {
            case 0x7e:
                stuffed_msg[output_index] = 0x7d;
                stuffed_msg[++output_index] = 0x5e;
                break;
            case 0x7d:
                stuffed_msg[output_index] = 0x7d;
                stuffed_msg[++output_index] = 0x5d;
                break;
            default:
                stuffed_msg[output_index] = data_array[index];
                break;
        }
    }

    *data_size = output_index;
    free(data_array);

    return stuffed_msg;
}

unsigned char* perform_byte_destuffing(unsigned char* data_array, int* data_size) {
    unsigned int buffer_size = 133;
    unsigned char* destuffed_msg = (unsigned char*) malloc(buffer_size);
    int index = 0;
    int output_index = 0;

    for(; index < *data_size; index++) {
        output_index++;
        if(output_index >= buffer_size) {
            buffer_size += buffer_size / 2;
            destuffed_msg = (unsigned char *) realloc(destuffed_msg, buffer_size);
        }

        if(data_array[index] == 0x7d) {
            switch(data_array[++index]) {
                case 0x5e:
                    destuffed_msg[output_index - 1] = 0x7e;
                    break;
                case 0x5d:
                    destuffed_msg[output_index - 1] = 0x7d;
                    break;
            }
        } else {
            destuffed_msg[output_index - 1] = data_array[index];
        }
    }

    *data_size = output_index;
    free(data_array);

    return destuffed_msg;
}

void send_file(int fd, const char* filename) {
    FILE* file = fopen(filename, "rb");
    char buf[CHUNK_SIZE];

    size_t n;
    while ((n = fread(buf, 1, CHUNK_SIZE, file)) > 0) {
        int size = n;
        char* stuffed_buf = perform_byte_stuffing(buf, &size);
        build_frame(fd, stuffed_buf, size);
        free(stuffed_buf);
    }

    fclose(file);
}

unsigned char* read_frame(int fd, int* size) {
    unsigned char frame[CHUNK_SIZE];
    *size = read(fd, frame, sizeof(frame));
    // If the read failed or read zero bytes, return NULL.
    if (*size <= 0) {
        return NULL;
    }
    unsigned char* data = perform_byte_destuffing(frame, size);
    return data;
}

void get_file(int fd, const char* output_filename) {
    // Open the output file.
    FILE* file = fopen(output_filename, "wb");
    if (file == NULL) {
        perror("Failed to open output file");
        return;
    }

    int size;
    unsigned char* data;

    // Loop until there is no more data to read.
    while ((data = read_frame(fd, &size)) != NULL) {
        // Write the received data to the output file.
        fwrite(data, 1, size, file);

        // Free the received data when done.
        free(data);
    }

    // Close the output file.
    fclose(file);
}

void serial_open(const char* port, int* fd){
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

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

}


int serial_close (int fd){
    struct termios oldtio;
	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);
    return 0;
}

