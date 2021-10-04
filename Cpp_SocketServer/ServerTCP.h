/*
    Servidor TCP para propositos generales

    By Yecid Moreno : 2021
    git: https://github.com/YecidMorenoUSP

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

#pragma comment (lib, "Ws2_32.lib")

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
        int initServer();
        int acceptClient();
        int closeServer();
        int loopClient();
        void waitJoin();
        void runLoop();
        SharedBuffer *app;

};

#include "ServerTCP.cpp"

#endif // SERVERTCP_H
