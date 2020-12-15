#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int server_open(int port,int connections)
{
    int socketServerFd;
    socketServerFd = socket(AF_INET,SOCK_STREAM,0);
    if(socketServerFd==-1)
        return -1;

    struct sockaddr srv;
    bzero((char*)&srv,sizeof(struct sockaddr));

    struct sockaddr_in * server = (struct sockaddr_in *)&srv;

    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(port);
    int ret = bind(socketServerFd,(struct sockaddr *) &srv,sizeof(struct sockaddr));

    if(ret < 0)
    {
        close(socketServerFd);
        return -1;
    }

    int retList = listen(socketServerFd,connections);
    if(retList != 0)
    {
        close(socketServerFd);
        return -1;
    }

    return socketServerFd;
}


int server_accept(int socketServerFd)
{
    char ip[64];
    int portInt;

    // IPv4
    struct sockaddr_in fromAddr;
    int clientStructLen = sizeof(fromAddr);
    int socketClientFd = accept(socketServerFd,(struct sockaddr *) &fromAddr,&clientStructLen);
    if(socketClientFd==-1)
        return -1; // Error
    // Load ip
    if(fromAddr.sin_family==AF_INET)
    {
        struct sockaddr_in* sa = (struct sockaddr_in*)&fromAddr;
        inet_ntop(AF_INET,&sa->sin_addr,ip,sizeof(ip));
    }
    else
    {
        struct sockaddr_in6* sa = (struct sockaddr_in6*)&fromAddr;
        inet_ntop(AF_INET6,&sa->sin6_addr,ip,sizeof(ip));
    }
    // Load port
    portInt = ntohs(fromAddr.sin_port);

    printf("IP:%s\r\n",ip);
    printf("PORT:%d\r\n",portInt);

    return socketClientFd;
}

void server_close(int socketFd)
{
    close(socketFd);
}

