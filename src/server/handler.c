#include "../../include/common.h"
#include "../../include/server.h"


void client_handler(int client_fd)
{
    char request_buffer[8092];
    while(1)
    {
        /* Receive command from client */
        int n = recv(client_fd, request_buffer, sizeof(request_buffer)-1, 0);
        if (n == -1)
        {
            perror("recv");
            break;
        }
        else if (n == 0)
        {
            printf("Client Disconnected\n");
            close(client_fd);
            break;
        }
        request_buffer[n] = '\0';
        printf("Requested Command: %s\n",request_buffer);
            
        
        char response[8192]={'\0'};
        int response_size=0;
        char cmd[100];

        char *token = strtok(request_buffer, " ");
        if (token != NULL)
        {
            char *arg = strtok(NULL, " ");
            if(strcmp(token, REQ_PROC_ALL) == 0)
            {
                if( strcmp(arg, "ALL") == 0)
                {
                    snprintf(cmd, sizeof(cmd), "ps -eo pid,comm,stat");
                    if( execute_command(cmd,response,&response_size) == FAILURE)
                    {
                        printf("ERROR: Execution Failed\n");
                        response_size = strlen(RESP_EXEC_FAIL);
                        strcpy(response,RESP_EXEC_FAIL);
                    }  
                }
                else
                {
                    printf("ERROR: Invalid Argument\n");
                    response_size = strlen(RESP_INVALID_ARG);
                    strcpy(response,RESP_INVALID_ARG);
                }
            }
            
            pid_t pid;
            int kill_flag=0;
            if( !response_size)
            {   
                if(arg != NULL && (validate_pid(arg, &pid) == SUCCESS)) 
                {   
                    if(strcmp(token, REQ_MEM_USG) == 0)
                    {
                        snprintf(cmd, sizeof(cmd), "ps -p %d -o pid,comm,%%mem,rss,vsz", pid);  
                    }
                    else if(strcmp(token, REQ_CPU_USG) == 0)
                    {
                        snprintf(cmd, sizeof(cmd), "ps -p %d -o pid,comm,%%cpu", pid);
                    }
                    else if(strcmp(token, REQ_PORTS_USG) == 0)
                    {
                        snprintf(cmd, sizeof(cmd), "lsof -nP -a -p %d -i; lsof -nP -a -p %d -U", pid, pid);
                    }
                    else if(strcmp(token, REQ_OPEN_FD) == 0)
                    {
                        snprintf(cmd, sizeof(cmd), "ls -l /proc/%d/fd", pid);
                    }
                    else if(strcmp(token, REQ_KILL) == 0)
                    {
                        if (kill(pid, SIGTERM) == 0)
                        {
                            printf("SERVER: SIGTERM sent successfully\n");

                            response_size = strlen(RESP_PROC_TERM);
                            strcpy(response, RESP_PROC_TERM);
                        }
                        else
                        {
                            perror("kill");
                            response_size = strlen(RESP_EXEC_FAIL);
                            strcpy(response, RESP_EXEC_FAIL);
                        }
                        kill_flag = 1;
                    }
                    
                    if( !kill_flag && execute_command(cmd,response,&response_size) == FAILURE)
                    {
                        printf("ERROR: Execution Failed\n");
                        response_size = strlen(RESP_EXEC_FAIL);
                        strcpy(response,RESP_EXEC_FAIL);
                    }
                }
                else
                {
                    printf("ERROR: Invalid Argument\n");
                    response_size = strlen(RESP_INVALID_ARG);
                    strcpy(response,RESP_INVALID_ARG);
                }
            }
            
    
            int ret = send(client_fd,&response_size,sizeof(response_size),0);
            if (ret <= 0)
            {
                perror("send");
                continue;
            }

            usleep(100000); //A small delay to process response_size by client
            
            size_t sent = 0;
            while (sent < response_size)
            {
                int ret = send(client_fd, response + sent, response_size - sent, 0);

                if (ret <= 0)
                {
                    perror("send");
                    break;
                }
                sent += ret;
            }  
        }
    }
}


int execute_command(char *cmd, char response[], int *total_size)
{
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        perror("popen");
        return FAILURE;
    }

    char buffer[1024];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        memcpy(response + *total_size, buffer, n);
        *total_size += n;
    }

    int status = pclose(fp);

    // printf("pclose status = %d\n", status);

    // if (WIFEXITED(status))
    // {
    //     printf("Exit code = %d\n", WEXITSTATUS(status));
    // }

    if (status == -1)
    {
        return FAILURE;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
        return FAILURE;
    }

    if (!WIFEXITED(status))
    {
        return FAILURE;
    }
    return SUCCESS;
}
