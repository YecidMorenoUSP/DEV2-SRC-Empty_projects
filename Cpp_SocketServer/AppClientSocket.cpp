/*
    Test para el cliente.
    1. Para activar el seguimiento de pasos defina la clave DEBUG_SERVER_TCP
    2. Abrir server.exe
    3. enviar datos

    By Yecid Moreno : 2021
    git: https://github.com/YecidMorenoUSP
*/


#include <iostream>
#include <thread>

#define DEBUG_CLIENT_TCP
#define DEFAULT_BUFLEN 64

#include "ClientTCP.h"

SharedBuffer app;

typedef struct{
    int stateFes;
    float angle1;
    float angle2;
    float force;
} RecFrameAnklebot;

typedef struct{
    char text [32];
} SendFrameAnklebot;




int main(void) 
{   
    SendFrameAnklebot sendFrameAnklebot;
    ZeroMemory(&sendFrameAnklebot,sizeof(SendFrameAnklebot));
    sprintf(sendFrameAnklebot.text,"Hola\n\0");     
    app.updateSendBuffer(sendFrameAnklebot.text,sizeof(sendFrameAnklebot.text));

    ClientTCP client("127.0.0.1","27015",&app);
    if(client.initClient()) return 1;
    if(client.tryConnect()) return 1;
    client.runLoop();

    Sleep(2000);


    char buffer[DEFAULT_BUFLEN];
    int len;

    for(int i = 0 ; i < 10 ; i++){
        ZeroMemory(buffer,DEFAULT_BUFLEN);
        app.readRecBuffer(buffer,&len);

        std::cout<<"["<<len<<"] : "<<buffer<<"\n";
        
        Sleep(200);
    }

    if(client.closeClient());
    client.waitJoin();

    printf("[END]");

    return 0;
}