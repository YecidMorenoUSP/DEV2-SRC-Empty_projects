#if !defined(UTILSTCP_H)
#define UTILSTCP_H


#if !defined(DEFAULT_BUFLEN)
#define DEFAULT_BUFLEN 512
#endif 


#include <mutex> 
#include <windows.h> 



typedef struct {
    char recBuffer[DEFAULT_BUFLEN];
    char sendBuffer[DEFAULT_BUFLEN]; 

    int iResult = 0;
    int lenSendBuffer = 0;

    std::mutex mtx;

    void updateRecBuffer(char * bufferRec, int len);
    void readRecBuffer(char * bufferSend, int * len);

    void updateSendBuffer(char * bufferRec, int len);
    void readSendBuffer(char * bufferSend, int * len);

}SharedBuffer;


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
    lenSendBuffer = len;
    mtx.unlock();
} 

void SharedBuffer::readSendBuffer(char * _buffer, int * len){
    mtx.lock();
    *len = lenSendBuffer;
    memcpy(_buffer,&sendBuffer,*len);
    mtx.unlock();
}   

#endif // UTILSTCP_H
