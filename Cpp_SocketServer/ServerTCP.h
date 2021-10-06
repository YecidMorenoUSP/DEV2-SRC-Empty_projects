/*
    Servidor TCP para propositos generales

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


#if !defined(SERVERTCP_H)
#define SERVERTCP_H

#if defined(DEBUG_SERVER_TCP)
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

#include "utilsTCP.h"


#pragma comment (lib, "Ws2_32.lib")


class ServerTCP
{
    private:
        PCSTR port;

    public:

        std::mutex mtx;

        struct {
            SOCKET ListenSocket = INVALID_SOCKET;
            SOCKET ClientSocket = INVALID_SOCKET;
            
            char recvbuf[DEFAULT_BUFLEN];
            char sendbuf[DEFAULT_BUFLEN];
            int sendbuflen = DEFAULT_BUFLEN;
            int recvbuflen = DEFAULT_BUFLEN;
            
            int iResult = 0;

            std::thread clientThread_;

        }LOCAL;

        ServerTCP(PCSTR port,SharedBuffer * app);
        ServerTCP();
        int initServer();
        int acceptClient();
        int closeServer();
        void loopClient();
        void waitJoin();
        void runLoop();
        SharedBuffer *app;

};

#include "ServerTCP.cpp"

#endif // SERVERTCP_H
