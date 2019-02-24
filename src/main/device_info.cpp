#include "device_info.h"
#include "ownership_change.h"

char host[NI_MAXHOST];
device_info *owner_device = (device_info*)malloc(sizeof(device_info));
device_info* parse_device_info(char* buffer){
	char *pos;
	//printf("Buffer \n= %s\nEnd\n", buffer);

	memset(owner_device->device_name,'\0', sizeof(owner_device->device_name));
	memset(owner_device->bt_address,'\0', sizeof(owner_device->bt_address));
	pos = strstr(buffer, "Device");
	memcpy(owner_device->bt_address, pos+7, 17);
	memcpy(owner_device->device_name,pos+25, 128-32);
	if((pos = strchr(owner_device->device_name, '\n')) != NULL)
		*pos = '\0';
	return owner_device;
}


char* get_own_ipadress()
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    if(is_test){
    	memset(host, '\0', NI_MAXHOST);
    	memcpy(host, "192.168.0.1", 12);
    }
    else{
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
    	    freeifaddrs(ifaddr);
    }

    if(DEBUG_LEVEL>2)
    	printf("%s\n", host);

    return host;
}
