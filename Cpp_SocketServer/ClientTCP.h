/*
    Cliente TCP para propositos generales

    By Yecid Moreno : 2021
    git: https://github.com/YecidMorenoUSP

    Baseado de:
        Ighor Augusto Barreto Candido -> https://pt.stackoverflow.com/users/11831/ighor-augusto
             https://pt.stackoverflow.com/questions/22719/cliente-servidor-windows-com-sockets-em-c

    Requerimientos:
        1. [V] El puerto debe ser de entrada numerica
        2. [V] la salida debhe ser sólo un Buffer
        3. [V] Permitir escalabilidad para poder integrar en la aplicacion
        4. [V] manejo de errores
*/


#if !defined(CLIENTTCP_H)
#define CLIENTTCP_H

#if defined(DEBUG_CLIENT_TCP)
    #include <iostream>
#endif




// #undef UNICODE

#define WIN32_LEAN_AND_MEAN


// size of buffer MAX
#if !defined(DEFAULT_BUFLEN)
#define DEFAULT_BUFLEN 512
#endif 


#include <windows.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <mutex> 
#include <thread>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


void __CRTDECL CLIENTE_TCP_DEBUG(_In_z_ _Printf_format_string_ char const* const _Format,...){
        #if defined(DEBUG_CLIENT_TCP)
            printf(_Format);
        #endif // DEBUG_CLIENT_TCP    
}

typedef struct {
    char recBuffer[DEFAULT_BUFLEN];
    char sendBuffer[DEFAULT_BUFLEN];
    int iResult = 0;
    std::mutex mtx;

    void updateRecBuffer(char * bufferRec, int len);
    void readRecBuffer(char * bufferSend, int * len);

    void updateSendBuffer(char * bufferRec, int len);
    void readSendBuffer(char * bufferSend, int * len);

}SharedBuffer;


class ClientTCP
{
    private:
        PCSTR address;
        PCSTR port;
        struct addrinfo *result = NULL;


    public:

        std::mutex mtx;

        struct {
            SOCKET ConnectSocket = INVALID_SOCKET;
            
            char recvbuf[DEFAULT_BUFLEN];
            char sendbuf[DEFAULT_BUFLEN];
            int sendbuflen = DEFAULT_BUFLEN;
            int recvbuflen = DEFAULT_BUFLEN;
            
            int iResult = 0;

            std::thread clientThread_;

        }LOCAL;

        ClientTCP(PCSTR address,PCSTR port,SharedBuffer * app);
        ClientTCP();
        int initClient();
        int tryConnect();
        void loopServer();
        void waitJoin();
        void runLoop();
        int closeClient();
        SharedBuffer *app;

};

#include "ClientTCP.cpp"

#endif // ClientTCP_H
