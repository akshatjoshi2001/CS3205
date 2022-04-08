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
#include <chrono>
#include <regex>

using namespace std;

bool checkIPValidity(char* ipstr)
{
    string ip_string((size_t)strlen(ipstr),'0');
    for(int i=0;i<ip_string.size();i++)
    {
        ip_string[i] = ipstr[i];
    }
     const regex ip_regex("^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$");
    return regex_match(ip_string,ip_regex);

    
}




int main(int argc,char** argv)
{
    if(argc < 2)
    {
        cout << "Please enter the IP address. " << endl;
        return -1;
    }

    if(!checkIPValidity(argv[1]))
    {
        cout << "Badly formatted IP address. " << endl;
        return -1;
    }


    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if(fd < 0)
    {
        cout << "Error occured! Check if IPPROTO_ICMP permissions are enabled through sysctl" << endl;
        return -1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);

    icmphdr icmp_packet; // Packet which is being sent
    icmphdr icmp_packet_recv; // Packet which is recieved as reply


    icmp_packet.type = ICMP_ECHO;   // ICMP Echo is being sents
    icmp_packet.un.echo.sequence = 0;

    unsigned char data[2048]; // Stores bytes of icmp packet (serialised)
    memcpy(data, &icmp_packet, sizeof(icmp_packet));
    memcpy(data + sizeof(icmp_packet), "payload", 7);


   
    int send_ret_val = sendto(fd,data,sizeof(icmp_packet)+7,0,(sockaddr*)&address,sizeof(address));
   
    
    if(send_ret_val<=0)
    {
        cout << "Host unreachable." << endl;
        exit(EXIT_SUCCESS);
    }

    
    timeval timeout;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    timeout.tv_sec = 5; 
    timeout.tv_usec = 0;

    auto start = std::chrono::high_resolution_clock::now();
    int select_ret_val =  select(fd+1,&set,nullptr,nullptr,&timeout);
    auto end = std::chrono::high_resolution_clock::now();
    double rtt = std::chrono::duration<double, std::milli>(end-start).count();


    if(select_ret_val <= 0)
    {
        cout << "Request timed out..."<< endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        unsigned int addrlen = 0;
        int read_ret_val  = recvfrom(fd, data, sizeof(data), 0, NULL, &addrlen);
        if(read_ret_val>0)
        {
            memcpy(&icmp_packet_recv, data, sizeof(icmp_packet_recv));

            
            if (icmp_packet_recv.type == ICMP_ECHOREPLY) {
                cout <<"Reply from " << argv[1] <<  ", id: "<< icmp_packet_recv.un.echo.id << ", RTT: " << rtt << " ms" << endl;
            }
        }
    }

}
