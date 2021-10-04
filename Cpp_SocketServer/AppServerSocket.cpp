/*
    Test para el servidor.
    1. Para activar el seguimiento de pasos defina la clave DEBUG_SERVER_TCP
    2. Abrir Putty
        1.1 Escribir la direccion IP del computador y puerto
        1.2 Usar cneccion Raw
    3. enviar datos

    By Yecid Moreno : 2021
    git: https://github.com/YecidMorenoUSP
*/


#include <iostream>
#include <thread>

#define DEBUG_SERVER_TCP
#define DEFAULT_BUFLEN 64

#include "ServerTCP.h"

SharedBuffer app;

typedef struct{
    int stateFes;
    float angle1;
    float angle2;
    float force;
} RecFrameAnklebot;

typedef struct{
    char text [20];
} SendFrameAnklebot;


int main(void) 
{
    SendFrameAnklebot sendFrameAnklebot;
    ZeroMemory(&sendFrameAnklebot,sizeof(SendFrameAnklebot));
    sprintf(sendFrameAnklebot.text,"Hola\n\0");     

    app.updateSendBuffer(sendFrameAnklebot.text,sizeof(sendFrameAnklebot.text));

    ServerTCP server("27015",&app);

    if(server.initServer()) return 1;
    if(server.acceptClient()) return 1;

    server.runLoop();
    
    Sleep(2000);

    RecFrameAnklebot recFrameAnklebot;
    char buffer[DEFAULT_BUFLEN];
    int len;

    for(int i = 0 ; i < 10 ; i++){
        ZeroMemory(buffer,DEFAULT_BUFLEN);
        app.readRecBuffer(buffer,&len);

        // Parse to RecFrameAnklebot format
        memcpy(&recFrameAnklebot,&buffer,sizeof(recFrameAnklebot));

        std::cout<<"["<<len<<"] : "<<buffer<<"  "<< recFrameAnklebot.stateFes <<"\n";
        
        Sleep(200);
    }
    
    if(server.closeServer()) return 1;
    server.waitJoin();

    return 0;
}