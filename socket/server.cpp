#include <sys/socket.h>
// #include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
int main()
{
    int sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1001);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sk, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cout << "Error: bind" << std::endl;
        return 0;
    };
    if (listen(sk, 1) == -1)
    {
        std::cout << "Error: listen" << std::endl;
        return 0;
    };
    std::cout << "...listening" << std::endl;

    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    int conn = accept(sk, (struct sockaddr *)&client_addr, &length);

    char clientIP[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &client_addr.sin_addr, clientIP, INET_ADDRSTRLEN);

    std::cout << "...connect" << clientIP << ":" << ntohs(client_addr.sin_port) << std::endl;
    close(sk);
    return 1;
}