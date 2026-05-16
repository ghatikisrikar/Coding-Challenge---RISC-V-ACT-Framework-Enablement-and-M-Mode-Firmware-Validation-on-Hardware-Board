#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>

int main(int argc, char *argv[]) {
    int serial_port;
    struct termios tty;
    const char *portname = "/dev/ttyUSB0"; // default port

    if (argc > 1) {
        portname = argv[1];
    }

    // open the serial port
    serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // read current config
    if (tcgetattr(serial_port, &tty) != 0) {
        perror("tcgetattr failed");
        close(serial_port);
        return 1;
    }

    // configure for 115200 8N1
    // TODO: make baud rate configurable later if needed
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag &= ~PARENB; // no parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 data bits
    
    // disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL; // turn on read & ignore ctrl lines

    // raw mode setup
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;
    
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    tty.c_oflag &= ~OPOST; // raw output

    // timeout config
    tty.c_cc[VTIME] = 0; 
    tty.c_cc[VMIN] = 0; // non-blocking for use with select

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
        close(serial_port);
        return 1;
    }

    // remove O_NDELAY flag now that port is open and configured
    fcntl(serial_port, F_SETFL, 0);

    // transmit a test message
    char *msg = "Hello UART Test\n";
    int n = write(serial_port, msg, strlen(msg));
    if (n < 0) {
        perror("Write failed");
    } else {
        printf("Sent %d bytes\n", n);
    }

    // wait for data using select
    fd_set read_fds;
    struct timeval tv;
    
    FD_ZERO(&read_fds);
    FD_SET(serial_port, &read_fds);

    // 2 second timeout
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    printf("Waiting for response...\n");
    int ret = select(serial_port + 1, &read_fds, NULL, NULL, &tv);

    if (ret < 0) {
        perror("select error");
    } else if (ret == 0) {
        printf("Timeout reached. No data.\n");
    } else {
        if (FD_ISSET(serial_port, &read_fds)) {
            char buf[256];
            memset(buf, 0, sizeof(buf));
            
            // read incoming data
            int bytes_read = read(serial_port, buf, sizeof(buf) - 1);
            if (bytes_read > 0) {
                printf("Received %d bytes: %s\n", bytes_read, buf);
            } else if (bytes_read < 0) {
                perror("Read error");
            }
        }
    }

    close(serial_port);
    return 0;
}
