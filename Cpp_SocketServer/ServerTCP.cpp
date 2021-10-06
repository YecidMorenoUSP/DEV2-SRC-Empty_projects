#include "ServerTCP.h"

ServerTCP::ServerTCP(PCSTR port,SharedBuffer * app)
{
    this->port = port;
    this->app = app;
}

ServerTCP::ServerTCP()
{
}


int ServerTCP::initServer(){
    
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        #if defined(DEBUG_SERVER_TCP)
            printf("\t\t[-] WSASturtup falhou! Erro: %d\n\n", iResult);
        #endif // DEBUG_SERVER_TCP
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL,this->port, &hints, &result);
    if ( iResult != 0 ) {
        #if defined(DEBUG_SERVER_TCP)
            printf("getaddrinfo failed with error: %d\n", iResult);
        #endif // DEBUG_SERVER_TCP
        WSACleanup();
        return 1;
    }

    LOCAL.ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (LOCAL.ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind( LOCAL.ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
            #if defined(DEBUG_SERVER_TCP)
                printf("\t\t\t\t\t\t[-] bind() falhou! Erro: %d\n\n", WSAGetLastError());
            #endif // DEBUG_SERVER_TCP

            freeaddrinfo(result);
            closesocket(LOCAL.ListenSocket);
            WSACleanup();
            return 1;
    }

    freeaddrinfo(result);

    #if defined(DEBUG_SERVER_TCP)
        printf("\n>> Linten by port %s",port);
    #endif // DEBUG_SERVER_TCP

    return 0;
}

int ServerTCP::acceptClient(){
    #if defined(DEBUG_SERVER_TCP)
        printf("\n>> Waiting client");
    #endif // DEBUG_SERVER_TCP
    int iResult = listen(LOCAL.ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
            closesocket(LOCAL.ListenSocket);
            WSACleanup();
            return 1;
    }
    
    LOCAL.ClientSocket = accept(LOCAL.ListenSocket, NULL, NULL);
    if (LOCAL.ClientSocket == INVALID_SOCKET) {
            closesocket(LOCAL.ListenSocket);
            WSACleanup();
            return 1;
    }
    closesocket(LOCAL.ListenSocket);
    
    #if defined(SERVER_TCP_SEND_FIRST)
        send( LOCAL.ClientSocket, "OI\0\n", sizeof("OI\0\n"), 0 );
    #endif

    return 0;
}

int ServerTCP::closeServer(){
    
    int iResult = shutdown(LOCAL.ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
            closesocket(LOCAL.ClientSocket);
            WSACleanup();
            return 1;
    }
    closesocket(LOCAL.ClientSocket);
    WSACleanup();

    #if defined(DEBUG_SERVER_TCP)
        printf("\n>> Closed server",this->port);
    #endif // DEBUG_SERVER_TCP

    return 0;
}

void ServerTCP::loopClient(){
    #if defined(DEBUG_SERVER_TCP)
        printf("\n>> LoopClient started");
    #endif // DEBUG_SERVER_TCP
    int iResult;
    int iSendResult;
        do {
        // mtx.lock();
        
        iResult = recv(LOCAL.ClientSocket, LOCAL.recvbuf, LOCAL.recvbuflen, 0);
      
        
        #if defined(DEBUG_SERVER_TCP)
                printf("\n\t[+] size: %4d \t\t Data: %s ",iResult,LOCAL.recvbuf);
        #endif // DEBUG_SERVER_TCP
        // mtx.unlock();
        if (iResult > 0) {   
            // mtx.lock();
            app->updateRecBuffer(LOCAL.recvbuf, iResult);
            LOCAL.iResult = iResult;
            app->readSendBuffer(LOCAL.sendbuf,&LOCAL.sendbuflen);
            iSendResult = send( LOCAL.ClientSocket, LOCAL.sendbuf, LOCAL.sendbuflen, 0 );
            #if defined(DEBUG_SERVER_TCP)
                    printf(" >> %s",LOCAL.sendbuf);
            #endif 
            // mtx.unlock();
            if (iSendResult == SOCKET_ERROR) {
                #if defined(DEBUG_SERVER_TCP)
                    printf("\n>> SOCKET_ERROR SEND");
                #endif // DEBUG_SERVER_TCP
                closesocket(LOCAL.ClientSocket);
                WSACleanup();
                return;
            }
        }
        else if (iResult == 0){
            #if defined(DEBUG_SERVER_TCP)
                printf("\n>> Closed by client");
            #endif // DEBUG_SERVER_TCP
            closesocket(LOCAL.ClientSocket);
            WSACleanup();
        }
        else  {
            #if defined(DEBUG_SERVER_TCP)
                printf("\n>> SOCKET_ERROR REC");
            #endif // DEBUG_SERVER_TCP
            closesocket(LOCAL.ClientSocket);
            WSACleanup();
            return;
        }

    } while (iResult > 0);
    #if defined(DEBUG_SERVER_TCP)
        printf("\n>> LoopClient finished");
    #endif // DEBUG_SERVER_TCP
    return;
}

void ServerTCP::waitJoin(){
    if(LOCAL.clientThread_.joinable())LOCAL.clientThread_.join();
}

void ServerTCP::runLoop(){
    waitJoin();
    LOCAL.clientThread_ =  std::thread(&loopClient,this);
}


