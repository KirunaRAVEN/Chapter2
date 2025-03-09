#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#define BUFSIZE 256 // will probably be changed

#define DEBUG 0 // debug level, 0-2

/*
Refactoring of serial_reader.py into C.
Author: Aurora Åhs, galaktahs

Currently, it can read from a single arduino, and it divides the stream into "packets", split by the delimiter. It does *not* have nearly full functionality.

TODO:
[X] read the stream of data from second arduino
[X] read at the requested baud rate
[ ] read the stream of data from first arduino
[ ] break out things into functions
[ ] read from two arduinos at the same time
[ ] csv printer (i hardly know 'er)
[ ] make things more reliable, readable, and pretty (and test the shit out of it)

*/

void main() {
    char buf[BUFSIZE];
    char * cptr = buf, *m;
    const char delim[] = "\r\n"; //arduino states CR+LF is standard delim, "d a" in ascii
    struct termios toptions;
    struct timeval timeout;
    int n;
    int fd;
    fd_set read_fds;

    memset(buf, '\0', BUFSIZE*sizeof(int));

    fd = open("/dev/ttyACM0", O_RDWR | O_NONBLOCK);

    tcgetattr(fd, &toptions);

    cfsetispeed(&toptions, B115200); // 115200 Baud
    cfsetospeed(&toptions, B115200);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    //Write options to the fd
    tcsetattr(fd, TCSANOW, &toptions);
    tcsetattr(fd, TCSAFLUSH, &toptions);

    for(;;) {
        timeout.tv_sec = 2; // timeout for select()
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        if(select(fd + 1, &read_fds, NULL, NULL, &timeout) == 1) {
            n = read(fd, cptr, 16);
#if (DEBUG >=2)
            for(int i = 0; i<BUFSIZE; i++) {
                printf("%x ", (int) buf[i]);
            }
            printf("(end)\n");
            for(int i = 0; i<BUFSIZE; i++) {
                printf("%c ", buf[i]);
            }
            printf("(end)\n");
#endif
            if(m = strstr(buf, delim)) { // if theres a terminating newline in recieved data, print packet.
#if (DEBUG >= 1)
                printf("delim detected, buffer state:\n");
                for(int i = 0; i<BUFSIZE; i++) {
                    printf("%x ", (int) buf[i]);
                }
                printf("(end)\n");
                for(int i = 0; i<BUFSIZE; i++) {
                    printf("%c ", buf[i]);
                }
                printf("(end)\n");
#endif
                *m = '\0';
                *(m+1) = '\0';
#if (DEBUG >= 1)
                printf("Packet:\n");
                for(int i = 0; buf[i] != '\0'; i++) {
                    printf("%x ", (int) buf[i]);
                }
                printf("\n[%s]\n", buf);
                printf("\n");
#else
                printf("Packet:[%s]\n", buf);
#endif
                memset(buf, '\0', cptr-buf); // clear used memory
                memcpy(buf, m+2, n-(m-cptr)); // move unused memory to beginning of buffer
                cptr = buf + n-(m-cptr)-2; // point cptr to first empty byte
            } else {
                cptr += n;
            }
        } else {
            putc('.', stderr);
        }
    }
}
