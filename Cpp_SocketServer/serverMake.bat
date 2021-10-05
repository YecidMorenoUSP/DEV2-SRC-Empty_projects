echo on
g++ .\AppServerSocket.cpp -l Ws2_32 -l wsock32 -o server.exe    
server.exe
