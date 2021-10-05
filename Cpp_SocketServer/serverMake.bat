echo on
g++ .\AppServerSocket.cpp -l Ws2_32 -l wsock32 -o server.exe
:: gcc .\ClientSocket.cpp -l Ws2_32 -o client.exe
server.exe
:: client.exe 127.0.0.1
