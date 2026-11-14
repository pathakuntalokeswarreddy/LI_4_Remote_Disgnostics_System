
#include "../../include/common.h"


int validate_arguments(int argc, char *argv[], struct sockaddr_in *server_addr)
{
    if(argc!=3)
    {
        printf("ERROR: Invalid Command Line Arguments\n");
        printf("USAGE: ./server <IP_Address> <port>\n");
        return FAILURE;
    }

    
    /* IP Address Validation */
    int ret = inet_pton(AF_INET, argv[1], &(server_addr->sin_addr));
    if( ret <= 0)
    {
        perror("ERROR: Invalid IP Address");
        return FAILURE;
    }


    /* Port Number Validation */
    char *end;
    long value;
    errno = 0;
    value = strtol(argv[2], &end, 10);
    
    if (errno == ERANGE || argv[1] == end || *end != '\0' || value < 1024 || value > 65535)
    {
        printf("ERROR: Invalid Port Number\n");
        printf("INFO : Accepted Range: 1024 to 65535\n");
        return FAILURE;
    }
    server_addr -> sin_port = htons((int)value);

    return SUCCESS;
}