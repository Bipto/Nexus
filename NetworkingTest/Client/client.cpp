#include <iostream>

#include "Core/Networking/Socket.h"

#include "SDL_net.h"

int main(int argc, char **argv)
{
    if (SDLNet_Init() == -1)
    {
        std::cout << SDLNet_GetError() << std::endl;
        return -1;
    }

    Nexus::SocketTCP *client = Nexus::SocketTCP::CreateClient("localhost", 9999);
    char text[100];
    client->Receive(text, 100);
    std::cout << text << std::endl;

    delete client;

    SDLNet_Quit();

    return 0;
}