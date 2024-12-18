#include <winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::vector<SOCKET> clients;
std::mutex clients_mutex;

void handle_client(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            break;
        }

        buffer[bytesReceived] = '\0';

        // Retransmet el missatge a tots els clients
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (SOCKET client : clients) {
            if (client != clientSocket) {
                send(client, buffer, bytesReceived, 0);
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Servidor actiu al port 12345" << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(clientSocket);

        std::thread clientThread(handle_client, clientSocket);
        clientThread.detach();
    }

    WSACleanup();
    return 0;
}
