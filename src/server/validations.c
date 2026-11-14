
#include "../../include/common.h"
#include "../../include/server.h"

int validate_arguments(int argc, char *argv[], int *port)
{
    if(argc!=2)
    {
        printf("ERROR: Invalid Command Line Arguments\n");
        printf("USAGE: ./server <port>");
        return FAILURE;
    }

    char *end;
    long value;
    errno = 0;
    value = strtol(argv[1], &end, 10);

    if (errno == ERANGE || argv[1] == end || *end != '\0' || value < 1024 || value > 65535)
    {
        printf("ERROR: Invalid Port Number\n");
        printf("INFO : Accepted Range: 1024 to 65535\n");
        return FAILURE;
    }
    
    *port = (int)value;

    return SUCCESS;
}


int validate_pid(const char *input, pid_t *pid)
{
    char *end;
    long value;

    errno = 0;
    value = strtol(input, &end, 10);

    if (input == end    || *end != '\0' ||
        errno == ERANGE ||  value <= 0  || value > INT_MAX)
    {
        return FAILURE;
    }
    
    if(check_pid(value) == SUCCESS)
    {
        *pid = (pid_t)value;
        return SUCCESS;
        
    }
    return FAILURE;
}

int check_pid(pid_t pid)
{
    if (kill(pid, 0) == 0)
    {
        return SUCCESS;
    }

    if (errno == ESRCH)
    {
        return FAILURE;       // PID does not exist
    }
    else if (errno == EPERM)
    {
        return SUCCESS;       // Exists, but insufficient permission
    }

    return FAILURE;
}