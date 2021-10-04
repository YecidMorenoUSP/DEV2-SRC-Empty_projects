 #ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
 #endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv) 
{
   WSADATA wsaData;
   SOCKET ConnectSocket = INVALID_SOCKET;
   struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
   char *sendbuf = "this is a test";
   char recvbuf[DEFAULT_BUFLEN];
   int iResult;
   int recvbuflen = DEFAULT_BUFLEN;

// Usage
if (argc != 2) {
    printf("\n\n\t\tCliente Socket TCP/IP by Ighor Augusto\n");
    printf("\t\tUsage: %s endereco do host\n", argv[0]);
    return 1;
}

printf("\n\n\t\t\t\t\t Socket TCP/IP Server by Ighor Augusto\n");
printf("\n\t-> Inicializando a biblioteca de Winsock...");
// Initializando a biblioteca de Winsock
iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
if (iResult != 0) {
    printf("\t\t[-] WSASturtup falhou! Erro: %d\n\n", iResult);
    return 1;
}
printf("\t\t[+] Biblioteca inicializada com sucesso!\n");

ZeroMemory( &hints, sizeof(hints) );
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;

printf("\t-> Resolvendo endereco e porta do servidor...");
// Resolvendo o endereco e porta do servidor
iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
if ( iResult != 0 ) {
    printf("\t\t[-] getaddrinfo() falhou! Erro: %d\n\n", iResult);
    WSACleanup();
    return 1;
}
printf("\t\t[+] Sucesso!\n");

printf("\t-> Iniciando procedimento de conexao...");
// Lanca tentativas de conexao com o host até obter sucesso... ou nao.
for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

    printf("\n\t-> Criando SOCKET para conexao...");
    // Criando socket
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
        ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("\t\t\t\t[-] Socket falhou! Erro: %ld\n\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("\t\t\t[+] Socket criado com sucesso!\n");

    printf("\t-> Conectando ao host...");
    // Connect to server.
    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        continue;
    }
    break;
}

freeaddrinfo(result);

if (ConnectSocket == INVALID_SOCKET) {
    printf("\t\t\t\t[-] Conexao com o server invalida!\n\n");
    WSACleanup();
    return 1;
}
printf("\t\t\t\t[+] Sucesso!!!\n");

printf("\t-> Enviando buffer inicial...");
// Enviando um buffer inicial
iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
if (iResult == SOCKET_ERROR) {
    printf("\t\t\t\t[-] send() falhou! Erro: %d\n\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
}
printf("\t\t\t\t[+] Sucesso! Bytes enviados: %ld\n", iResult);

printf("\t-> Encerrando conexao...");
// Encerrando conexao desde que nao iremos enviar mais buffers
iResult = shutdown(ConnectSocket, SD_SEND);
if (iResult == SOCKET_ERROR) {
    printf("\t\t\t\t[-] shutdown() falhou! Erro: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
}
printf("\t\t\t\t[+] Encerrado!\n");

printf("\t-> Lendo a resposta do servidor...");
// Aguardando resposta do servidor
do {
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if ( iResult > 0 ) 
        printf("\t\t\t[+] Sucesso! Bytes recebidos: %d\n", iResult);
    else if ( iResult == 0 )
        printf("\n\t[+] Procedimento finalizado!\n");
    else
        printf("\t\t\t[-] recv() falhou! Erro: %d\n", WSAGetLastError());

} while( iResult > 0 );

printf("\n\n\t### Bye!!! ###\n");
// cleanup
closesocket(ConnectSocket);
WSACleanup();

return 0;
}