#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    printf("\n\n\t\t\t\t\t Socket TCP/IP Server by Ighor Augusto\n");
    printf("\n\t-> Inicializando a biblioteca de Winsock...");  
    
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("\t\t[-] WSASturtup falhou! Erro: %d\n\n", iResult);
        return 1;
    }
    printf("\t\t[+] Biblioteca inicializada com sucesso!\n");

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    printf("\t-> Resolvendo endereco e porta do servidor...");
    
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }printf("\t\t[+] Sucesso!\n");


    printf("\t-> Criando SOCKET para conexao...");
    // Criando Socket
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("\t\t\t[-] Socket falhou! Erro: %ld\n\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    printf("\t\t\t[+] Socket criado com sucesso!\n");       

    printf("\t-> Setando TCP listening socket...\n\n");
    printf("\t-> bind()...");
    // Setup TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
            printf("\t\t\t\t\t\t[-] bind() falhou! Erro: %d\n\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
    }
    printf("\t\t\t\t\t\t[+] Sucesso!\n");

    freeaddrinfo(result);

    printf("\t-> listen()...");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
            printf("\t\t\t\t\t\t[-] listen() falhou! Erro: %d\n\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
    }
    printf("\t\t\t\t\t\t[+] Sucesso!\n");

    printf("\t-> Aguardando conexoes! accept()...");
    // Aceitando um cliente socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
            printf("\t\t\t[-] accept() falhou! Erro: %d\n\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
    }
    printf("\t\t\t[+] Sucesso!\n");

    closesocket(ListenSocket);

    printf("\n\t[+] Servidor pronto para receber dados!!!\n\n");
    // Recebe ate o cliente encerrar a conexao
    do {

        printf("\t-> Aguardando...");
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("\t\t\t\t\t[+] Bytes recebidos: %d\n", iResult);
            printf("\t-> Enviando um eco ao cliente...");
            // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("\t\t\t[-] send() falhou! Erro: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("\t\t\t[+] Sucesso! Bytes enviados: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("\t\t\t\t\t[-] Conexao fexada pelo cliente!\n");
        else  {
            printf("\t\t\t\t\t[-] recv() falhou! Erro: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    printf("\t-> Encerrando...");
    // Fexando a conexao quando terminarmos
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
            printf("\t\t\t\t\t[-] shutdown() falhou! Erro: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
    }
    printf("\t\t\t\t\t[+] Encerrado!\n");
    printf("\n\n\t### Bye!!! ###\n");
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;

}