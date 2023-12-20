#include <iostream>

#include "Nexus/Networking/Socket.hpp"

#include "SDL_net.h"

void TCP()
{
    Nexus::IPAddress address = Nexus::IPAddress::Create("localhost", 9999);
    Nexus::SocketTCP *client = Nexus::SocketTCP::CreateClient(address);
    char text[100];
    client->Receive(text, 100);
    std::cout << text << std::endl;

    const char *message = "HELLO FROM CLIENT\n";
    client->Send(message, strlen(message) + 1);

    delete client;
}

void UDP()
{
    Nexus::SocketUDP *socket = Nexus::SocketUDP::CreateSocket(0);
    Nexus::IPAddress target = Nexus::IPAddress::Create("localhost", 9999);

    Nexus::PacketUDP *packet = socket->CreatePacket(512);
    const char *text = "HELLO FROM CLIENT!\n";
    packet->SetTarget(target);
    packet->SetData(text, strlen(text) + 1);
    socket->SendPacket(packet);

    delete socket;
    delete packet;
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