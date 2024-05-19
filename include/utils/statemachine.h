#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define FLAG 0x5C
#define ESCAPE 0x5D
#define A_SENDER_RECEIVER 0x01
#define A_RECEIVER_SENDER 0x03
#define C_SET 0x03
#define C_UA 0x06


extern volatile int STOP;

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP_STATE,
    DATA_RCV,
    ESCAPE_RCV
} State;

/**
 * @function char compute_bcc(char a, char c)
 * @brief Computes the Block Check Character (BCC) for two characters.
 *
 * This function computes the BCC by performing a bitwise XOR operation on
 * the two input characters. The BCC is used for error detection in data
 * transmission.
 *
 * @param[in] a The first character.
 * @param[in] c The second character.
 * @return The computed BCC.
 */
char compute_bcc(char a, char c);

/**
 * @function void parse_frame(int fd, char c)
 * @brief Parses a frame of data and writes it to a file descriptor.
 *
 * This function implements a state machine to parse a frame of data. It reads
 * the input character by character and changes the state of the machine
 * accordingly. When a complete frame has been read (indicated by the FLAG
 * character), it writes the frame to the given file descriptor.
 *
 * @param[in] fd The file descriptor to write the frame to.
 * @param[in] c The next character in the frame to parse.
 */
void parse_frame(int fd, char c);

#endif // STATEMACHINE_H