#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <iostream>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <functional>

class TcpServer {
public:
    TcpServer(int port, std::function<void(int, const std::string&)> onOrderReceived);
    ~TcpServer();
    virtual void start();

    virtual void handleClient(int clientSocket);

private:
    int serverSocket;
    std::vector<std::thread> clientThreads;
    std::function<void(int, const std::string&)> orderHandler;
};

#endif
