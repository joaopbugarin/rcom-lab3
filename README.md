# Data Communication Protocol Implementation

This project aims to develop and test a data communication protocol. The protocol provides reliable communication between two systems connected by a serial cable. It involves various aspects of data communication including frame synchronization, error robustness, error correction, and stop & wait ARQ protocol. 

The implementation is based on the principles of layering, interfaces, functionality separation, and state machines. It is developed in C programming language and requires competences in Linux environment. The protocol also includes performance evaluation and debugging features.

The project is divided into several parts:

1. Exchange of strings over serial connections.
2. Sending and receiving control frames using a state machine.
3. Implementing stop & wait protocol in llwrite and llread functions.
4. Implementing timer and retransmission logic.
5. Structuring code for llopen, llclose, llwrite, llread functions and separating header and implementation files.
6. Performance evaluation.

## Getting Started

To get started with this project, you need to have a basic understanding of C programming and Linux commands. You will also need to set up a GitHub repository for version control.

## Prerequisites

Before you begin, ensure you have met the following requirements:

* You have installed the latest version of gcc compiler.
* You are familiar with basic Linux commands and C programming.


## Contact

If you want to contact me, you can reach me at `joaopbugarin@gmail.com`.

## License

This project uses the following license: [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html).
