echo on
g++ .\AppClientSocket.cpp -l Ws2_32 -l wsock32 -o client.exe
:: gcc .\ClientSocket.cpp -l Ws2_32 -o client.exe
@REM start server.exe
client.exe
:: client.exe 127.0.0.1
