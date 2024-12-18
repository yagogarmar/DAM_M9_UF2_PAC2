#include <winsock2.h>
#include <iostream>
#include <thread>
#include <string>

void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Obtiene el manejador de la consola
    SetConsoleTextAttribute(hConsole, color);         // Cambia el color del texto
}

void receive_messages(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            setTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << "Mensaje recibido: " << buffer << std::endl;
            setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Canvia a l'adreça IP del servidor

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error connectant al servidor" << std::endl;
        return 1;
    }

    std::cout << "Connectat al servidor!" << std::endl;

    std::thread recvThread(receive_messages, clientSocket);
    recvThread.detach();

    while (true) {
        std::string message;
        std::getline(std::cin, message);
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
