#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024

void print_sorted_emails_by_sender() {
    FILE *pipe;
    char command[MAX_LINE_LENGTH];

    // Create command to list emails sorted by sender
    snprintf(command, MAX_LINE_LENGTH, "mail -H | sort -t '|' -k 5");

    // Open pipe to read output of command
    pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen failed");
        exit(1);
    }

    // Print output of command
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, pipe)) {
        printf("%s", line);
    }

    // Close pipe
    pclose(pipe);
}

void print_sorted_emails_by_date() {
    FILE *pipe;
    char command[MAX_LINE_LENGTH];

    // Create command to list emails sorted by date
    snprintf(command, MAX_LINE_LENGTH, "mail -H | sort -t '|' -k 3");

    // Open pipe to read output of command
    pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen failed");
        exit(1);
    }

    // Print output of command
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, pipe)) {
        printf("%s", line);
    }

    // Close pipe
    pclose(pipe);
}

void send_email(char *to, char *subject, char *body) {
    FILE *pipe;
    char command[MAX_LINE_LENGTH];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Create command to send email
    snprintf(command, MAX_LINE_LENGTH, "mail -s \"%s\" %s << EOF\n%s\nEOF", subject, to, body);

    // Open pipe to run command
    pipe = popen(command, "w");
    if (pipe == NULL) {
        perror("popen failed");
        exit(1);
    }

    // Write body of email to pipe
    fputs(body, pipe);

    // Close pipe
    pclose(pipe);

    printf("Email sent to %s on %d-%02d-%02d\n", to, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // One argument - sort emails by sender or date
        char *arg = argv[1];
        if (strcmp(arg, "nadawca") == 0) {
            print_sorted_emails_by_sender();
        } else if (strcmp(arg, "data") == 0) {
            print_sorted_emails_by_date();
        } else {
            printf("Invalid argument\n");
            exit(1);
        }
    } else if (argc == 4) {
        // Three arguments - send email
        char *to = argv[1];
        char *subject = argv[2];
        char *body = argv[3];
        send_email(to, subject, body);
    } else {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    return 0;
}
