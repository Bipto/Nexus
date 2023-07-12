#include <iostream>

#include "Core/Networking/Socket.hpp"

#include "SDL_net.h"
#include <cstring>

void TCP()
{
    const char *text = "HELLO FROM SERVER!\n";

    Nexus::SocketTCP *server = Nexus::SocketTCP::CreateServer(9999);

    if (server)
    {
        while (true)
        {
            Nexus::SocketTCP *client = server->AcceptConnection();
            if (client)
            {
                client->Send(text, strlen(text) + 1);

                char text[100];
                client->Receive(text, 100);
                std::cout << text << std::endl;
                delete client;
            }
        }
    }

    delete server;
}

void UDP()
{
    Nexus::SocketUDP *incomingSocket = Nexus::SocketUDP::CreateSocket(9999);
    Nexus::PacketUDP *incomingPacket = incomingSocket->CreatePacket(512);

    while (true)
    {
        if (incomingSocket->ReceivePacket(incomingPacket))
        {
            std::cout << "Packet received: ";
            const char *data = (const char *)incomingPacket->GetData();
            std::cout << data << std::endl;

            break;
        }
    }

    delete incomingSocket;
    delete incomingPacket;
}

int main(int argc, char **argv)
{
    if (SDLNet_Init() == -1)
    {
        std::cout << SDLNet_GetError() << std::endl;
        return -1;
    }

    UDP();

    SDLNet_Quit();

    return 0;
}