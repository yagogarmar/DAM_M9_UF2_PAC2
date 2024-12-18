#include <winsock2.h>
#include <iostream>

int main() {
    // Inicialitza Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "Error inicialitzant Winsock: " << result << std::endl;
        return 1;
    }

    std::cout << "Sockets configurats correctament" << std::endl;

    // Finalitza Winsock
    WSACleanup();
    return 0;
}