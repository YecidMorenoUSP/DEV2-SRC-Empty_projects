echo on
g++ .\AppClientSocket.cpp -l Ws2_32 -l wsock32 -o client.exe
client.exe

