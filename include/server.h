#ifndef SERVER_H
#define SERVER_H

int validate_arguments(int argc, char *argv[], int *port);

void display_ip_addresses(void);

void client_handler(int client_fd);

int validate_pid(const char *input, pid_t *pid);
int check_pid(pid_t pid);

int execute_command(char *cmd, char response[], int *total_size);
#endif
