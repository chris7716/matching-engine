#include "TCPServer.h"
#include <unistd.h>
#include <arpa/inet.h>

TcpServer::TcpServer(int port, std::function<void(int, const std::string&)> onOrderReceived)
    : orderHandler(std::move(onOrderReceived)) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "TCP Server started on port " << port << std::endl;
}

TcpServer::~TcpServer() {
    close(serverSocket);
}

void TcpServer::start() {
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }

        std::cout << "New client connected" << std::endl;
        clientThreads.emplace_back(&TcpServer::handleClient, this, clientSocket);
    }
}

void TcpServer::handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cout << "Client disconnected" << std::endl;
            close(clientSocket);
            return;
        }

        buffer[bytesRead] = '\0';
        std::string orderMessage(buffer);
        orderHandler(clientSocket, orderMessage);
    }
}
