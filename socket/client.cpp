#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
int main()
{
    int cleint = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1001);
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (connect(cleint, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "Error: connect" << std::endl;
        return 0;
    };
    std::cout << "...connect" << std::endl;
}