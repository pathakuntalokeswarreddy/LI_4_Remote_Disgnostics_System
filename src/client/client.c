#include "../../include/common.h"
#include "../../include/client.h"

int sock_fd;
char history[MAX_HISTORY][MAX_COMMAND_SIZE];
int history_count = 0;
struct termios old_term;

void signal_handler(int signum)
{
    printf("\n[INFO] Caught signal %d, shutting down server...\n", signum);
    if (sock_fd > 0)
        close(sock_fd);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    exit(0);
}


int main(int argc, char *argv[])
{

    struct sockaddr_in server_addr = {0};
    
    /* Register signal handler */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Validating Command Line Argumnets */
    if( validate_arguments(argc, argv, &server_addr) == FAILURE)
        return 0;
    

    /* Creating a socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        perror("Socket creation failed\n");
        return FAILURE;
    }


    /* Initializing binding information */
    server_addr.sin_family = AF_INET;


    /* Connect to server */
    if( connect(sock_fd,(struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection Failed");
        close(sock_fd);
        return FAILURE;
    }



    char ip_buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &server_addr.sin_addr, ip_buffer, sizeof(ip_buffer)) == NULL)
    {
        perror("inet_ntop");
        return FAILURE;
    }

    printf("\n%s============================================%s\n", CYAN, RESET);
    printf("%s%s            Remote Diagnostic Client       %s\n",BOLD, RED, RESET);
    printf("%s      Connected to Server: %s:%d            %s\n", YELLOW, ip_buffer, ntohs(server_addr.sin_port),RESET);
    printf("%s============================================%s\n", CYAN, RESET);
  
    char command[1024];
    
    while(1)
    {
        printf("%s\nClient]%s",GREEN, RESET);
        fflush(stdout);

        read_command(command, sizeof(command));
        store_history(command);
        
        char temp_buffer[1024];
        strcpy(temp_buffer,command);
        char *token = strtok(temp_buffer, " ");
                
        if (token != NULL)
        {
            printf("Requested Command: %s\n", token);

            if (strcmp(token, "exit") == 0)
            {
                printf("\nExiting.....\n");
                if (sock_fd > 0)
                    close(sock_fd);
                exit(0);
            }
            
            if (strcmp(token, "history") == 0)
            {
                display_history();
                continue;
            }
            else if (strcmp(token, "help") == 0)
            {
                display_menu();
                continue;
            }
            else if( (strcmp(token, REQ_PROC_ALL) == 0) ||
                     (strcmp(token, REQ_MEM_USG)  == 0) ||
                     (strcmp(token, REQ_CPU_USG)  == 0) ||
                     (strcmp(token, REQ_PORTS_USG)== 0) ||
                     (strcmp(token, REQ_OPEN_FD)  == 0) ||
                     (strcmp(token, REQ_KILL)     == 0) )
            {
                send(sock_fd,command,strlen(command),0);
                printf("INFO: Message sent to the server\n");

                receive_server_response(sock_fd);
            }
            else
            {
                printf("Invalid command\n");
            }
        }  
    }
}

void display_menu(void)
{
    printf("\n=====================================================================================\n");
    printf("                REMOTE MANAGEMENT HELP MENU\n");
    printf("=======================================================================================\n");

    printf("get-processes ALL             - For fetching the current running processes \n");
    printf("get-mem <process-PID>         - For fetching the memory usage of given process \n");
    printf("get-cpu-usage <process-PID>   - For fetching CPU usage of the given process \n");
    printf("get-ports-used <process-PID>  - For fetching the socket ports used by given process \n");
    printf("get-open-fd <process-PID>     - For fetching the open file descriptors of a given process \n");
    printf("kill <process-PID>            - For killing a given process \n");
    printf("history                       - Shows command history \n");
    printf("help                          - Shows this output \n");
    printf("exit                          - to exit from the program \n");
    fflush(stdout);
}

void display_history(void)
{
    for(int i=0; i<history_count; i++)
    {
        printf("%d : %s\n",i+1,history[i]);
    }
}