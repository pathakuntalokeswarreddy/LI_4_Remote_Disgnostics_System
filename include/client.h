#ifndef CLIENT_H
#define CLIENT_H

#define MAX_HISTORY 50
#define MAX_COMMAND_SIZE 1024

extern char history[MAX_HISTORY][MAX_COMMAND_SIZE];
extern int history_count;
extern struct termios old_term;

int validate_arguments(int argc, char *argv[], struct sockaddr_in *port);

void display_menu(void);
void display_history(void);
void receive_server_response(int client_fd);

void read_command(char *command, int size);
void store_history(const char *command);



#endif