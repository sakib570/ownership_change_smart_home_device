#include "device_info.h"
#include "ownership_change.h"

char host[NI_MAXHOST];

char* get_own_ipadress()
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        s=getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name,"wlan0")==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
        }
    }
    //inet_aton(host, &ip_address);
    if(DEBUG_LEVEL>2)
    	printf("%s\n", host);
    freeifaddrs(ifaddr);
    return host;
}
