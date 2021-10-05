#include "ClientTCP.h"

ClientTCP::ClientTCP(PCSTR address,PCSTR port,SharedBuffer * app)
{
    this->address = address;
    this->port = port;
    this->app = app;
}

ClientTCP::ClientTCP()
{
}

        
int ClientTCP::initClient(){

    CLIENTE_TCP_DEBUG("\n\t\t\t\t\t Socket TCP/IP Server \n\n");
    
    int iResult;
    WSADATA wsaData;

    struct addrinfo hints;


    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        CLIENTE_TCP_DEBUG("\t\t[-] WSASturtup falhou! Erro: %d\n\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    CLIENTE_TCP_DEBUG("\t-> Resolvendo endereco e porta do servidor...");
    iResult = getaddrinfo(this->address, this->port, &hints, &result);
    if ( iResult != 0 ) {
        CLIENTE_TCP_DEBUG("\t\t[-] getaddrinfo() falhou! Erro: %d\n\n", iResult);
        WSACleanup();
        return 1;
    }
    CLIENTE_TCP_DEBUG("\t\t[+] Sucesso!\n");

    return 0;
}

int ClientTCP::tryConnect(){
    int iResult;
    struct addrinfo *ptr = NULL;

    CLIENTE_TCP_DEBUG("\t-> Iniciando procedimento de conexao...");
    // Lanca tentativas de conexao com o host até obter sucesso... ou nao.
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        CLIENTE_TCP_DEBUG("\n\t-> Criando SOCKET para conexao...");
        // Criando socket
        LOCAL.ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (LOCAL.ConnectSocket == INVALID_SOCKET) {
            CLIENTE_TCP_DEBUG("\t\t\t\t[-] Socket falhou! Erro: %ld\n\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        CLIENTE_TCP_DEBUG("\t\t\t[+] Socket criado com sucesso!\n");

        CLIENTE_TCP_DEBUG("\t-> Conectando ao host...");
        // Connect to server.
        iResult = connect( LOCAL.ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(LOCAL.ConnectSocket);
            LOCAL.ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (LOCAL.ConnectSocket == INVALID_SOCKET) {
        CLIENTE_TCP_DEBUG("\t\t\t\t[-] Conexao com o server invalida!\n\n");
        WSACleanup();
        return 1;
    }
    CLIENTE_TCP_DEBUG("\t\t\t\t[+] Sucesso!!!\n");

    #if defined(CLIENT_TCP_SEND_FIRST)
        send( LOCAL.ConnectSocket, "OI\0\n", sizeof("OI\0\n"), 0 );
    #endif

    return 0;
}

void ClientTCP::loopServer(){
    int iResult;
    int iSendResult;

    do {
        iResult = recv(LOCAL.ConnectSocket, LOCAL.recvbuf, LOCAL.recvbuflen, 0);
        if ( iResult > 0 ){ 
            app->updateRecBuffer(LOCAL.recvbuf, iResult);
            LOCAL.iResult = iResult;
            app->readSendBuffer(LOCAL.sendbuf,&LOCAL.sendbuflen);
            iSendResult = send( LOCAL.ConnectSocket, LOCAL.sendbuf, LOCAL.sendbuflen, 0 );
            if (iSendResult == SOCKET_ERROR) {
                CLIENTE_TCP_DEBUG("\n>> SOCKET_ERROR SEND");
                break;
            }

            CLIENTE_TCP_DEBUG("\t\t\t[+] Sucesso! Bytes recebidos: %d\n", iResult);

        }else if ( iResult == 0 ){
            CLIENTE_TCP_DEBUG("\n\t[+] Procedimento finalizado!\n");
            break;
        }else{
            CLIENTE_TCP_DEBUG("\t\t\t[-] recv() falhou! Erro: %d\n", WSAGetLastError());
            break;
        }
    } while( iResult > 0 );
    
    ClientTCP::closeClient();
}

void ClientTCP::waitJoin(){
    if(LOCAL.clientThread_.joinable())LOCAL.clientThread_.join();
}

void ClientTCP::runLoop(){
    waitJoin();
    LOCAL.clientThread_ =  std::thread(&loopServer,this);
}

int ClientTCP::closeClient(){
    CLIENTE_TCP_DEBUG("\t-> Encerrando conexao...");
    
    int iResult = shutdown(LOCAL.ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        CLIENTE_TCP_DEBUG("\t\t\t\t[-] shutdown() falhou! Erro: %d\n", WSAGetLastError());
        closesocket(LOCAL.ConnectSocket);
        WSACleanup();
        return 1;
    }
    CLIENTE_TCP_DEBUG("\t\t\t\t[+] Encerrado!\n");

    CLIENTE_TCP_DEBUG("\t-> Lendo a resposta do servidor...");
    // Aguardando resposta do servidor
    do {
        iResult = recv(LOCAL.ConnectSocket, LOCAL.recvbuf, LOCAL.recvbuflen, 0);
        if ( iResult > 0 ) 
            CLIENTE_TCP_DEBUG("\t\t\t[+] Sucesso! Bytes recebidos: %d\n", iResult);
        else if ( iResult == 0 )
            CLIENTE_TCP_DEBUG("\n\t[+] Procedimento finalizado!\n");
        else
            CLIENTE_TCP_DEBUG("\t\t\t[-] recv() falhou! Erro: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    CLIENTE_TCP_DEBUG("\n\n\t### Bye!!! ###\n");
    closesocket(LOCAL.ConnectSocket);
    WSACleanup();
    return 0;
};

void SharedBuffer::updateRecBuffer(char * _buffer, int len){
    mtx.lock();
    iResult = len;
    ZeroMemory(recBuffer,DEFAULT_BUFLEN);
    memcpy(&recBuffer,_buffer,len);
    mtx.unlock();
} 

void SharedBuffer::readRecBuffer(char * _buffer, int * len){
    mtx.lock();
    *len = iResult;
    memcpy(_buffer,&recBuffer,*len);
    mtx.unlock();
}   

void SharedBuffer::updateSendBuffer(char * _buffer, int len){
    mtx.lock();
    ZeroMemory(sendBuffer,DEFAULT_BUFLEN);
    memcpy(&sendBuffer,_buffer,len);
    mtx.unlock();
} 

void SharedBuffer::readSendBuffer(char * _buffer, int * len){
    mtx.lock();
    memcpy(_buffer,&sendBuffer,*len);
    mtx.unlock();
}   