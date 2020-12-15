#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SocketServer.h"
#include "SocketServerManager.h"
#include "Rs232Layer.h"

#define NUMBER_CLIENTS  8
#define CLIENTS_BUFFER_QUEUE_SIZE   64
struct S_ClientData
{
    int fd;
    pthread_t thread;
    int flagFree;
};
typedef struct S_ClientData ClientData;
struct S_ClientPacket
{
    ClientData* client;
    char buffer[512];
    int len;
    int flagFree;
};
typedef struct S_ClientPacket ClientPacket;

void* ssm_startServer(void* args);
void* ssm_listenerClients(void* args);
ClientData* ssm_getAvailableClient(void);
void ssm_pushNewPacket(ClientData* client,char* buff,int len);

pthread_t threadServer;
pthread_mutex_t mutexClientsPackets;
ClientData clients[NUMBER_CLIENTS];

ClientPacket clientsBuffer[CLIENTS_BUFFER_QUEUE_SIZE];
int indexQueue;


void printBuffer(char* b,int len)
{
    int i=0;
    while(i<len)
    {
        printf("0x%02x:",b[i]);
        i++;
    }
}

void ssm_init()
{
    int i;
    indexQueue=0;
    for(i=0;i<NUMBER_CLIENTS;i++)
        clients[i].flagFree=1;
    for(i=0;i<CLIENTS_BUFFER_QUEUE_SIZE;i++)
        clientsBuffer[i].flagFree=1;

    pthread_mutex_init(&mutexClientsPackets, NULL);
    pthread_create(&threadServer, NULL, ssm_startServer, NULL);
}

void* ssm_listenerClients(void* clientData)
{
    ClientData* client = (ClientData*)clientData;

    while(1)
    {
        char buff[512];
        //printf("check packet for client:%d\n\r",client->fd);
        int l = read(client->fd, buff,512);
        //printf("read devolvio:%d\r\n",l);
        if(l>0)
        {
           //printf("Packet from client: %d\r\n",client->fd);
           // Retransmito por puerto serie
           ssm_pushNewPacket(client,buff,l);
        }
        else
        {
            printf("Connection closed.\r\n");
            server_close(client->fd);
            client->flagFree=1;
            pthread_exit(NULL);
        }
    }
    return NULL;
}

void* ssm_startServer(void* args)
{
    //int serverFd = server_open(3000,1);
    //printf("Server Opened. fd:%d\r\n",serverFd);
    int serverFd;
    do{

        serverFd = server_open(3000,1);
        printf("Server Opened. fd:%d\r\n",serverFd);
        if(serverFd==-1)
        {
            printf("Error opening server, waiting for retry...\r\n");
            usleep(1000000);
            continue;
        }

        while(1)
        {
            printf("Listening incomming clients...\r\n");
            int clientFd = server_accept(serverFd);
            printf("New client. fd:%d\r\n",clientFd);
            ClientData* pClient = ssm_getAvailableClient();
            if(pClient!=NULL)
            {
                pClient->fd = clientFd;
                pthread_create(&(pClient->thread), NULL, ssm_listenerClients,(void*)pClient);
            }
            else
                server_close(clientFd); // rechazo conexion, se llego al limite de clientes

            usleep(100000);
        }
    }while(serverFd==-1);

    return NULL;
}

int ssm_getClientLen(void)
{
    int i;
    int count=0;
    for(i=0;i<NUMBER_CLIENTS;i++)
    {
        if(clients[i].flagFree==0)
            count++;
    }
    return count;
}

ClientData* ssm_getAvailableClient(void)
{
    int i;
    for(i=0;i<NUMBER_CLIENTS;i++)
    {
        if(clients[i].flagFree==1)
        {
            clients[i].flagFree=0;
            //printf("Cliente disponible en posicion:%d\r\n",i);
            return &(clients[i]);
        }
    }
    return NULL;
}

void ssm_closeAll(void)
{
    int i;
    for(i=0;i<NUMBER_CLIENTS;i++)
    {
        if(clients[i].flagFree==0)
            server_close(clients[i].fd);
    }
}

void ssm_pushNewPacket(ClientData* client,char* buff,int len)
{
    pthread_mutex_lock(&mutexClientsPackets);
    // busco ultima pos ocupada
    int i;
    for(i=indexQueue;i<CLIENTS_BUFFER_QUEUE_SIZE;i++)
    {
        if(clientsBuffer[i].flagFree==1)
        {
            //printf("guardo packet de cliente en cola, pos:%d data: ",i);
            //printBuffer(buff,len);printf("\r\n");
            clientsBuffer[i].client = client;
            clientsBuffer[i].len=len;
            memcpy(clientsBuffer[i].buffer,buff,len);
            clientsBuffer[i].flagFree=0;
            indexQueue=i+1;
            break;
        }
    }
    pthread_mutex_unlock(&mutexClientsPackets);
}

/*
*   Vacia la cola de paquetes desde los sockets, por cada paquete en la cola,
*   ejecuta la funcion pasada como argumento pasandole el buffer y la longitud
*   del paquete en el buffer.
*/
void ssm_poolClientsPackets(void (*callbackReleasePacket)(char*,int))
{
    pthread_mutex_lock(&mutexClientsPackets);
    int i;
    for(i=0;i<CLIENTS_BUFFER_QUEUE_SIZE;i++)
    {
        if(clientsBuffer[i].flagFree==0)
        {
            //printf("reenvio packet de cliente en cola, pos:%d ",i);
            //printBuffer(clientsBuffer[i].buffer,clientsBuffer[i].len);printf("\r\n");

            // Envio por puerto serie a la cape
            (*callbackReleasePacket)(clientsBuffer[i].buffer,clientsBuffer[i].len);
            usleep(10000);
            clientsBuffer[i].flagFree=1;
        }
    }
    indexQueue=0;
    pthread_mutex_unlock(&mutexClientsPackets);
}


void ssm_sendPacketToAllClients(char* pData,int len)
{
    int i;
    for(i=0;i<NUMBER_CLIENTS;i++)
    {
        if(clients[i].flagFree==0)
        {
            write(clients[i].fd,pData,len);
        }
    }
}


