#include <sys/socket.h>
#include <arpa/inet.h>
#include<iostream>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char** argv)
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if(fd < 0)
    {
        cout << "Error occured!" << endl;
        return -1;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);


}
