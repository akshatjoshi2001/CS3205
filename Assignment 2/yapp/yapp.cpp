#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <iostream>


using namespace std;
int main(int argc,char** argv)
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if(fd < 0)
    {
        cout << "Error occured! Check if IPPROTO_ICMP permissions are enabled through sysctl" << endl;
        return -1;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    icmphdr icmp_packet;
    icmphdr icmp_packet_recv;
    icmp_packet.type = ICMP_ECHO;
    icmp_packet.un.echo.id = 1112;
    icmp_packet.un.echo.sequence = 0;

    unsigned char data[2048];
    memcpy(data, &icmp_packet, sizeof(icmp_packet));
    memcpy(data + sizeof(icmp_packet), "hello", 5);



    int ret = sendto(fd,data,sizeof(icmp_packet)+5,0,(sockaddr*)&address,sizeof(address));
    if(ret<=0)
    {
        cout << "Host unreachable." << endl;
        exit(EXIT_SUCCESS);
    }

    
    timeval timeout;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    timeout.tv_sec = 5; // 5 second time out
    timeout.tv_usec = 0;

    int ret2 =  select(fd+1,&set,nullptr,nullptr,&timeout);
    if(ret2 <= 0)
    {
        cout << "Request timed out..."<< endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        unsigned int addrlen = 0;
        int ret3  = recvfrom(fd, data, sizeof(data), 0, NULL, &addrlen);
        if(ret3>0)
        {
            memcpy(&icmp_packet_recv, data, sizeof(icmp_packet_recv));

            
            if (icmp_packet_recv.type == ICMP_ECHOREPLY) {
                cout <<"Reply from server, id: "<< icmp_packet_recv.un.echo.id << endl;
            }
        }
    }


    




    


}
