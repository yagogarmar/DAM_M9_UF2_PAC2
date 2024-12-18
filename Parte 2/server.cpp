#include <winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm> // Para std::find
#include <map>
std::vector<SOCKET> clients;
std::mutex clients_mutex;
std::vector<int> clientes;
void setTextColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Obtiene el manejador de la consola
    SetConsoleTextAttribute(hConsole, color);          // Cambia el color del texto
}
std::map<int, std::string> numeroMensaje;                 // Mapa para asociar números con mensajes
std::string mensajes[] = {"1", "2", "3"};                 // Los tres mensajes
int numMensajes = sizeof(mensajes) / sizeof(mensajes[0]); // Cantidad de mensajes disponibles
int index = 0;                                            // Índice del mensaje actual

void handle_client(SOCKET clientSocket)
{
    char buffer[1024];

    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            closesocket(clientSocket);
            break;
        }

        buffer[bytesReceived] = '\0';
        // GESTION DE LOS COLORES DE LOS CLIENTES >_<

        if (numeroMensaje.find(clientSocket) == numeroMensaje.end())
        {
            // Si no tiene, asignar el siguiente mensaje disponible
            numeroMensaje[clientSocket] = mensajes[index];
            index = (index + 1) % numMensajes; // Ciclar entre los mensajes
        }
        if (numeroMensaje[clientSocket] == "1")
        {
            setTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        }
        else if (numeroMensaje[clientSocket] == "2")
        {
            setTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }
        else
        {
            setTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        }

        std::cout << "Mensaje de " << clientSocket << ": " << buffer << std::endl;
        setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        // Retransmet el missatge a tots els clients
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (SOCKET client : clients)
        {
            if (client != clientSocket)
            {
                send(client, buffer, bytesReceived, 0);
            }
        }
    }
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Servidor actiu al port 12345" << std::endl;

    while (true)
    {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);

        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(clientSocket);

        std::thread clientThread(handle_client, clientSocket);
        clientThread.detach();
    }

    WSACleanup();
    return 0;
}
