#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>

#include "../../include/common.h"
#include "../../include/server.h"

void display_ip_addresses(void)
{
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return;
    }

    printf("\n%s%s      Server Network Interfaces%s",BOLD,MAGENTA,RESET);

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        /* Consider only IPv4 interfaces */
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;

            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));

            /* Ignore loopback interface */
            if (!(ifa->ifa_flags & IFF_LOOPBACK))
            {
                printf("\n%s          Interface : %s%s\n", BLUE, ifa->ifa_name, RESET);
                printf("%s          IP Address: %s%s\n", BLUE, ip, RESET);
            }
        }
    }

    freeifaddrs(ifaddr);
}