
#include "../../include/common.h"
#include "../../include/server.h"

int server_fd, client_fd;

void signal_handler(int signum)
{
    printf("\n[INFO] Caught signal %d, shutting down server...\n", signum);
    if (server_fd > 0)
        close(server_fd);

    if (client_fd > 0)
        close(client_fd);

    exit(0);
}

int main(int argc, char *argv[])
{
    
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    int server_port;


    /* Register signal handler */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    /* Validating Command Line Argumnets */
    if( validate_arguments(argc, argv, &server_port) == FAILURE)
        return 0;
    

    /* Creating a socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        perror("Server Socket creation failed\n");
        return FAILURE;
    }

    /* Enable SO_REUSEADDR */
    int opt = 1;

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        close(server_fd);
        return FAILURE;
    }

    /* Initializing binding information */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces

    /*binding socket with suer given port and valid IPv$ interface*/
    if( bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        return FAILURE;
    }
    
    /* making socket to listen */
    if( listen(server_fd,10) == -1)
    {
        perror("listen");
        close(server_fd);
        return FAILURE;
    }


    printf("\n%s==========================================%s\n", CYAN, RESET);
    printf("%s%s   --- Remote Diagnostic Server ---   %s\n",BOLD, RED,RESET);
    printf("%s               Port : %d            %s\n", YELLOW, server_port, RESET);
    display_ip_addresses();
    printf("%s\n==========================================%s\n", CYAN, RESET);
    printf("%s[SYSTEM]%s Waiting for incoming connections...\n", GREEN, RESET);


    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < -1)
        {
            perror("accept failed");
            close(server_fd);
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];  
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("%s[INFO] Connection accepted from %s:%d%s\n", YELLOW, client_ip, ntohs(client_addr.sin_port), RESET);
        
        
        /* Create a dedicated thread for the new client */
        int *client_sock_ptr = malloc(sizeof(int));
        if (client_sock_ptr == NULL)
        {
            perror("Malloc failed");
            close(server_fd);
            continue;
        }

        *client_sock_ptr = client_fd;
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void*)client_sock_ptr) != 0)
        {
            perror("Thread creation failed");
            free(client_sock_ptr);
            close(server_fd);
        }
        else
        {
            /* Detach the thread so it cleans up automatically upon exit */
            pthread_detach(tid);
        }
    }   
}