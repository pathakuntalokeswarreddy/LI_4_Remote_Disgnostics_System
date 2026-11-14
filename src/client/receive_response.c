#include "../../include/common.h"
#include "../../include/client.h"

/* Receive complete response from server */
void receive_server_response(int client_fd)
{
    int total_size;

    // Receive total size of server response
    int n = recv(client_fd, &total_size, sizeof(total_size), 0);
    if (n == -1)
    {
        perror("recv");
        return;
    }

    printf("\nReceived total size = %d\n\n", total_size);

    char response[8192] = {'\0'};
    int size = 0;
    char line_buffer[1025] = {'\0'};

    // Receive complete response until expected size is reached
    while (size < total_size)
    {
        int bytes = recv(client_fd, line_buffer,(total_size - size) < 1024 ? (total_size - size) : 1024,0);

        if (bytes > 0)
        {
            line_buffer[bytes] = '\0';
            strcat(response, line_buffer);
            size += bytes;
        }
        else
        {
            break;
        }
    }

    /* Display server response */
    printf("RESPONSE: \n%s\n", response);
}