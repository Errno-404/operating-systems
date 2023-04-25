#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

int main() {
    fd_set rfds;
    struct timeval tv;
    int retval;
    int fd = 0; // file descriptor for stdin
    char input_buffer[1024];

    while (1) {
        // Print a message every second
        printf("Waiting for input...\n");
        sleep(1);

        // Check for input from the user
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        retval = select(fd+1, &rfds, NULL, NULL, NULL);

        if (retval == -1) {
            perror("select()");
        } else if (retval) {
            // Input is available, read it
            fgets(input_buffer, sizeof(input_buffer), stdin);
            printf("You typed: %s", input_buffer);
        }
    }

    return 0;
}