#ifndef DATALINK_H
#define DATALINK_H
#include <assert.h>
#include "utils/statemachine.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


unsigned char* perform_byte_stuffing(unsigned char* data_array, int* data_size);
unsigned char* perform_byte_destuffing(unsigned char* data_array, int* data_size);
void build_frame(int fd, unsigned char* buf, int size, char type);
unsigned char* read_frame(int fd, int* size);
void get_file(int fd, const char* output_filename);
void send_file(int fd, const char* filename);



#endif // DATALINK_H